/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include <data/camera.hpp>
#include <data/camera_set.hpp>

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief This service extracts camera parameters from a camera set
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::data::get_camera" >
    <inout key="camera_set" uid="..."/>
    <out group="camera" >
        <key index="0" uid="..." />
        <key index="1" uid="..." />
    </out>
    <out group="extrinsic" >
        <key index="1" uid="..." />
    </out>
   </service>
       @endcode
 * @subsection Inout Inout
 * - \b camera_set [sight::data::get_camera]: camera series from which the parameters will be extracted
 *
 * @subsection Output Output
 * - \b camera [sight::data::camera]: output camera extracted from the input Camera series.
 * - \b extrinsic [sight::data::matrix4]: define the extrinsic matrix data.
 */

class get_camera : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(get_camera, service::controller);

    /// Constructor
    get_camera() = default;

    /// Destructor
    ~get_camera() override = default;

protected:

    /// Does nothing
    void starting() override;

    /// Does nothing
    void configuring() override;

    /// Does nothing
    void stopping() override;

    /// Extract an image
    void updating() override;

private:

    std::vector<std::size_t> m_camera_index_numbers;
    std::vector<std::size_t> m_extrinsic_index_numbers;

    sight::data::ptr<sight::data::camera_set, sight::data::access::inout> m_camera_set {this, "camera_set"};
    sight::data::ptr_vector<sight::data::camera, sight::data::access::out> m_camera {this, "camera"};
    sight::data::ptr_vector<sight::data::matrix4, sight::data::access::out> m_extrinsic {this, "extrinsic"};
};

} // namespace sight::module::data
