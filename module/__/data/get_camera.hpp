/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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
    <input camera_set="${...}" />
    <output camera="${...}" index="0" />
    <output camera="${...}" index="1" />
    <output extrinsic="${...}" index="1" />
   </service>
       @endcode
 * @subsection Inout Inout
 * - \b input.camera_set [sight::data::camera_set]: camera series from which the parameters will be extracted
 *
 * @subsection Output Output
 * - \b output.camera [sight::data::camera]: output camera extracted from the input Camera series.
 * - \b output.extrinsic [sight::data::matrix4]: define the extrinsic matrix data.
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

    sight::data::ptr<sight::data::camera_set, sight::data::access::inout> m_camera_set {this, "input.camera_set"};
    sight::data::ptr_vector<sight::data::camera, sight::data::access::out> m_camera {this, "output.camera"};
    sight::data::ptr_vector<sight::data::matrix4, sight::data::access::out> m_extrinsic {this, "output.extrinsic"};
};

} // namespace sight::module::data
