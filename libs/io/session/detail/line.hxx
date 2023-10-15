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

#include <data/line.hpp>
#include <data/point.hpp>

namespace sight::io::session::detail::line
{

constexpr static auto s_Position {"Position"};
constexpr static auto s_Direction {"Direction"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto line = helper::safe_cast<data::line>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::line>(tree, 1);

    children[s_Position]  = line->getPosition();
    children[s_Direction] = line->getDirection();
}

//------------------------------------------------------------------------------

inline static data::line::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto line = helper::cast_or_create<data::line>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::line>(tree, 0, 1);

    line->setPosition(std::dynamic_pointer_cast<data::point>(children.at(s_Position)));
    line->setDirection(std::dynamic_pointer_cast<data::point>(children.at(s_Direction)));

    return line;
}

SIGHT_REGISTER_SERIALIZER(data::line, write, read);

} // namespace sight::io::session::detail::line
