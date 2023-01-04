/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/filter/mesh/config.hpp"

#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>

#include <service/IFilter.hpp>

#include <string>

namespace sight::module::filter::mesh
{

/**
 * @brief This service generates meshes from an ImageSeries using VTK library.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::mesh::SVTKMesher" >
       <in key="imageSeries" uid="..."/>
       <out key="modelSeries" uid="..." />
       <config percentReduction="80" threshold="255" />
    </service>
    @endcode
 *
 * @subsection Input Input
 * - \b imageSeries [sight::data::ImageSeries] : image used to generate the output mesh
 * @subsection Output Output
 * - \b modelSeries [sight::data::ModelSeries]: mesh generated from ImageSeries
 * @subsection Configuration Configuration
 * - \b percentReduction : Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)
 * - \b threshold : This value is used for threshold of the image means that Pixels below threshold value are converted
 *      to black (bit value of zero), and pixels above the threshold value are converted to white (a bit value of one).
 */
class MODULE_FILTER_MESH_CLASS_API SVTKMesher : public service::IFilter
{
public:

    SIGHT_DECLARE_SERVICE(SVTKMesher, sight::service::IFilter);

    MODULE_FILTER_MESH_API SVTKMesher() noexcept;
    MODULE_FILTER_MESH_API ~SVTKMesher() noexcept override = default;

protected:

    MODULE_FILTER_MESH_API void starting() override;

    MODULE_FILTER_MESH_API void stopping() override;

    MODULE_FILTER_MESH_API void configuring() override;

    MODULE_FILTER_MESH_API void updating() override;

private:

    unsigned int m_reduction {0};
    unsigned int m_threshold {0};
    void updateThreshold(int);

    /// Input image mask
    data::ptr<data::ImageSeries, data::Access::in> m_image {this, "imageSeries", false};
    /// Output segmentation
    data::ptr<data::ModelSeries, data::Access::out> m_model {this, "modelSeries", false};
};

} // namespace sight::module::filter::mesh
