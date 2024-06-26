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

#include <data/color.hpp>

namespace sight::io::session::detail::color
{

constexpr static auto RED {"Red"};
constexpr static auto GREEN {"Green"};
constexpr static auto BLUE {"Blue"};
constexpr static auto ALPHA {"Alpha"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto color = helper::safe_cast<data::color>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::color>(_tree, 1);

    _tree.put(RED, color->red());
    _tree.put(GREEN, color->green());
    _tree.put(BLUE, color->blue());
    _tree.put(ALPHA, color->alpha());
}

//------------------------------------------------------------------------------

inline static data::color::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto color = helper::cast_or_create<data::color>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::color>(_tree, 0, 1);

    color->set_rgba(
        _tree.get<float>(RED),
        _tree.get<float>(GREEN),
        _tree.get<float>(BLUE),
        _tree.get<float>(ALPHA)
    );

    return color;
}

SIGHT_REGISTER_SERIALIZER(data::color, write, read);

} // namespace sight::io::session::detail::color
