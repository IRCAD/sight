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

#include <data/ActivitySet.hpp>

namespace sight::io::session
{

namespace detail::ActivitySet
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
    const auto activity_set = Helper::safeCast<data::ActivitySet>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ActivitySet>(tree, 1);

    for(std::size_t index = 0, end = activity_set->size() ; index < end ; ++index)
    {
        children[data::ActivitySeries::classname() + std::to_string(index)] = activity_set->at(index);
    }
}

//------------------------------------------------------------------------------

inline static data::ActivitySet::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto activity_set = Helper::safeCast<data::ActivitySet>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ActivitySet>(tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    activity_set->clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::ActivitySeries::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        const auto& activity = std::dynamic_pointer_cast<data::ActivitySeries>(it->second);

        if(!activity)
        {
            break;
        }

        activity_set->push_back(activity);
    }

    return activity_set;
}

} // namespace detail::ActivitySet

} // namespace sight::io
