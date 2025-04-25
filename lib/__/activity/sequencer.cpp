/************************************************************************
 *
 * Copyright (C) 2019-2025 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "activity/sequencer.hpp"

#include "activity/builder/base.hpp"
#include "activity/builder/data.hpp"

#include <data/mt/locked_ptr.hpp>

namespace sight::activity
{

static std::int64_t activity_id_counter = 0;

//------------------------------------------------------------------------------

int sequencer::parse_activities(data::activity_set& _activity_set)
{
    const auto scoped_emitter = _activity_set.scoped_emit();
    std::size_t index         = 0;

    for(auto it = _activity_set.cbegin() ; it != _activity_set.cend() ; ++it)
    {
        if(*it == nullptr)
        {
            SIGHT_ERROR("One activity is unknown, it will be removed");
            it = _activity_set.erase(it);
        }
        else if(!(index < m_activity_ids.size() && m_activity_ids[index] == (*it)->get_activity_config_id()))
        {
            // Remove the wrong data
            SIGHT_ERROR("The activity '" + (*it)->get_activity_config_id() + "' is unknown, it will be removed");
            it = _activity_set.erase(it);
        }
        else if(!sight::activity::sequencer::validate_activity(*it).first)
        {
            break;
        }
        else
        {
            this->store_activity_data(_activity_set, index++);
        }
    }

    return int(index) - 1;
}

//------------------------------------------------------------------------------

void sequencer::store_activity_data(
    const data::activity_set& _activity_set,
    std::size_t _index,
    const data::map::csptr& _overrides
)
{
    // Retrives the current activity data
    SIGHT_ASSERT("ActivitySet does not contain enough activities.", _activity_set.size() > _index);
    const auto& activity = _activity_set[_index];
    SIGHT_ASSERT("ActivitySet contains an unknown activity.", activity);

    for(const auto& [key, value] : *activity)
    {
        // Do not store overriden requirements
        if(!_overrides || _overrides->count(key) == 0)
        {
            m_requirements[key] = value;
        }
    }
}

//------------------------------------------------------------------------------

data::activity::sptr sequencer::get_activity(
    data::activity_set& _activity_set,
    std::size_t _index,
    const core::com::slot_base::sptr& _slot
)
{
    data::activity::sptr activity;

    const auto& activity_id = m_activity_ids[_index];
    const auto& info        = activity::extension::activity::get_default()->get_info(activity_id);

    if(_activity_set.size() > _index) // The activity already exists, update the data
    {
        activity = _activity_set[_index];
        SIGHT_ASSERT("ActivitySet contains an unknown activity.", activity);

        // FIXME: update all the data or only the requirement ?
        for(const auto& req : info.requirements)
        {
            // Look at the non overriden requirements
            if(const auto& it = m_requirements.find(req.name); it != m_requirements.cend())
            {
                activity->insert_or_assign(req.name, it->second);
            }
        }
    }
    else // create a new activity
    {
        // try to create the intermediate activities
        if(_index > 0 && (_index - 1) >= _activity_set.size())
        {
            get_activity(_activity_set, _index - 1, _slot);
        }

        // Create the activity
        activity = std::make_shared<data::activity>();

        activity->set_activity_config_id(info.id);
        activity->set_description(info.description);

        for(const auto& req : info.requirements)
        {
            // Look at the non overriden requirements
            if(const auto& it = m_requirements.find(req.name); it != m_requirements.cend())
            {
                activity->insert_or_assign(req.name, it->second);
            }
            else if(req.create || (req.min_occurs == 0 && req.max_occurs == 0))
            {
                // Create the new data
                auto object = sight::activity::detail::data::create(req.type, req.object_config);

                // Generate a unique id for each object according to the requirement name, otherwise the ids are based
                // on data type. This would make debugging harder and it is required for some other services to work.
                const auto id = core::id::join("activity", std::to_string(activity_id_counter), req.name);
                object->set_id(id);

                activity->insert_or_assign(req.name, object);
                m_requirements.insert_or_assign(req.name, object);
            }
            else if(req.min_occurs == 0)
            {
                // Create an empty map for optional data
                auto object = std::make_shared<data::map>();
                activity->insert_or_assign(req.name, object);
                m_requirements.insert_or_assign(req.name, object);
            }
        }

        auto scoped_emitter = _activity_set.scoped_emit();
        _activity_set.push_back(activity);

        if(_slot)
        {
            auto sig = _activity_set.signal<data::activity_set::added_signal_t>(data::activity_set::ADDED_OBJECTS_SIG);
            core::com::connection::blocker block(sig->get_connection(_slot));

            // Force signal emission while blocker exists
            scoped_emitter.reset();
        }
    }

    return activity;
}

//------------------------------------------------------------------------------

void sequencer::remove_last_activities(data::activity_set& _activity_set, std::size_t _index)
{
    if(_activity_set.size() > _index)
    {
        const auto scoped_emitter = _activity_set.scoped_emit();

        // Remove the activities behind the index
        _activity_set.erase(_activity_set.cbegin() + int(_index), _activity_set.cend());

        // clear the requirements and parse the remaining activities to recreate the requirements
        m_requirements.clear();
        // increase the id counter, because we will recreate new requirements if we start again activities
        // and the current requirements can still be referenced by some services until the restart is processed;
        ++activity_id_counter;
        this->parse_activities(_activity_set);
    }
}

//------------------------------------------------------------------------------

void sequencer::reset_requirements()
{
    // For all registered activities
    for(const auto& id : m_activity_ids)
    {
        // Get the information about the activity
        const auto& info =
            sight::activity::extension::activity::get_default()->get_info(id);

        // For all registered requirements of the current activity
        for(const auto& requirement : info.requirements)
        {
            // Only reset the requirements that are created here
            if((requirement.create || requirement.min_occurs == 0)
               && m_requirements.contains(requirement.name))
            {
                // Get the data object and lock it
                auto object = m_requirements[requirement.name];
                data::mt::locked_ptr locked_object(object);

                if(!requirement.create && requirement.max_occurs != 0)
                {
                    object->shallow_copy(std::make_shared<data::map>());
                }
                else
                {
                    object->shallow_copy(detail::data::create(requirement.type, requirement.object_config));
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::activity
