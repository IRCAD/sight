/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <sight/app/config.hpp>

#include <service/object_parser.hpp>

namespace sight::app::parser
{

/**
 * @brief   Specific service that allows to initialize a sight::data::camera.
 * Currently this only allows to initialize the camera by using a filepath, path can either be sight resources or
 * absolute filesystem files.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <object uid="camera" type="sight::data::camera" >
        <resource>sight::io:resources/my/video</resource>
        <file>/absolute/path/to/my/video</file>
    </object>
 * @endcode
 * @subsection Configuration Configuration:
 * - \b resource(optional): : sight-like resource path to a video file for default mode.
 * - \b file(optional): absolute filesystem path to a video file for default mode.
 * @see     service::object_parser
 */
class SIGHT_APP_CLASS_API camera : public service::object_parser
{
public:

    SIGHT_DECLARE_SERVICE(camera, service::object_parser);

    SIGHT_APP_API void parse(
        const service::config_t& _cfg,
        core::object::sptr _obj,
        objects_t&
    ) override;
};

} // namespace sight::app::parser
