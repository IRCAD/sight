/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "io/session/config.hpp"
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>

namespace sight::io::session::detail::camera_set
{

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto camera_set = helper::safe_cast<data::camera_set>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::camera_set>(tree, 1);

    for(std::size_t index = 0, end = camera_set->size() ; index < end ; ++index)
    {
        const auto& pair = camera_set->at(index);

        const std::string& index_string = std::to_string(index);
        children[data::camera_set::classname() + data::camera::classname() + index_string]  = pair.first;
        children[data::camera_set::classname() + data::matrix4::classname() + index_string] = pair.second;
    }
}

//------------------------------------------------------------------------------

inline static data::camera_set::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto camera_set = helper::cast_or_create<data::camera_set>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::camera_set>(tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    camera_set->clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const std::string& index_string = std::to_string(index);

        const auto& camera_it = children.find(data::camera_set::classname() + data::camera::classname() + index_string);

        if(camera_it == children.cend())
        {
            break;
        }

        const auto& camera = std::dynamic_pointer_cast<data::camera>(camera_it->second);

        if(!camera)
        {
            break;
        }

        const auto& matrix_it =
            children.find(data::camera_set::classname() + data::matrix4::classname() + index_string);

        if(matrix_it == children.cend())
        {
            break;
        }

        const auto& matrix = std::dynamic_pointer_cast<data::matrix4>(matrix_it->second);

        if(!matrix)
        {
            break;
        }

        camera_set->push_back(std::make_pair(camera, matrix));
    }

    return camera_set;
}

SIGHT_REGISTER_SERIALIZER(data::camera_set, write, read);

} // namespace sight::io::session::detail::camera_set
