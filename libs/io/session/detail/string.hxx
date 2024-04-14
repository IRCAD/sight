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

#include <data/string.hpp>

namespace sight::io::session::detail::string
{

constexpr static auto VALUE {"Value"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto string = helper::safe_cast<data::string>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::string>(_tree, 1);

    helper::write_string(_tree, VALUE, string->get_value());
}

//------------------------------------------------------------------------------

inline static data::string::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto string = helper::cast_or_create<data::string>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::string>(_tree, 0, 1);

    // Assign the value
    string->set_value(helper::read_string(_tree, VALUE));

    return string;
}

SIGHT_REGISTER_SERIALIZER(data::string, write, read);

} // namespace sight::io::session::detail::string
