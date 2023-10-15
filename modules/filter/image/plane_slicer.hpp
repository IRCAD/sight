/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/filter/image/config.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>

#include <service/filter.hpp>

#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

namespace sight::module::filter::image
{

/**
 * @brief Operator computing a 2D slice from a 3D image.
 *
 * @section Slots Slots
 * - \b updateSliceType(int, int) : change slice
 * - \b updateDefaultValue() : set the image minimum as the reslicer background value.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::filter::image::plane_slicer" auto_connect="true">
       <in key="image" uid="..." />
       <in key="imageExtent" uid="..." />
       <in key="axes" uid="..." />
       <inout key="slice" uid="..." />
       <config orientation="axial" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::image]: image from which the slice is extracted.
 * - \b imageExtent [sight::data::image]: image used to pass the extent.
 * - \b axes [sight::data::matrix4]: Matrix containing axes defining the reslicing plane.
 *
 * @subsection In-Out In-Out
 * - \b slice [sight::data::image]: Extracted slice.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : operator configuration
 *    - \b orientation (mandatory, values=axial|sagital|frontal): image orientation, determines the slicing plane axes.
 */

class MODULE_FILTER_IMAGE_CLASS_API plane_slicer : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(plane_slicer, service::filter);

    /// Constructor.
    MODULE_FILTER_IMAGE_API plane_slicer() noexcept;

    /// Destructor.
    MODULE_FILTER_IMAGE_API ~plane_slicer() noexcept override;

protected:

    MODULE_FILTER_IMAGE_API void configuring() override;
    MODULE_FILTER_IMAGE_API void starting() override;
    MODULE_FILTER_IMAGE_API void updating() override;
    MODULE_FILTER_IMAGE_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     *
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::BUFFER_MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::BUFFER_MODIFIED_SIG to this::UPDATE_DEFAULT_VALUE_SLOT
     * Connect imageExtent::SLICE_INDEX_MODIFIED_SIG to this::UPDATE_SLICE_TYPE_SLOT
     * Connect imageExtent::SLICE_TYPE_MODIFIED_SIG to this::service::slots::UPDATE
     * Connect axes::MODIFIED_SIG to this::service::slots::UPDATE
     */
    MODULE_FILTER_IMAGE_API connections_t auto_connections() const override;

private:

    /// Set the extent based on the extentImage and orientation.
    void setReslicerExtent();

    /// Set the plane axes using the input transform.
    void setReslicerAxes();

    /// Modify the matrix parameter to align the plane with the current extent image slice.
    void applySliceTranslation(vtkSmartPointer<vtkMatrix4x4> vtkMat) const;

    /// Slot: called when the extent image orientation.
    void updateorientation_t(int from, int to);

    /// Slot: sets the reslicer default value which is the minimum value of the input image.
    void updateDefaultValue();

    /// Slicing orientation. Determines the two axes defining the plane.
    data::helper::MedicalImage::orientation_t m_orientation {data::helper::MedicalImage::orientation_t::Z_AXIS};

    /// Vtk reslicing algorithm.
    vtkSmartPointer<vtkImageReslice> m_reslicer;

    static constexpr std::string_view s_IMAGE_IN    = "image";
    static constexpr std::string_view s_EXTENT_IN   = "imageExtent";
    static constexpr std::string_view s_AXES_IN     = "axes";
    static constexpr std::string_view s_SLICE_INOUT = "slice";

    sight::data::ptr<sight::data::image, sight::data::Access::in> m_image {this, s_IMAGE_IN};
    sight::data::ptr<sight::data::image, sight::data::Access::in> m_extent {this, s_EXTENT_IN};
    sight::data::ptr<sight::data::matrix4, sight::data::Access::in> m_axes {this, s_AXES_IN};
    sight::data::ptr<sight::data::image, sight::data::Access::inout> m_slice {this, s_SLICE_INOUT};
};

} //namespace sight::module::filter::image
