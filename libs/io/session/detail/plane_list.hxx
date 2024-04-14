/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/io/session/config.hpp>

#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/plane_list.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::plane_list
{

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto plane_list = helper::safe_cast<data::plane_list>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::plane_list>(_tree, 1);

    // Add points to children list
    std::size_t index = 0;
    for(const auto& plane : plane_list->get_planes())
    {
        _children[data::plane::classname() + std::to_string(index++)] = plane;
    }
}

//------------------------------------------------------------------------------

inline static data::plane_list::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto plane_list = helper::cast_or_create<data::plane_list>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::plane_list>(_tree, 0, 1);

    // Deserialize planes
    auto& planes = plane_list->get_planes();

    // Clearing is required in case the object is reused
    planes.clear();

    for(std::size_t index = 0, end = _children.size() ; index < end ; ++index)
    {
        const auto& it = _children.find(data::plane::classname() + std::to_string(index));

        if(it == _children.cend())
        {
            break;
        }

        planes.push_back(std::dynamic_pointer_cast<data::plane>(it->second));
    }

    return plane_list;
}

SIGHT_REGISTER_SERIALIZER(data::plane_list, write, read);

} // namespace sight::io::session::detail::plane_list
