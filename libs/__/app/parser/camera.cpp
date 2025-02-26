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

#include "app/parser/camera.hpp"

#include <core/runtime/path.hpp>

#include <data/camera.hpp>

namespace sight::app::parser
{

//------------------------------------------------------------------------------

void camera::parse(
    const service::config_t& _cfg,
    core::object::sptr _obj,
    objects_t&
    /*_sub_objects*/
)
{
    const auto camera = std::dynamic_pointer_cast<data::camera>(_obj);
    SIGHT_ASSERT("camera does not exist.", camera);

    std::string node("<xmlattr>.resource");
    auto input = _cfg.get_optional<std::string>(node);
    if(not input.has_value())
    {
        node  = "resource";
        input = _cfg.get_optional<std::string>(node);
    }

    bool uses_absolute_path = !input.has_value();
    if(not input.has_value()) // If we still have no tag then it might be an absolute path configuration.
    {
        uses_absolute_path = true;
        node               = "<xmlattr>.file";
        input              = _cfg.get_optional<std::string>(node);
        if(not input.has_value())
        {
            node  = "file";
            input = _cfg.get_optional<std::string>(node);
        }
    }

    if(input.has_value())
    {
        std::string file_name;
        file_name = _cfg.get<std::string>(node);

        std::filesystem::path file;
        if(uses_absolute_path)
        {
            file = std::filesystem::path(file_name);
        }
        else
        {
            file = sight::core::runtime::get_resource_file_path(file_name);
        }

        camera->set_video_file(file);
        camera->set_camera_source(data::camera::source_t::file);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::parser
