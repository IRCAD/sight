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

#include <data/color.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>

namespace sight::io::session::detail::matrix4
{

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto matrix = helper::safe_cast<data::matrix4>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::matrix4>(tree, 1);

    std::size_t index = 0;
    for(const auto& coefficient : *matrix)
    {
        tree.put(std::to_string(index++), coefficient);
    }
}

//------------------------------------------------------------------------------

inline static data::matrix4::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto matrix = helper::cast_or_create<data::matrix4>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::matrix4>(tree, 0, 1);

    for(std::size_t index = 0 ; index < 16 ; ++index)
    {
        (*matrix)[index] = tree.get<double>(std::to_string(index));
    }

    return matrix;
}

SIGHT_REGISTER_SERIALIZER(data::matrix4, write, read);

} // namespace sight::io::session::detail::matrix4
