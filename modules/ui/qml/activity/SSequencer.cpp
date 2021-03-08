/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "SSequencer.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::ui::qml::activity
{

const service::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

const core::com::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG = "activityCreated";
const core::com::Signals::SignalKeyType s_DATA_REQUIRED_SIG    = "dataRequired";

const core::com::Slots::SlotKeyType s_GO_TO_SLOT      = "goTo";
const core::com::Slots::SlotKeyType s_CHECK_NEXT_SLOT = "checkNext";
const core::com::Slots::SlotKeyType s_NEXT_SLOT       = "next";
const core::com::Slots::SlotKeyType s_PREVIOUS_SLOT   = "previous";

//------------------------------------------------------------------------------

SSequencer::SSequencer()
{
    m_sigActivityCreated = newSignal< ActivityCreatedSignalType >(s_ACTIVITY_CREATED_SIG);
    m_sigDataRequired    = newSignal< DataRequiredSignalType >(s_DATA_REQUIRED_SIG);
    newSlot(s_GO_TO_SLOT, &SSequencer::goTo, this);
    newSlot(s_CHECK_NEXT_SLOT, &SSequencer::checkNext, this);
    newSlot(s_NEXT_SLOT, &SSequencer::next, this);
    newSlot(s_PREVIOUS_SLOT, &SSequencer::previous, this);
}

//------------------------------------------------------------------------------

SSequencer::~SSequencer()
{
}

//------------------------------------------------------------------------------

void SSequencer::configuring()
{

}

//------------------------------------------------------------------------------

void SSequencer::starting()
{
    for (int i = 0; i < m_qActivityIds.size(); ++i)
    {
        m_activityIds.push_back(m_qActivityIds.at(i).toStdString());
    }
}

//------------------------------------------------------------------------------

void SSequencer::stopping()
{

}

//------------------------------------------------------------------------------

void SSequencer::updating()
{
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >(s_SERIESDB_INOUT);
    SIGHT_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

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

void SSequencer::goTo(int index)
{
    if (index < 0 || index >= static_cast<int>(m_activityIds.size()))
    {
        SIGHT_ERROR("no activity to launch at index " << index)
        return;
    }
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >(s_SERIESDB_INOUT);
    SIGHT_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    if (m_currentActivity >= 0)
    {
        this->storeActivityData(seriesDB, m_currentActivity);
    }

    const size_t newIdx = static_cast<size_t>(index);

    data::ActivitySeries::sptr activity = this->getActivity(seriesDB, newIdx, m_slotUpdate);

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
        sight::ui::base::dialog::MessageDialog::show("Activity not valid", errorMsg);
        m_sigDataRequired->asyncEmit(activity);
    }
}

//------------------------------------------------------------------------------

void SSequencer::checkNext()
{
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >(s_SERIESDB_INOUT);
    SIGHT_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    // Store current activity data before checking the next one,
    // new data can be added in the current activity during the process.
    if (m_currentActivity >= 0)
    {
        this->storeActivityData(seriesDB, m_currentActivity);
    }

    const size_t nextIdx = static_cast<size_t>(m_currentActivity + 1);
    if (nextIdx < m_activityIds.size())
    {
        data::ActivitySeries::sptr nextActivity = this->getActivity(seriesDB, nextIdx, m_slotUpdate);

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

void SSequencer::next()
{
    this->goTo(m_currentActivity + 1);
}

//------------------------------------------------------------------------------

void SSequencer::previous()
{
    this->goTo(m_currentActivity - 1);
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSequencer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SERIESDB_INOUT, data::SeriesDB::s_ADDED_SERIES_SIG, s_UPDATE_SLOT );
    connections.push( s_SERIESDB_INOUT, data::SeriesDB::s_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

}// namespace sight::module::ui::qml::activity
