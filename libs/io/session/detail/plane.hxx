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

#include <data/plane.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::plane
{

constexpr static auto IS_INTERSECTION {"IsIntersection"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto plane = helper::safe_cast<data::plane>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::plane>(_tree, 1);

    _tree.put(IS_INTERSECTION, plane->get_is_intersection());

    std::size_t index = 0;
    for(const auto& point : plane->get_points())
    {
        _children[data::point::classname() + std::to_string(index++)] = point;
    }
}

//------------------------------------------------------------------------------

inline static data::plane::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto plane = helper::cast_or_create<data::plane>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::plane>(_tree, 0, 1);

    plane->set_is_intersection(_tree.get<bool>(IS_INTERSECTION));

    auto& points = plane->get_points();

    for(std::size_t index = 0, end = _children.size() ; index < end ; ++index)
    {
        const auto& it = _children.find(data::point::classname() + std::to_string(index));

        if(it == _children.cend())
        {
            break;
        }

        points[index] = std::dynamic_pointer_cast<data::point>(it->second);
    }

    return plane;
}

SIGHT_REGISTER_SERIALIZER(data::plane, write, read);

} // namespace sight::io::session::detail::plane
