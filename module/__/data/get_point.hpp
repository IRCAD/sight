/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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
#include <data/point.hpp>

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief Extracts fiducials from an image series into individual points.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::data::get_point" >
            <in key="image" uid="..." />
            <inout group="points">
                <key group="..." index="..." uid="..."/>
            </inout>
        </service>
       @endcode
 * @subsection Input Input
 * - \b image [sight::data::image_series]: image containing the fiducials.
 *
 * @subsection InOut InOut
 * - \b point [sight::data::point]: output points.
 * *
 * @subsection Configuration Configuration
 * - \b group (mandatory) : name of the fiducials group.
 * - \b index (mandatory) : index of the fiducial in the group.
 */
class get_point : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(get_point, service::controller);

    /// Constructor
    get_point() = default;

    /// Destructor
    ~get_point() override = default;

protected:

    /// Does nothing
    void starting() override;

    /// Configure the service
    void configuring() override;

    /// Does nothing
    void stopping() override;

    /// Extract the object(s)
    void updating() override;

private:

    /// structure which contains the element of an index configuration:
    /// inputIndex: index of the mesh in the modelSeries
    /// outputIndex: index of the output variable
    struct point_config
    {
        std::string group;
        std::size_t index;
    };

    /// The list of configurations.
    std::vector<point_config> m_configuration;

    /// The in/out variables
    sight::data::ptr<sight::data::image_series, sight::data::access::in> m_image {this, "image"};
    sight::data::ptr_vector<sight::data::point, sight::data::access::inout> m_points {this, "points"};
};

} // namespace sight::module::data
