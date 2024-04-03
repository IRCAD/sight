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

constexpr static auto DATA {"Data"};
constexpr static auto ACTIVITY_CONFIG_ID {"activityConfigId"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer&,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& = ""
)
{
    const auto activity = helper::safe_cast<data::activity>(_object);

    helper::write_version<data::activity>(_tree, 2);

    // Now serialize the remaining
    // Serialize children properties
    _children = std::map<std::string, data::object::csptr>(activity->cbegin(), activity->cend());

    // Serialize trivial properties
    helper::write_string(_tree, ACTIVITY_CONFIG_ID, activity->get_activity_config_id());
}

//------------------------------------------------------------------------------

inline static data::activity::sptr read(
    zip::archive_reader&,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto activity = helper::cast_or_create<data::activity>(_object);

    // Check version number. Not mandatory, but could help for future release
    const int version = helper::read_version<data::activity>(_tree, 0, 2);

    // Deserialize the remaining
    // Deserialize children properties
    if(version < 2)
    {
        auto composite = std::dynamic_pointer_cast<data::composite>(_children.at(DATA));
        std::ranges::copy(*composite, std::inserter(*activity, activity->begin()));
    }
    else
    {
        std::ranges::copy(_children, std::inserter(*activity, activity->begin()));
    }

    // Deserialize trivial properties
    activity->set_activity_config_id(helper::read_string(_tree, ACTIVITY_CONFIG_ID));

    return activity;
}

SIGHT_REGISTER_SERIALIZER(data::activity, write, read);

} // namespace detail::activity

} // namespace sight::io
