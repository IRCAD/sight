/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "io/session/detail/Helper.hpp"

#include <data/Port.hpp>

namespace sight::io::session
{

namespace detail::Port
{

constexpr static auto s_Identifier {"Identifier"};
constexpr static auto s_Type {"Type"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto port = Helper::safeCast<data::Port>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Port>(tree, 1);

    Helper::writeString(tree, s_Identifier, port->getIdentifier(), password);
    Helper::writeString(tree, s_Type, port->getType(), password);
}

//------------------------------------------------------------------------------

inline static data::Port::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto port = Helper::safeCast<data::Port>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Port>(tree, 0, 1);

    port->setIdentifier(Helper::readString(tree, s_Identifier, password));
    port->setType(Helper::readString(tree, s_Type, password));

    return port;
}

} // namespace detail::Port

} // namespace sight::io
