/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "modules/data/config.hpp"

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

#include <service/IController.hpp>

namespace sight::module::data
{

/**
 * @brief This service extracts camera parameters from a camera series
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::data::SGetCamera" >
    <inout key="cameraSeries" uid="..."/>
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
 * - \b cameraSeries [sight::data::SGetCamera]: camera series from which the parameters will be extracted
 *
 * @subsection Output Output
 * - \b camera [sight::data::Camera]: output camera extracted from the input Camera series.
 * - \b extrinsic [sight::data::Matrix4]: define the extrinsic matrix data.
 */

class MODULE_DATA_CLASS_API SGetCamera : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SGetCamera, service::IController);

    /// Constructor
    MODULE_DATA_API SGetCamera() = default;

    /// Destructor
    MODULE_DATA_API ~SGetCamera() = default;

protected:

    /// Does nothing
    MODULE_DATA_API void starting() override;

    /// Does nothing
    MODULE_DATA_API void configuring() override;

    /// Does nothing
    MODULE_DATA_API void stopping() override;

    /// Extract an image
    MODULE_DATA_API void updating() override;

private:

    std::vector<std::size_t> m_cameraIndexNumbers;
    std::vector<std::size_t> m_extrinsicIndexNumbers;

    sight::data::ptr<sight::data::CameraSeries, sight::data::Access::inout> m_cameraSeries {this, "cameraSeries"};
    sight::data::ptr_vector<sight::data::Camera, sight::data::Access::out> m_camera {this, "camera"};
    sight::data::ptr_vector<sight::data::Matrix4, sight::data::Access::out> m_extrinsic {this, "extrinsic"};
};

} // namespace sight::module::data
