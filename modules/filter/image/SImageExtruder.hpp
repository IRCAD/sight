/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/filter/image/config.hpp"

#include <data/Image.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief This service sets all voxels of an image that are inside meshes to the lowest value of the image type.
 *
 * To compute this quickly, we loop over two dimensions out of three, for each voxel, we launch a ray on the third
 * dimension and get a list of intersections between this ray, and triangles of the mesh. After that, we iterate over
 * the voxel line on the third dimension and look where it's located relatively to intersections, it allows to know if
 * the voxel is inside or outside of the mesh.
 *
 * @pre The input image must be in 3D.
 * @pre Input meshes must have cells with 3 or 4 points.
 * @warning The image and meshes must be used without transformation matrices in 3D scene, no matrices are used here.
 *
 * @section Slots Slots
 * - \b addReconstructions(data::ModelSeries::ReconstructionVectorType): extrude reconstructions from the image.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::filter::image::SImageExtruder">
        <in key="meshes" uid="..." />
        <in key="image" uid="..." />
        <in key="transform" uid="..." />
        <inout key="mask" uid="..." />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b meshes [sight::data::ModelSeries]: model series where all meshes used for extrusion are stored.
 * - \b image [sight::data::Image]: image to extrude, must be in 3 dimensions.
 * - \b transform [sight::data::Matrix4] (optional): transform matrix of the image.
 *
 * @subsection In-Out In-Out
 * - \b mask [sight::data::Image]: resulting mask. Cropped regions are zeroed and full regions are marked with 255.
 */
class MODULE_FILTER_IMAGE_CLASS_API SImageExtruder final : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SImageExtruder, sight::service::filter);

    /// Initializes the slot.
    MODULE_FILTER_IMAGE_API SImageExtruder();

    /// Destroys the service.
    MODULE_FILTER_IMAGE_API ~SImageExtruder() override;

private:

    /// Does nothing.
    void configuring() override;

    /// Starts the service.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::ModelSeries::MODIFIED_SIG of s_MESHES_INPUT to service::slots::UPDATE.
     * Connect data::ModelSeries::RECONSTRUCTIONS_ADDED_SIG of s_MESHES_INPUT to ADD_RECONSTRUCTIONS_SLOT.
     * Connect data::ModelSeries::RECONSTRUCTIONS_REMOVED_SIG of s_MESHES_INPUT to service::slots::UPDATE.
     * Connect data::Matrix4::MODIFIED_SIG of s_TRANSFORM_INPUT to service::slots::UPDATE.
     * Connect data::Image::MODIFIED_SIG of s_IMAGE_INPUT to service::slots::UPDATE.
     * Connect data::Image::BUFFER_MODIFIED_SIG of s_IMAGE_INPUT to service::slots::UPDATE.
     */
    connections_t getAutoConnections() const override;

    /// Extrudes all mesh of the model series from the image.
    void updating() override;

    /// Stops the services.
    void stopping() override;

    /// SLOT: called when reconstructions are added to the model series.
    void addReconstructions(data::ModelSeries::ReconstructionVectorType _reconstructions) const;

    static constexpr std::string_view s_MESHES_INPUT    = "meshes";
    static constexpr std::string_view s_IMAGE_INPUT     = "image";
    static constexpr std::string_view s_TRANSFORM_INPUT = "transform";
    static constexpr std::string_view s_IMAGE_INOUT     = "mask";

    sight::data::ptr<sight::data::ModelSeries, sight::data::Access::in> m_meshes {this, s_MESHES_INPUT, true};
    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_image {this, s_IMAGE_INPUT, true};
    sight::data::ptr<sight::data::Matrix4, sight::data::Access::in> m_transform {this, s_TRANSFORM_INPUT, true, true};
    sight::data::ptr<sight::data::Image, sight::data::Access::inout> m_extrudedImage {this, s_IMAGE_INOUT};
};

} // namespace sight::module::filter::image.
