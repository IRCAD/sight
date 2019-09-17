/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "fwActivities/IActivitySequencer.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/Signal.hxx>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

namespace fwActivities
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

int IActivitySequencer::parseActivities(const ::fwMedData::SeriesDB::sptr& seriesDB)
{
    int lastActivityIndex = -1;

    for (const auto& series: seriesDB->getContainer())
    {
        ::fwMedData::ActivitySeries::sptr activity = ::fwMedData::ActivitySeries::dynamicCast(series);

        if (!activity)
        {
            // Remove the wrong data
            SLM_ERROR("The series DB must only contain 'ActivitySeries'. The series of type '" +
                      series->getClassname() + "' will be removed")

            ::fwMedDataTools::helper::SeriesDB helper(seriesDB);
            helper.remove(series);
            helper.notify();
        }
        else if (std::find(m_activityIds.begin(), m_activityIds.end(),
                           activity->getActivityConfigId()) == m_activityIds.end())
        {
            // Remove the wrong data
            SLM_ERROR("The activity '" +activity->getActivityConfigId() + "' is unknown, it will be removed")

            ::fwMedDataTools::helper::SeriesDB helper(seriesDB);
            helper.remove(activity);
            helper.notify();
        }
        else
        {
            ++lastActivityIndex;
            this->storeActivityData(seriesDB);
        }
    }
    return lastActivityIndex;
}

//------------------------------------------------------------------------------

void IActivitySequencer::storeActivityData(const ::fwMedData::SeriesDB::sptr& seriesDB,
                                           const ::fwData::Composite::csptr& overrides)
{
    // Retrives the current activity data
    const size_t currentIdx = static_cast<size_t>(m_currentActivity);
    SLM_ASSERT("SeriesDB does not contain enough series.", seriesDB->size() > currentIdx);
    ::fwMedData::Series::sptr series           = seriesDB->getContainer()[currentIdx];
    ::fwMedData::ActivitySeries::sptr activity = ::fwMedData::ActivitySeries::dynamicCast(series);
    SLM_ASSERT("seriesDB contains an unknown series : " + series->getClassname(), activity);
    ::fwData::Composite::sptr composite = activity->getData();

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

::fwMedData::ActivitySeries::sptr IActivitySequencer::getActivity(const ::fwMedData::SeriesDB::sptr& seriesDB,
                                                                  size_t index, const ::fwCom::SlotBase::sptr& slot,
                                                                  const ::fwData::Composite::csptr& overrides)
{
    ::fwMedData::ActivitySeries::sptr activity;
    if (seriesDB->size() > index) // The activity already exists, update the data
    {
        ::fwMedData::Series::sptr series = seriesDB->getContainer()[index];
        activity                         = ::fwMedData::ActivitySeries::dynamicCast(series);
        SLM_ASSERT("seriesDB contains an unknown series : " + series->getClassname(), activity);
        ::fwData::Composite::sptr composite = activity->getData();

        // FIXME: update all the data or only the requirement ?
        if(overrides)
        {
            auto overridesContainer = overrides->getContainer();
            for (const auto& elt : composite->getContainer())
            {
                composite->getContainer()[elt.first] = overridesContainer.count(elt.first) == 0 ?
                                                       m_requirements[elt.first] : overridesContainer[elt.first];
            }
        }
        else
        {
            for (const auto& elt : composite->getContainer())
            {
                composite->getContainer()[elt.first] = m_requirements[elt.first];
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

        const std::string activityId                       = m_activityIds[index];
        const ::fwActivities::registry::ActivityInfo& info =
            ::fwActivities::registry::Activities::getDefault()->getInfo(activityId);

        activity = ::fwMedData::ActivitySeries::New();

        activity->setModality("OT");
        activity->setInstanceUID("fwActivities." + ::fwTools::UUID::generateUUID() );

        const ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
        activity->setDate(::fwTools::getDate(now));
        activity->setTime(::fwTools::getTime(now));

        activity->setActivityConfigId(info.id);
        activity->setDescription(info.description);

        ::fwData::Composite::sptr composite = activity->getData();

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
                ::fwData::Object::sptr newObj       = ::fwData::factory::New(req.type);
                composite->getContainer()[req.name] = newObj;
                m_requirements[req.name]            = newObj;
            }
            else if (req.minOccurs == 0)
            {
                // create empty composite for optional data
                ::fwData::Composite::sptr optionalDataComposite = ::fwData::Composite::New();
                composite->getContainer()[req.name]             = optionalDataComposite;
                m_requirements[req.name]                        = optionalDataComposite;
            }
        }

        ::fwMedDataTools::helper::SeriesDB helper(seriesDB);
        helper.add(activity);
        {
            auto sig = seriesDB->signal< ::fwMedData::SeriesDB::AddedSeriesSignalType >(
                ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG);
            if (slot)
            {
                ::fwCom::Connection::Blocker block(sig->getConnection( slot ));
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

} // namespace fwActivities
