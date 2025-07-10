/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/activity_set.hpp>

namespace sight::io::session::detail::activity_set
{

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto activity_set = helper::safe_cast<data::activity_set>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::activity_set>(_tree, 1);

    for(std::size_t index = 0, end = activity_set->size() ; index < end ; ++index)
    {
        _children[data::activity::classname() + std::to_string(index)] = activity_set->at(index);
    }
}

//------------------------------------------------------------------------------

inline static data::activity_set::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto activity_set = helper::cast_or_create<data::activity_set>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::activity_set>(_tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    activity_set->clear();

    for(std::size_t index = 0, end = _children.size() ; index < end ; ++index)
    {
        const auto& it = _children.find(data::activity::classname() + std::to_string(index));

        if(it == _children.cend())
        {
            break;
        }

        const auto& activity = std::dynamic_pointer_cast<data::activity>(it->second);

        if(!activity)
        {
            break;
        }

        activity_set->push_back(activity);
    }

    // Generate a unique id for each object according to the requirement name, otherwise the ids are based on data type
    // This would make debugging harder and it is required for some other services to work
    static std::int64_t counter = 0;
    std::ranges::for_each(
        *activity_set,
        [](const auto& _activity)
        {
            std::ranges::for_each(
                *_activity,
                [](const auto& _x)
            {
                const auto full_id = core::id::join("loaded_activity", std::to_string(counter), _x.first);
                if(_x.second->get_id() != full_id)
                {
                    _x.second->set_id(full_id);
                }
            });
        });

    counter++;

    return activity_set;
}

SIGHT_REGISTER_SERIALIZER(data::activity_set, write, read);

} // namespace sight::io::session::detail::activity_set
