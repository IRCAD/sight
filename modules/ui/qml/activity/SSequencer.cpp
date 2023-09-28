/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/Composite.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::ui::qml::activity
{

const core::com::signals::key_t ACTIVITY_CREATED_SIG = "activityCreated";
const core::com::signals::key_t DATA_REQUIRED_SIG    = "dataRequired";

const core::com::slots::key_t GO_TO_SLOT      = "goTo";
const core::com::slots::key_t CHECK_NEXT_SLOT = "checkNext";
const core::com::slots::key_t NEXT_SLOT       = "next";
const core::com::slots::key_t PREVIOUS_SLOT   = "previous";

//------------------------------------------------------------------------------

SSequencer::SSequencer() :
    m_sigActivityCreated(new_signal<ActivityCreatedSignalType>(ACTIVITY_CREATED_SIG)),
    m_sigDataRequired(new_signal<DataRequiredSignalType>(DATA_REQUIRED_SIG))
{
    new_slot(GO_TO_SLOT, &SSequencer::goTo, this);
    new_slot(CHECK_NEXT_SLOT, &SSequencer::checkNext, this);
    new_slot(NEXT_SLOT, &SSequencer::next, this);
    new_slot(PREVIOUS_SLOT, &SSequencer::previous, this);
}

//------------------------------------------------------------------------------

SSequencer::~SSequencer()
= default;

//------------------------------------------------------------------------------

void SSequencer::configuring()
{
}

//------------------------------------------------------------------------------

void SSequencer::starting()
{
    for(const auto& m_qActivityId : m_qActivityIds)
    {
        m_activityIds.push_back(m_qActivityId.toStdString());
    }
}

//------------------------------------------------------------------------------

void SSequencer::stopping()
{
}

//------------------------------------------------------------------------------

void SSequencer::updating()
{
    {
        auto activity_set = m_activity_set.lock();
        SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

        m_currentActivity = this->parseActivities(*activity_set);
    }

    if(m_currentActivity >= 0)
    {
        for(int i = 0 ; i <= m_currentActivity ; ++i)
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
    if(index < 0 || index >= static_cast<int>(m_activityIds.size()))
    {
        SIGHT_ERROR("no activity to launch at index " << index)
        return;
    }

    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    if(m_currentActivity >= 0)
    {
        storeActivityData(*activity_set, std::size_t(m_currentActivity));
    }

    auto activity = getActivity(*activity_set, std::size_t(index), slot(service::slots::UPDATE));

    bool ok = true;
    std::string errorMsg;

    std::tie(ok, errorMsg) = sight::module::ui::qml::activity::SSequencer::validateActivity(activity);
    if(ok)
    {
        m_sigActivityCreated->async_emit(activity);

        m_currentActivity = index;
        Q_EMIT select(index);
    }
    else
    {
        sight::ui::dialog::message::show("Activity not valid", errorMsg);
        m_sigDataRequired->async_emit(activity);
    }
}

//------------------------------------------------------------------------------

void SSequencer::checkNext()
{
    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Store current activity data before checking the next one,
    // new data can be added in the current activity during the process.
    if(m_currentActivity >= 0)
    {
        this->storeActivityData(*activity_set, std::size_t(m_currentActivity));
    }

    const auto nextIdx = static_cast<std::size_t>(m_currentActivity) + 1;
    if(nextIdx < m_activityIds.size())
    {
        data::Activity::sptr nextActivity = this->getActivity(*activity_set, nextIdx, slot(service::slots::UPDATE));

        bool ok = true;
        std::string errorMsg;

        std::tie(ok, errorMsg) = sight::module::ui::qml::activity::SSequencer::validateActivity(nextActivity);

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

service::connections_t SSequencer::getAutoConnections() const
{
    connections_t connections;
    connections.push(s_ACTIVITY_SET_INOUT, data::ActivitySet::ADDED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(s_ACTIVITY_SET_INOUT, data::ActivitySet::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::activity
