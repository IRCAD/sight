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

#include <data/point.hpp>

namespace sight::io::session::detail::point
{

constexpr static auto X {"X"};
constexpr static auto Y {"Y"};
constexpr static auto Z {"Z"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto point = helper::safe_cast<data::point>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::point>(_tree, 1);

    const auto& coordinate = point->get_coord();
    _tree.put(X, coordinate[0]);
    _tree.put(Y, coordinate[1]);
    _tree.put(Z, coordinate[2]);
}

//------------------------------------------------------------------------------

inline static data::point::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto point = helper::cast_or_create<data::point>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::point>(_tree, 0, 1);

    const std::array<double, 3> coordinates = {
        _tree.get<double>(X),
        _tree.get<double>(Y),
        _tree.get<double>(Z)
    };

    point->set_coord(coordinates);

    return point;
}

SIGHT_REGISTER_SERIALIZER(data::point, write, read);

} // namespace sight::io::session::detail::point
