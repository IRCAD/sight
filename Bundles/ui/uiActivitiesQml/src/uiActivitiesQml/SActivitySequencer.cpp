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

#include "uiActivitiesQml/SActivitySequencer.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace uiActivitiesQml
{

fwServicesRegisterMacro( ::fwQml::IQmlEditor, ::uiActivitiesQml::SActivitySequencer )

const ::fwServices::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

const ::fwCom::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG = "activityCreated";
const ::fwCom::Signals::SignalKeyType s_DATA_REQUIRED_SIG    = "dataRequired";

const ::fwCom::Slots::SlotKeyType s_GO_TO_SLOT      = "goTo";
const ::fwCom::Slots::SlotKeyType s_CHECK_NEXT_SLOT = "checkNext";
const ::fwCom::Slots::SlotKeyType s_NEXT_SLOT       = "next";
const ::fwCom::Slots::SlotKeyType s_PREVIOUS_SLOT   = "previous";

//------------------------------------------------------------------------------

SActivitySequencer::SActivitySequencer()
{
    m_sigActivityCreated = newSignal< ActivityCreatedSignalType >(s_ACTIVITY_CREATED_SIG);
    m_sigDataRequired    = newSignal< DataRequiredSignalType >(s_DATA_REQUIRED_SIG);
    newSlot(s_GO_TO_SLOT, &SActivitySequencer::goTo, this);
    newSlot(s_CHECK_NEXT_SLOT, &SActivitySequencer::checkNext, this);
    newSlot(s_NEXT_SLOT, &SActivitySequencer::next, this);
    newSlot(s_PREVIOUS_SLOT, &SActivitySequencer::previous, this);
}

//------------------------------------------------------------------------------

SActivitySequencer::~SActivitySequencer()
{
}

//------------------------------------------------------------------------------

void SActivitySequencer::configuring()
{

}

//------------------------------------------------------------------------------

void SActivitySequencer::starting()
{
    for (int i = 0; i < m_qActivityIds.size(); ++i)
    {
        m_activityIds.push_back(m_qActivityIds.at(i).toStdString());
    }
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
        for (int i = 0; i <= m_currentActivity; ++i)
        {
            Q_EMIT enable(i);
        }
        this->goTo(m_currentActivity);
    }
    else
    {
        // launch the first activity
        Q_EMIT enable(0);
        this->goTo(0);
    }
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

    ::fwMedData::ActivitySeries::sptr activity = this->getActivity(seriesDB, newIdx, m_slotUpdate);

    bool ok = true;
    std::string errorMsg;

    std::tie(ok, errorMsg) = this->validateActivity(activity);
    if(ok)
    {
        m_sigActivityCreated->asyncEmit(activity);

        m_currentActivity = index;
        Q_EMIT select(index);
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity not valid", errorMsg);
        m_sigDataRequired->asyncEmit(activity);
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::checkNext()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    const size_t nextIdx = static_cast<size_t>(m_currentActivity + 1);
    if (nextIdx < m_activityIds.size())
    {
        ::fwMedData::ActivitySeries::sptr nextActivity = this->getActivity(seriesDB, nextIdx, m_slotUpdate);

        bool ok = true;
        std::string errorMsg;

        std::tie(ok, errorMsg) = this->validateActivity(nextActivity);

        if(ok)
        {
            Q_EMIT enable(m_currentActivity + 1);
        }
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

::fwServices::IService::KeyConnectionsMap SActivitySequencer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG, s_UPDATE_SLOT );
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

}// namespace uiActivitiesQml
