/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include <data/helper/SeriesDB.hpp>

namespace sight::activity
{

//-----------------------------------------------------------------------------

IActivitySequencer::IActivitySequencer()
{
}

//-----------------------------------------------------------------------------

IActivitySequencer::~IActivitySequencer()
{
}

//------------------------------------------------------------------------------

int IActivitySequencer::parseActivities(data::SeriesDB& seriesDB)
{
    int lastActivityIndex = -1;

    for(const auto& series : seriesDB.getContainer())
    {
        data::ActivitySeries::sptr activity = data::ActivitySeries::dynamicCast(series);

        if(!activity)
        {
            // Remove the wrong data
            SIGHT_ERROR(
                "The series DB must only contain 'ActivitySeries'. The series of type '"
                + series->getClassname() + "' will be removed"
            )

            data::helper::SeriesDB helper(seriesDB);
            helper.remove(series);
            helper.notify();
        }
        else if(!(static_cast<std::size_t>(lastActivityIndex + 1) < m_activityIds.size()
                  && m_activityIds[static_cast<std::size_t>(lastActivityIndex + 1)] == activity->getActivityConfigId()))
        {
            // Remove the wrong data
            SIGHT_ERROR("The activity '" + activity->getActivityConfigId() + "' is unknown, it will be removed")

            data::helper::SeriesDB helper(seriesDB);
            helper.remove(activity);
            helper.notify();
        }
        else
        {
            const bool ok = this->validateActivity(activity).first;
            if(ok)
            {
                ++lastActivityIndex;
                this->storeActivityData(seriesDB, lastActivityIndex);
            }
            else
            {
                break;
            }
        }
    }

    return lastActivityIndex;
}

//------------------------------------------------------------------------------

void IActivitySequencer::storeActivityData(
    const data::SeriesDB& seriesDB,
    int index,
    const data::Composite::csptr& overrides
)
{
    // Retrives the current activity data
    const std::size_t currentIdx = static_cast<std::size_t>(index);
    SIGHT_ASSERT("SeriesDB does not contain enough series.", seriesDB.size() > currentIdx);
    data::Series::sptr series           = seriesDB.getContainer()[currentIdx];
    data::ActivitySeries::sptr activity = data::ActivitySeries::dynamicCast(series);
    SIGHT_ASSERT("seriesDB contains an unknown series : " + series->getClassname(), activity);
    data::Composite::sptr composite = activity->getData();

    if(overrides)
    {
        // Do not store overriden requirements
        for(const auto& elt : *composite)
        {
            if(overrides->count(elt.first) == 0)
            {
                m_requirements[elt.first] = elt.second;
            }
        }
    }
    else
    {
        for(const auto& elt : *composite)
        {
            m_requirements[elt.first] = elt.second;
        }
    }
}

//------------------------------------------------------------------------------

data::ActivitySeries::sptr IActivitySequencer::getActivity(
    data::SeriesDB& seriesDB,
    std::size_t index,
    const core::com::SlotBase::sptr& slot,
    const data::Composite::csptr& overrides
)
{
    data::ActivitySeries::sptr activity;
    if(seriesDB.size() > index) // The activity already exists, update the data
    {
        data::Series::sptr series = seriesDB.getContainer()[index];
        activity = data::ActivitySeries::dynamicCast(series);
        SIGHT_ASSERT("seriesDB contains an unknown series : " + series->getClassname(), activity);
        data::Composite::sptr composite = activity->getData();

        // FIXME: update all the data or only the requirement ?
        if(overrides)
        {
            const std::string activityId                  = m_activityIds[index];
            const activity::extension::ActivityInfo& info =
                activity::extension::Activity::getDefault()->getInfo(activityId);

            for(const auto& req : info.requirements)
            {
                if(m_requirements.find(req.name) != m_requirements.end()
                   || overrides->find(req.name) != m_requirements.end())
                {
                    (*composite)[req.name] = overrides->count(req.name) == 0
                                             ? m_requirements[req.name] : overrides->get(req.name);
                }
            }
        }
        else
        {
            const std::string activityId                  = m_activityIds[index];
            const activity::extension::ActivityInfo& info =
                activity::extension::Activity::getDefault()->getInfo(activityId);

            for(const auto& req : info.requirements)
            {
                if(m_requirements.find(req.name) != m_requirements.end())
                {
                    (*composite)[req.name] = m_requirements[req.name];
                }
            }
        }
    }
    else // create a new activity series
    {
        // try to create the intermediate activities
        if(index > 0 && (index - 1) >= seriesDB.size())
        {
            this->getActivity(seriesDB, index - 1, slot, overrides);
        }

        const std::string activityId                  = m_activityIds[index];
        const activity::extension::ActivityInfo& info =
            activity::extension::Activity::getDefault()->getInfo(activityId);

        activity = data::ActivitySeries::New();

        activity->setModality("OT");
        activity->setInstanceUID("activity." + core::tools::UUID::generateUUID());

        const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        activity->setDate(core::tools::getDate(now));
        activity->setTime(core::tools::getTime(now));

        activity->setActivityConfigId(info.id);
        activity->setDescription(info.description);

        data::Composite::sptr composite = activity->getData();

        for(const auto& req : info.requirements)
        {
            if(overrides)
            {
                if(overrides->count(req.name) != 0)
                {
                    (*composite)[req.name] = overrides->get(req.name);
                }
            }
            else if(m_requirements.find(req.name) != m_requirements.end())
            {
                (*composite)[req.name] = m_requirements[req.name];
            }
            else if(req.create == true || (req.minOccurs == 0 && req.maxOccurs == 0))
            {
                // create the new data
                auto newObj = data::factory::New(req.type);
                (*composite)[req.name]   = newObj;
                m_requirements[req.name] = newObj;
            }
            else if(req.minOccurs == 0)
            {
                // create empty composite for optional data
                auto optionalDataComposite = data::Composite::New();
                (*composite)[req.name]   = optionalDataComposite;
                m_requirements[req.name] = optionalDataComposite;
            }
        }

        data::helper::SeriesDB helper(seriesDB);
        helper.add(activity);
        {
            auto sig = seriesDB.signal<data::SeriesDB::AddedSeriesSignalType>(
                data::SeriesDB::s_ADDED_SERIES_SIG
            );
            if(slot)
            {
                core::com::Connection::Blocker block(sig->getConnection(slot));
                helper.notify();
            }
            else
            {
                helper.notify();
            }
        }
    }

    return activity;
}

//------------------------------------------------------------------------------

void IActivitySequencer::removeLastActivities(data::SeriesDB& seriesDB, std::size_t index)
{
    if(seriesDB.size() > index)
    {
        data::helper::SeriesDB helper(seriesDB);

        // remove the last activities
        while(seriesDB.size() > index)
        {
            const auto activity = seriesDB.back();
            helper.remove(activity);
        }

        helper.notify();

        // clear the requirements and parse the remaining activities to regereate the requirements with the existing
        // activities
        m_requirements.clear();
        this->parseActivities(seriesDB);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::activity
