/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

#include "activity/IActivitySequencer.hpp"

#include <core/com/Connection.hpp>
#include <core/com/Signal.hxx>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

namespace sight::activity
{

//-----------------------------------------------------------------------------

IActivitySequencer::IActivitySequencer()
= default;

//-----------------------------------------------------------------------------

IActivitySequencer::~IActivitySequencer()
= default;

//------------------------------------------------------------------------------

int IActivitySequencer::parseActivities(data::ActivitySet& activity_set)
{
    const auto scoped_emitter = activity_set.scoped_emit();
    std::size_t index         = 0;

    for(auto it = activity_set.cbegin() ; it != activity_set.cend() ; ++it)
    {
        if(*it == nullptr)
        {
            SIGHT_ERROR("One activity is unknown, it will be removed");
            it = activity_set.erase(it);
        }
        else if(!(index < m_activityIds.size() && m_activityIds[index] == (*it)->getActivityConfigId()))
        {
            // Remove the wrong data
            SIGHT_ERROR("The activity '" + (*it)->getActivityConfigId() + "' is unknown, it will be removed");
            it = activity_set.erase(it);
        }
        else if(!sight::activity::IActivitySequencer::validateActivity(*it).first)
        {
            break;
        }
        else
        {
            this->storeActivityData(activity_set, index++);
        }
    }

    return int(index) - 1;
}

//------------------------------------------------------------------------------

void IActivitySequencer::storeActivityData(
    const data::ActivitySet& activity_set,
    std::size_t index,
    const data::Composite::csptr& overrides
)
{
    // Retrives the current activity data
    SIGHT_ASSERT("ActivitySet does not contain enough activities.", activity_set.size() > index);
    const auto& activity = activity_set[index];
    SIGHT_ASSERT("ActivitySet contains an unknown activity.", activity);

    for(const auto& [key, value] : *activity)
    {
        // Do not store overriden requirements
        if(!overrides || overrides->count(key) == 0)
        {
            m_requirements[key] = value;
        }
    }
}

//------------------------------------------------------------------------------

data::Activity::sptr IActivitySequencer::getActivity(
    data::ActivitySet& activity_set,
    std::size_t index,
    const core::com::SlotBase::sptr& slot,
    const data::Composite::csptr& overrides
)
{
    data::Activity::sptr activity;

    const auto& activityId = m_activityIds[index];
    const auto& info       = activity::extension::Activity::getDefault()->getInfo(activityId);

    if(activity_set.size() > index) // The activity already exists, update the data
    {
        activity = activity_set[index];
        SIGHT_ASSERT("ActivitySet contains an unknown activity.", activity);

        // FIXME: update all the data or only the requirement ?
        for(const auto& req : info.requirements)
        {
            if(overrides)
            {
                if(const auto& it = overrides->find(req.name); it != overrides->cend())
                {
                    activity->insert_or_assign(req.name, it->second);

                    // Look for the next requirement
                    continue;
                }
            }

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
        if(index > 0 && (index - 1) >= activity_set.size())
        {
            getActivity(activity_set, index - 1, slot, overrides);
        }

        // Create the activity
        activity = data::Activity::New();

        activity->setActivityConfigId(info.id);
        activity->setDescription(info.description);

        for(const auto& req : info.requirements)
        {
            if(overrides)
            {
                if(const auto& it = overrides->find(req.name); it != overrides->cend())
                {
                    activity->insert_or_assign(req.name, it->second);

                    // Look for the next requirement
                    continue;
                }
            }

            // Look at the non overriden requirements
            if(const auto& it = m_requirements.find(req.name); it != m_requirements.cend())
            {
                activity->insert_or_assign(req.name, it->second);
            }
            else if(req.create || (req.minOccurs == 0 && req.maxOccurs == 0))
            {
                // Create the new data
                auto object = data::factory::New(req.type);
                activity->insert_or_assign(req.name, object);
                m_requirements.insert_or_assign(req.name, object);
            }
            else if(req.minOccurs == 0)
            {
                // Create an empty composite for optional data
                auto object = data::Composite::New();
                activity->insert_or_assign(req.name, object);
                m_requirements.insert_or_assign(req.name, object);
            }
        }

        auto scoped_emitter = activity_set.scoped_emit();
        activity_set.push_back(activity);

        if(slot)
        {
            auto sig = activity_set.signal<data::ActivitySet::added_signal_t>(data::ActivitySet::s_ADDED_OBJECTS_SIG);
            core::com::Connection::Blocker block(sig->getConnection(slot));

            // Force signal emission while blocker exists
            scoped_emitter.reset();
        }
    }

    return activity;
}

//------------------------------------------------------------------------------

void IActivitySequencer::removeLastActivities(data::ActivitySet& activity_set, std::size_t index)
{
    if(activity_set.size() > index)
    {
        const auto scoped_emitter = activity_set.scoped_emit();

        // Remove the activities behind the index
        activity_set.erase(activity_set.cbegin() + int(index), activity_set.cend());

        // clear the requirements and parse the remaining activities to regereate the requirements
        m_requirements.clear();
        this->parseActivities(activity_set);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::activity
