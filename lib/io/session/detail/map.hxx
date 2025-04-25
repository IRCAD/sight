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

#include <data/map.hpp>

namespace sight::io::session::detail::map
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
    const auto map = helper::safe_cast<data::map>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::map>(_tree, 1);

    // map is map of child object..
    _children = std::map<std::string, data::object::csptr>(map->cbegin(), map->cend());
}

//------------------------------------------------------------------------------

inline static data::map::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto map = helper::cast_or_create<data::map>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::map>(_tree, 0, 1);

    map->operator=(_children);

    return map;
}

SIGHT_REGISTER_SERIALIZER(data::map, write, read);
SIGHT_REGISTER_SERIALIZER_DEPRECATED("sight::data::map", read);

} // namespace sight::io::session::detail::map
