/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

int IActivitySequencer::parseActivities(const data::SeriesDB::sptr& seriesDB)
{
    int lastActivityIndex = -1;

    for (const auto& series: seriesDB->getContainer())
    {
        data::ActivitySeries::sptr activity = data::ActivitySeries::dynamicCast(series);

        if (!activity)
        {
            // Remove the wrong data
            SLM_ERROR("The series DB must only contain 'ActivitySeries'. The series of type '" +
                      series->getClassname() + "' will be removed")

            data::helper::SeriesDB helper(seriesDB);
            helper.remove(series);
            helper.notify();
        }
        else if(!(lastActivityIndex+1 < m_activityIds.size() &&
                  m_activityIds[lastActivityIndex+1] == activity->getActivityConfigId()))
        {
            // Remove the wrong data
            SLM_ERROR("The activity '" +activity->getActivityConfigId() + "' is unknown, it will be removed")

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

void IActivitySequencer::storeActivityData(const data::SeriesDB::sptr& seriesDB, int index,
                                           const data::Composite::csptr& overrides)
{
    // Retrives the current activity data
    const size_t currentIdx = static_cast<size_t>(index);
    SLM_ASSERT("SeriesDB does not contain enough series.", seriesDB->size() > currentIdx);
    data::Series::sptr series           = seriesDB->getContainer()[currentIdx];
    data::ActivitySeries::sptr activity = data::ActivitySeries::dynamicCast(series);
    SLM_ASSERT("seriesDB contains an unknown series : " + series->getClassname(), activity);
    data::Composite::sptr composite = activity->getData();

    if(overrides)
    {
        // Do not store overriden requirements
        auto overridesContainer = overrides->getContainer();
        for (const auto& elt : composite->getContainer())
        {
            if(overridesContainer.count(elt.first) == 0)
            {
                m_requirements[elt.first] = elt.second;
            }
        }
    }
    else
    {
        for (const auto& elt : composite->getContainer())
        {
            m_requirements[elt.first] = elt.second;
        }
    }
}

//------------------------------------------------------------------------------

data::ActivitySeries::sptr IActivitySequencer::getActivity(const data::SeriesDB::sptr& seriesDB,
                                                           size_t index, const core::com::SlotBase::sptr& slot,
                                                           const data::Composite::csptr& overrides)
{
    data::ActivitySeries::sptr activity;
    if (seriesDB->size() > index) // The activity already exists, update the data
    {
        data::Series::sptr series = seriesDB->getContainer()[index];
        activity = data::ActivitySeries::dynamicCast(series);
        SLM_ASSERT("seriesDB contains an unknown series : " + series->getClassname(), activity);
        data::Composite::sptr composite = activity->getData();

        // FIXME: update all the data or only the requirement ?
        if(overrides)
        {
            const std::string activityId                 = m_activityIds[index];
            const activity::extension::ActivityInfo& info =
                activity::extension::Activity::getDefault()->getInfo(activityId);

            auto overridesContainer = overrides->getContainer();

            for(const auto& req : info.requirements)
            {
                if(m_requirements.find(req.name) != m_requirements.end() ||
                   overridesContainer.find(req.name) != m_requirements.end())
                {
                    composite->getContainer()[req.name] = overridesContainer.count(req.name) == 0 ?
                                                          m_requirements[req.name] : overridesContainer[req.name];
                }
            }
        }
        else
        {
            const std::string activityId                 = m_activityIds[index];
            const activity::extension::ActivityInfo& info =
                activity::extension::Activity::getDefault()->getInfo(activityId);

            for(const auto& req : info.requirements)
            {
                if(m_requirements.find(req.name) != m_requirements.end())
                {
                    composite->getContainer()[req.name] = m_requirements[req.name];
                }
            }
        }
    }
    else // create a new activity series
    {
        // try to create the intermediate activities
        if (index > 0 && (index - 1) >= seriesDB->size())
        {
            this->getActivity(seriesDB, index-1, slot, overrides);
        }

        const std::string activityId                 = m_activityIds[index];
        const activity::extension::ActivityInfo& info =
            activity::extension::Activity::getDefault()->getInfo(activityId);

        activity = data::ActivitySeries::New();

        activity->setModality("OT");
        activity->setInstanceUID("activity." + core::tools::UUID::generateUUID() );

        const ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
        activity->setDate(core::tools::getDate(now));
        activity->setTime(core::tools::getTime(now));

        activity->setActivityConfigId(info.id);
        activity->setDescription(info.description);

        data::Composite::sptr composite = activity->getData();

        for (const auto& req : info.requirements)
        {
            if (overrides)
            {
                auto overridesContainer = overrides->getContainer();
                if(overridesContainer.count(req.name) != 0)
                {
                    composite->getContainer()[req.name] = overridesContainer[req.name];
                }
            }
            else if (m_requirements.find(req.name) != m_requirements.end())
            {
                composite->getContainer()[req.name] = m_requirements[req.name];
            }
            else if (req.create == true || (req.minOccurs == 0 && req.maxOccurs == 0))
            {
                // create the new data
                data::Object::sptr newObj = data::factory::New(req.type);
                composite->getContainer()[req.name] = newObj;
                m_requirements[req.name]            = newObj;
            }
            else if (req.minOccurs == 0)
            {
                // create empty composite for optional data
                data::Composite::sptr optionalDataComposite = data::Composite::New();
                composite->getContainer()[req.name] = optionalDataComposite;
                m_requirements[req.name]            = optionalDataComposite;
            }
        }

        data::helper::SeriesDB helper(seriesDB);
        helper.add(activity);
        {
            auto sig = seriesDB->signal< data::SeriesDB::AddedSeriesSignalType >(
                data::SeriesDB::s_ADDED_SERIES_SIG);
            if (slot)
            {
                core::com::Connection::Blocker block(sig->getConnection( slot ));
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

} // namespace sight::activity
