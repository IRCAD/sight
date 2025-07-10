/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <data/boolean.hpp>
#include <data/color.hpp>
#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>
#include <data/real.hpp>

#include <io/session/helper.hpp>

namespace sight::io::session::detail::string_serializable
{

/// Generic serialization function
/// @param[in,out] _archive output archive where to write binary files to
/// @param[in,out] _tree output tree where to write trivial class members to
/// @param[in] _object the object to serialize
/// @param[out] _children the list of child objects belonging to object
/// @param[in] _password (optional) the password to use if encryption is enabled
template<typename T>
inline static void serialize(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto& casted = helper::safe_cast<T>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<T>(_tree, 1);

    _tree.put(helper::VALUE, casted->to_string());
}

/// Generic deserialization function
/// @param[in] _archive the archive where to read binary files
/// @param[in] _tree the tree where to read trivial object members
/// @param[in] _children a map of data object that belongs to the object
/// @param[in,out] _object the object to deserialize using the archive, the tree and the children map
/// @param[in] _password (optional) the password used for encryption
template<typename T>
inline static typename T::sptr deserialize(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    const auto& casted = helper::cast_or_create<T>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<T>(_tree, 0, 1);

    // Assign the value
    casted->from_string(_tree.get<std::string>(helper::VALUE));

    return casted;
}

} // sight::io::session::detail::string_serializable

SIGHT_REGISTER_SERIALIZER(
    sight::data::boolean,
    &sight::io::session::detail::string_serializable::serialize<sight::data::boolean>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::boolean>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::dvec2,
    &sight::io::session::detail::string_serializable::serialize<sight::data::dvec2>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::dvec2>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::dvec3,
    &sight::io::session::detail::string_serializable::serialize<sight::data::dvec3>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::dvec3>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::dvec4,
    &sight::io::session::detail::string_serializable::serialize<sight::data::dvec4>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::dvec4>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::integer,
    &sight::io::session::detail::string_serializable::serialize<sight::data::integer>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::integer>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::ivec2,
    &sight::io::session::detail::string_serializable::serialize<sight::data::ivec2>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::ivec2>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::ivec3,
    &sight::io::session::detail::string_serializable::serialize<sight::data::ivec3>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::ivec3>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::ivec4,
    &sight::io::session::detail::string_serializable::serialize<sight::data::ivec4>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::ivec4>
);

SIGHT_REGISTER_SERIALIZER(
    sight::data::real,
    &sight::io::session::detail::string_serializable::serialize<sight::data::real>,
    &sight::io::session::detail::string_serializable::deserialize<sight::data::real>
);
