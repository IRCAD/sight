/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#include <data/boolean.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

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
 * @section Signals Signals
 * - \b slice_range_changed(): send the minimum and maximum translation value in the reslice direction.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::filter::image::plane_slicer" auto_connect="true">
       <input image="${...}" axes="${...}" offset="${...}" />
       <output slice="${...}" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::image]: image from which the slice is extracted.
 * - \b axes [sight::data::matrix4]: matrix containing axes defining the reslicing plane.
 * - \b offset [sight::data::matrix4] (optional): matrix containing an extra offset, for instance allowing to translate
 * along the direction plane (only set a translation in Z for this).
 *
 * @subsection In-Out In-Out
 * - \b slice [sight::data::image]: Extracted slice.
 *
 * @subsection Properties Properties
 * - \b center [sight::data::boolean]: Center the reslice at the location of the matrix.
 */
class plane_slicer final : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(plane_slicer, service::filter);

    struct signals
    {
        using slice_range_changed_t = core::com::signal<void (double, double)>;
        static inline const signal_key_t SLICE_RANGE_CHANGED = "slice_range_changed";
    };

    struct slots
    {
        static inline const slot_key_t UPDATE_DEFAULT_VALUE = "update_default_value";
    };

    /// Constructor.
    plane_slicer() noexcept;

    /// Destructor.
    ~plane_slicer() noexcept final = default;

protected:

    void configuring() final;
    void starting() final;
    void updating() final;
    void stopping() final;

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     *
     * Connect data::signals::MODIFIED to this::service::slots::UPDATE
     * Connect image::signals::BUFFER_MODIFIED to this::service::slots::UPDATE
     * Connect data::signals::MODIFIED to slots::UPDATE_DEFAULT_VALUE
     * Connect image::signals::BUFFER_MODIFIED to slots::UPDATE_DEFAULT_VALUE
     * Connect data::signals::MODIFIED to this::service::slots::UPDATE
     */
    connections_t auto_connections() const final;

private:

    /// Slot: sets the reslicer default value which is the minimum value of the input image.
    void update_default_value();

    /// Vtk reslicing algorithm.
    vtkSmartPointer<vtkImageReslice> m_reslicer;

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, "input.image"};
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_axes {this, "input.axes"};
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_offset {this, "input.offset", true};
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_slice {this, "output.slice"};

    sight::data::ptr<sight::data::boolean, sight::data::access::in> m_center {this, "config.center", false};
    sight::data::ptr<sight::data::string, sight::data::access::in> m_interpolation {
        this, "config.interpolation", std::string("LINEAR")
    };
};

} //namespace sight::module::filter::image
