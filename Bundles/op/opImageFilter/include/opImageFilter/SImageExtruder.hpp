/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "opImageFilter/config.hpp"

#include <fwData/Mesh.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/IOperator.hpp>

#include <glm/vec3.hpp>

namespace opImageFilter
{

/**
 * @brief This service sets all voxel inside meshes to -1000.
 *
 * To compute this quickly, we loop over two dimensions out of three, for each voxel, we launch a ray on the third
 * dimension and get a list of intersections between this ray, and triangles of the mesh. After that, we iterate over
 * the voxel line on the third dimension and look where it's located relatively to intersections, it allows to know if
 * the voxel is inside or outside of the mesh.
 *
 * @pre The input image must be in 3D.
 * @warning The image and meshes must be used without transformatino matrices in 3D scene, no matrix are used here.
 *
 * @section Slots Slots
 * - \b addReconstructions(::fwMedData::ModelSeries::ReconstructionVectorType): extrude reconstructions from the image.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::opImageFilter::SImageExtruder">
        <in key="meshes" uid="..." />
        <in key="image" uid="..." />
        <inout key="extrudedImage" uid="..." />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b meshes [::fwMedData::ModelSeries]: model series where all meshes used for extrusion are stored.
 * - \b image [::fwData::Image]: image to extrude, must be in 3 dimensions.
 *
 * @subsection In-Out In-Out
 * - \b extrudedImage [::fwData::Image]: extruded image.
 */
class OPIMAGEFILTER_CLASS_API SImageExtruder : public ::fwServices::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SImageExtruder, ::fwServices::IOperator)

    /// Initializes the slot.
    OPIMAGEFILTER_API SImageExtruder();

    /// Destroys the service.
    OPIMAGEFILTER_API ~SImageExtruder();

private:

    /// Does nothing.
    virtual void configuring() override;

    /// Starts the service.
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwMedData::ModelSeries::s_MODIFIED_SIG of s_MESHES_INPUT to s_UPDATE_SLOT.
     * Connect ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG of s_MESHES_INPUT to s_ADD_RECONSTRUCTIONS_SLOT.
     * Connect ::fwMedData::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG of s_MESHES_INPUT to s_UPDATE_SLOT.
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_IMAGE_INPUT to s_UPDATE_SLOT.
     * Connect ::fwData::Image::s_BUFFER_MODIFIED_SIG of s_IMAGE_INPUT to s_UPDATE_SLOT.
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Extrudes all mesh of the model series from the image.
    virtual void updating() override;

    /// Stops the services.
    virtual void stopping() override;

    /// SLOT: called when reconstructions are added to the model series.
    void addReconstructions(::fwMedData::ModelSeries::ReconstructionVectorType _reconstructions) const;

    /// Extrudes one mesh from the image.
    void extrudeMesh(const ::fwData::Mesh::csptr _mesh) const;

};

} // namespace opImageFilter.
