/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/model_series.hpp>
#include <data/real.hpp>

#include <service/filter.hpp>

#include <string>

namespace sight::module::filter::mesh
{

/**
 * @brief This service generates meshes from an ImageSeries using VTK library.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::filter::mesh::vtk_mesher" >
       <in key="imageSeries" uid="..."/>
       <out key="modelSeries" uid="..." />
       <properties percent_reduction="80" threshold="255" />
    </service>
    @endcode
 *
 * @subsection Input Input
 * - \b imageSeries [sight::data::image_series] : image used to generate the output mesh
 * @subsection Output Output
 * - \b modelSeries [sight::data::model_series]: mesh generated from ImageSeries
 * @subsection Properties Properties
 * - \b percent_reduction : Specify the desired reduction in the total number of polygons (e.g., if
 *      TargetReduction is set to 90, this filter will try to reduce the data set to 10% of its original size)
 * - \b threshold : This value is used for threshold of the image means that Pixels below threshold value are converted
 *      to black (bit value of zero), and pixels above the threshold value are converted to white (a bit value of one).
 */
class vtk_mesher : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(vtk_mesher, sight::service::filter);

    vtk_mesher() noexcept;
    ~vtk_mesher() noexcept override = default;

protected:

    void starting() override;

    void stopping() override;

    void configuring() override;

    void updating() override;

    connections_t auto_connections() const override;

private:

    /// Input image mask
    data::ptr<data::image_series, data::access::in> m_image {this, "imageSeries"};
    /// Output segmentation
    data::ptr<data::model_series, data::access::out> m_model {this, "modelSeries"};

    data::property<data::real> m_reduction {this, "percent_reduction", 0.};
    data::property<data::integer> m_threshold {this, "threshold", 0};
};

} // namespace sight::module::filter::mesh
