/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
#include "io/session/Helper.hpp"

#include <data/String.hpp>

namespace sight::io::session::detail::String
{

constexpr static auto s_Value {"Value"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto string = Helper::safeCast<data::String>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::String>(tree, 1);

    Helper::writeString(tree, s_Value, string->getValue());
}

//------------------------------------------------------------------------------

inline static data::String::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto string = Helper::safeCast<data::String>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::String>(tree, 0, 1);

    // Assign the value
    string->setValue(Helper::readString(tree, s_Value));

    return string;
}

} // namespace sight::io::session::detail::String
