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
#include "io/session/detail/Series.hxx"
#include "io/session/Helper.hpp"
#include "io/session/macros.hpp"

#include <data/Activity.hpp>
#include <data/Composite.hpp>

#include <ranges>
namespace sight::io::session
{

namespace detail::Activity
{

constexpr static auto s_Data {"Data"};
constexpr static auto s_ActivityConfigId {"ActivityConfigId"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto activity = Helper::safe_cast<data::Activity>(object);

    Helper::writeVersion<data::Activity>(tree, 2);

    // Now serialize the remaining
    // Serialize children properties
    children = std::map<std::string, data::Object::csptr>(activity->cbegin(), activity->cend());

    // Serialize trivial properties
    Helper::writeString(tree, s_ActivityConfigId, activity->getActivityConfigId());
}

//------------------------------------------------------------------------------

inline static data::Activity::sptr read(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto activity = Helper::cast_or_create<data::Activity>(object);

    // Check version number. Not mandatory, but could help for future release
    const int version = Helper::readVersion<data::Activity>(tree, 0, 2);

    // Deserialize the remaining
    // Deserialize children properties
    if(version < 2)
    {
        auto composite = std::dynamic_pointer_cast<data::Composite>(children.at(s_Data));
        std::ranges::copy(*composite, std::inserter(*activity, activity->begin()));
    }
    else
    {
        std::ranges::copy(children, std::inserter(*activity, activity->begin()));
    }

    // Deserialize trivial properties
    activity->setActivityConfigId(Helper::readString(tree, s_ActivityConfigId));

    return activity;
}

SIGHT_REGISTER_SERIALIZER(data::Activity, write, read);

} // namespace detail::Activity

} // namespace sight::io
