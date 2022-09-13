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

#include <data/Set.hpp>

namespace sight::io::session
{

namespace detail::Set
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto set = Helper::safe_cast<data::Set>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Set>(tree, 1);

    for(std::size_t index = 0, end = set->size() ; index < end ; ++index)
    {
        children[data::Set::classname() + std::to_string(index)] = set->at(index);
    }
}

//------------------------------------------------------------------------------

inline static data::Set::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto set = Helper::cast_or_create<data::Set>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Set>(tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    set->clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Set::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        set->push_back(it->second);
    }

    return set;
}

} // namespace detail::Set

} // namespace sight::io
