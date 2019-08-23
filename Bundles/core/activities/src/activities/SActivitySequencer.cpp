/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "activities/SActivitySequencer.hpp"

#include <fwActivities/IActivityValidator.hpp>
#include <fwActivities/registry/Activities.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

#include <boost/foreach.hpp>

namespace activities
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::activities::SActivitySequencer );

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType s_NEXT_SLOT      = "next";
const ::fwCom::Slots::SlotKeyType s_PREVIOUS_SLOT  = "previous";
const ::fwCom::Slots::SlotKeyType s_SEND_INFO_SLOT = "sendInfo";

const ::fwCom::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG = "activityCreated";
const ::fwCom::Signals::SignalKeyType s_DATA_REQUIRED_SIG    = "dataRequired";
const ::fwCom::Signals::SignalKeyType s_ENABLED_PREVIOUS_SIG = "enabledPrevious";
const ::fwCom::Signals::SignalKeyType s_ENABLED_NEXT_SIG     = "enabledNext";

const ::fwServices::IService::KeyType s_SERIESDB_INOUT  = "seriesDB";
const ::fwServices::IService::KeyType s_OVERRIDES_INOUT = "requirementOverrides";

//------------------------------------------------------------------------------

SActivitySequencer::SActivitySequencer() noexcept :
    m_currentActivity(-1)
{
    newSlot(s_NEXT_SLOT, &SActivitySequencer::next, this);
    newSlot(s_PREVIOUS_SLOT, &SActivitySequencer::previous, this);
    newSlot(s_SEND_INFO_SLOT, &SActivitySequencer::sendInfo, this);

    m_sigActivityCreated = newSignal< ActivityCreatedSignalType >(s_ACTIVITY_CREATED_SIG);
    m_sigDataRequired    = newSignal< DataRequiredSignalType >(s_DATA_REQUIRED_SIG);
    m_sigEnabledPrevious = newSignal< EnabledPreviousSignalType >(s_ENABLED_PREVIOUS_SIG);
    m_sigEnabledNext     = newSignal< EnabledNextSignalType >(s_ENABLED_NEXT_SIG);
}

//------------------------------------------------------------------------------

SActivitySequencer::~SActivitySequencer() noexcept
{
}

//------------------------------------------------------------------------------

void SActivitySequencer::configuring()
{
    const ::fwServices::IService::ConfigType config = this->getConfigTree();
    BOOST_FOREACH( const ::fwServices::IService::ConfigType::value_type& v,  config.equal_range("activity") )
    {
        m_activityIds.push_back(v.second.get<std::string>(""));
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::starting()
{
}

//------------------------------------------------------------------------------

void SActivitySequencer::stopping()
{
}

//------------------------------------------------------------------------------

void SActivitySequencer::updating()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    m_currentActivity = -1;

    for (const auto& series: seriesDB->getContainer())
    {
        ::fwMedData::ActivitySeries::sptr activity = ::fwMedData::ActivitySeries::dynamicCast(series);

        if (!activity)
        {
            // Remove the wrong data
            SLM_ERROR("The series DB must only contain 'ActivitySeries'. The series of type '" +
                      series->getClassname() + "' will be removed");

            ::fwMedDataTools::helper::SeriesDB helper(seriesDB);
            helper.remove(series);
            helper.notify();
        }
        else if (std::find(m_activityIds.begin(), m_activityIds.end(),
                           activity->getActivityConfigId()) == m_activityIds.end())
        {
            // Remove the wrong data
            SLM_ERROR("The activity '" +activity->getActivityConfigId() + "' is unknown, it will be removed");

            ::fwMedDataTools::helper::SeriesDB helper(seriesDB);
            helper.remove(activity);
            helper.notify();
        }
        else
        {
            ++m_currentActivity;
            this->storeActivityData();
        }
    }

    if (m_currentActivity >= 0)
    {
        // launch the last series
        const size_t index = static_cast<size_t>(m_currentActivity);
        ::fwMedData::ActivitySeries::sptr lastActivity = this->getActivity(index);

        if (this->checkValidity(lastActivity, false))
        {
            m_sigActivityCreated->asyncEmit(lastActivity);
        }
        else
        {
            m_sigDataRequired->asyncEmit(lastActivity);
        }
    }
    else
    {
        // launch the first activity
        next();
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::next()
{
    if (m_currentActivity >= 0)
    {
        this->storeActivityData();
    }

    const size_t newIdx = static_cast<size_t>(m_currentActivity + 1);
    if (newIdx >= m_activityIds.size())
    {
        OSLM_ERROR("no activity to launch, the current activity '"+ m_activityIds.back() + "' is the last one.");
        return;
    }

    ::fwMedData::ActivitySeries::sptr activity = this->getActivity(newIdx);

    if(this->checkValidity(activity, true))
    {
        m_sigActivityCreated->asyncEmit(activity);

        ++m_currentActivity;
    }
    else
    {
        m_sigDataRequired->asyncEmit(activity);
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::previous()
{
    if (m_currentActivity <= 0)
    {
        OSLM_ERROR("no activity to launch, the current activity '"+ m_activityIds[0] + "' is the first one.");
        return;
    }
    this->storeActivityData();
    const size_t newIdx = static_cast<size_t>(m_currentActivity - 1);

    ::fwMedData::ActivitySeries::sptr activity = this->getActivity(newIdx);

    if(this->checkValidity(activity, true))
    {
        m_sigActivityCreated->asyncEmit(activity);

        --m_currentActivity;
    }
    else
    {
        m_sigDataRequired->asyncEmit(activity);
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::sendInfo() const
{
    const bool previousEnabled = (m_currentActivity > 0);
    m_sigEnabledPrevious->asyncEmit(previousEnabled);

    const bool nextEnabled = (m_currentActivity < static_cast<int>(m_activityIds.size()) -1);
    m_sigEnabledNext->asyncEmit(nextEnabled);
}

//------------------------------------------------------------------------------

void SActivitySequencer::storeActivityData()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    // Retrives the current activity data
    const size_t currentIdx = static_cast<size_t>(m_currentActivity);
    SLM_ASSERT("SeriesDB does not contain enough series.", seriesDB->size() > currentIdx);
    ::fwMedData::Series::sptr series           = seriesDB->getContainer()[currentIdx];
    ::fwMedData::ActivitySeries::sptr activity = ::fwMedData::ActivitySeries::dynamicCast(series);
    SLM_ASSERT("seriesDB contains an unknown series : " + series->getClassname(), activity);
    ::fwData::Composite::sptr composite  = activity->getData();
    ::fwData::Composite::csptr overrides = this->getInput< ::fwData::Composite>(s_OVERRIDES_INOUT);

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

::fwMedData::ActivitySeries::sptr SActivitySequencer::getActivity(size_t index)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    ::fwMedData::ActivitySeries::sptr activity;
    ::fwData::Composite::csptr overrides = this->getInput< ::fwData::Composite>(s_OVERRIDES_INOUT);
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
            ::fwCom::Connection::Blocker block(sig->getConnection( this->slot(s_UPDATE_SLOT)));
            helper.notify();
        }
    }
    return activity;
}

//------------------------------------------------------------------------------

bool SActivitySequencer::checkValidity(const fwMedData::ActivitySeries::csptr& activity, bool showDialog) const
{
    bool ok = true;
    std::string errorMsg;

    SLM_ASSERT("activity is not defined", activity);

    const ::fwActivities::registry::ActivityInfo& info =
        ::fwActivities::registry::Activities::getDefault()->getInfo(activity->getActivityConfigId());

    // load activity bundle to register validator factory
    std::shared_ptr< ::fwRuntime::Bundle > bundle = ::fwRuntime::findBundle(info.bundleId, info.bundleVersion);
    if (!bundle->isStarted())
    {
        bundle->start();
    }

    for (std::string validatorImpl : info.validatorsImpl)
    {
        ::fwActivities::IValidator::sptr validator = ::fwActivities::validator::factory::New(validatorImpl);

        ::fwActivities::IActivityValidator::sptr activityValidator =
            ::fwActivities::IActivityValidator::dynamicCast(validator);

        SLM_ERROR_IF("Validator '" + validatorImpl + "' instantiation failed", !activityValidator);

        if (activityValidator)
        {

            ::fwActivities::IValidator::ValidationType validation = activityValidator->validate(activity);
            if(!validation.first)
            {
                ok        = false;
                errorMsg += "\n" + validation.second;
            }
        }
    }

    if (!ok && showDialog)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity not valid", "The activity '" + info.title + "' can "
                                                          "not be launched: \n" + errorMsg);
    }
    return ok;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SActivitySequencer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG, s_UPDATE_SLOT );
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace activities
