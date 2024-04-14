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

#include <data/composite.hpp>

namespace sight::io::session::detail::composite
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
    const auto composite = helper::safe_cast<data::composite>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::composite>(_tree, 1);

    // composite is map of child object..
    _children = std::map<std::string, data::object::csptr>(composite->cbegin(), composite->cend());
}

//------------------------------------------------------------------------------

inline static data::composite::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto composite = helper::cast_or_create<data::composite>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::composite>(_tree, 0, 1);

    composite->operator=(_children);

    return composite;
}

SIGHT_REGISTER_SERIALIZER(data::composite, write, read);

} // namespace sight::io::session::detail::composite
