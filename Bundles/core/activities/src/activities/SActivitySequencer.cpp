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

#include <boost/foreach.hpp>

namespace activities
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::activities::SActivitySequencer )

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType s_NEXT_SLOT = "next";
const ::fwCom::Slots::SlotKeyType s_PREVIOUS_SLOT  = "previous";
const ::fwCom::Slots::SlotKeyType s_GO_TO_SLOT     = "goTo";
const ::fwCom::Slots::SlotKeyType s_SEND_INFO_SLOT = "sendInfo";

const ::fwCom::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG = "activityCreated";
const ::fwCom::Signals::SignalKeyType s_DATA_REQUIRED_SIG    = "dataRequired";
const ::fwCom::Signals::SignalKeyType s_ENABLED_PREVIOUS_SIG = "enabledPrevious";
const ::fwCom::Signals::SignalKeyType s_ENABLED_NEXT_SIG     = "enabledNext";

const ::fwServices::IService::KeyType s_SERIESDB_INOUT  = "seriesDB";
const ::fwServices::IService::KeyType s_OVERRIDES_INPUT = "requirementOverrides";

//------------------------------------------------------------------------------

SActivitySequencer::SActivitySequencer() noexcept
{
    newSlot(s_NEXT_SLOT, &SActivitySequencer::next, this);
    newSlot(s_PREVIOUS_SLOT, &SActivitySequencer::previous, this);
    newSlot(s_GO_TO_SLOT, &SActivitySequencer::goTo, this);
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

    m_currentActivity = this->parseActivities(seriesDB);

    if (m_currentActivity >= 0)
    {
        // launch the last series
        const size_t index = static_cast<size_t>(m_currentActivity);
        ::fwData::Composite::csptr overrides           = this->getInput< ::fwData::Composite >(s_OVERRIDES_INPUT);
        ::fwMedData::ActivitySeries::sptr lastActivity = this->getActivity(seriesDB, index, m_slotUpdate, overrides);

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
    this->goTo(m_currentActivity + 1);
}

//------------------------------------------------------------------------------

void SActivitySequencer::previous()
{
    this->goTo(m_currentActivity - 1);
}

//------------------------------------------------------------------------------

void SActivitySequencer::goTo(int index)
{
    if (index < 0 || index >= static_cast<int>(m_activityIds.size()))
    {
        OSLM_ERROR("no activity to launch at index " << index)
        return;
    }
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    if (m_currentActivity >= 0)
    {
        this->storeActivityData(seriesDB, m_currentActivity);
    }

    const size_t newIdx = static_cast<size_t>(index);

    ::fwData::Composite::csptr overrides       = this->getInput< ::fwData::Composite >(s_OVERRIDES_INPUT);
    ::fwMedData::ActivitySeries::sptr activity = this->getActivity(seriesDB, newIdx, m_slotUpdate, overrides);

    if(this->checkValidity(activity, true))
    {
        m_sigActivityCreated->asyncEmit(activity);

        m_currentActivity = index;
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

bool SActivitySequencer::checkValidity(const fwMedData::ActivitySeries::csptr& activity, bool showDialog) const
{
    bool ok = true;
    std::string errorMsg;

    std::tie(ok, errorMsg) = this->validateActivity(activity);

    if (!ok && showDialog)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity not valid", errorMsg);
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
