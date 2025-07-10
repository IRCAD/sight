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

#include <data/line.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::line
{

constexpr static auto POSITION {"Position"};
constexpr static auto DIRECTION {"Direction"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto line = helper::safe_cast<data::line>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::line>(_tree, 1);

    _children[POSITION]  = line->get_position();
    _children[DIRECTION] = line->get_direction();
}

//------------------------------------------------------------------------------

inline static data::line::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto line = helper::cast_or_create<data::line>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::line>(_tree, 0, 1);

    line->set_position(std::dynamic_pointer_cast<data::point>(_children.at(POSITION)));
    line->set_direction(std::dynamic_pointer_cast<data::point>(_children.at(DIRECTION)));

    return line;
}

SIGHT_REGISTER_SERIALIZER(data::line, write, read);

} // namespace sight::io::session::detail::line
