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
#include "io/session/detail/series.hxx"
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/activity.hpp>
#include <data/composite.hpp>

#include <ranges>
namespace sight::io::session
{

namespace detail::activity
{

constexpr static auto s_Data {"Data"};
constexpr static auto s_activityConfigId {"activityConfigId"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto activity = helper::safe_cast<data::activity>(object);

    helper::write_version<data::activity>(tree, 2);

    // Now serialize the remaining
    // Serialize children properties
    children = std::map<std::string, data::object::csptr>(activity->cbegin(), activity->cend());

    // Serialize trivial properties
    helper::write_string(tree, s_activityConfigId, activity->getActivityConfigId());
}

//------------------------------------------------------------------------------

inline static data::activity::sptr read(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto activity = helper::cast_or_create<data::activity>(object);

    // Check version number. Not mandatory, but could help for future release
    const int version = helper::read_version<data::activity>(tree, 0, 2);

    // Deserialize the remaining
    // Deserialize children properties
    if(version < 2)
    {
        auto composite = std::dynamic_pointer_cast<data::composite>(children.at(s_Data));
        std::ranges::copy(*composite, std::inserter(*activity, activity->begin()));
    }
    else
    {
        std::ranges::copy(children, std::inserter(*activity, activity->begin()));
    }

    // Deserialize trivial properties
    activity->setActivityConfigId(helper::read_string(tree, s_activityConfigId));

    return activity;
}

SIGHT_REGISTER_SERIALIZER(data::activity, write, read);

} // namespace detail::activity

} // namespace sight::io
