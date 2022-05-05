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

#include <data/Vector.hpp>

namespace sight::io::session
{

namespace detail::Vector
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
    const auto vector = Helper::safeCast<data::Vector>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Vector>(tree, 1);

    std::size_t index = 0;
    for(const auto& child : *vector)
    {
        children[data::Object::classname() + std::to_string(index++)] = child;
    }
}

//------------------------------------------------------------------------------

inline static data::Vector::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto vector = Helper::safeCast<data::Vector>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Vector>(tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    vector->clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Object::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        vector->push_back(it->second);
    }

    return vector;
}

} // namespace detail::Vector

} // namespace sight::io
