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

#include "sequencer.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/composite.hpp>

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

sequencer::sequencer() :
    m_sigActivityCreated(new_signal<activity_created_signal_t>(ACTIVITY_CREATED_SIG)),
    m_sigDataRequired(new_signal<data_required_signal_t>(DATA_REQUIRED_SIG))
{
    new_slot(GO_TO_SLOT, &sequencer::goTo, this);
    new_slot(CHECK_NEXT_SLOT, &sequencer::checkNext, this);
    new_slot(NEXT_SLOT, &sequencer::next, this);
    new_slot(PREVIOUS_SLOT, &sequencer::previous, this);
}

//------------------------------------------------------------------------------

sequencer::~sequencer()
= default;

//------------------------------------------------------------------------------

void sequencer::configuring()
{
}

//------------------------------------------------------------------------------

void sequencer::starting()
{
    for(const auto& m_q_activity_id : m_qActivityIds)
    {
        m_activityIds.push_back(m_q_activity_id.toStdString());
    }
}

//------------------------------------------------------------------------------

void sequencer::stopping()
{
}

//------------------------------------------------------------------------------

void sequencer::updating()
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

void sequencer::goTo(int _index)
{
    if(_index < 0 || _index >= static_cast<int>(m_activityIds.size()))
    {
        SIGHT_ERROR("no activity to launch at index " << _index)
        return;
    }

    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    if(m_currentActivity >= 0)
    {
        storeActivityData(*activity_set, std::size_t(m_currentActivity));
    }

    auto activity = getActivity(*activity_set, std::size_t(_index), slot(service::slots::UPDATE));

    bool ok = true;
    std::string error_msg;

    std::tie(ok, error_msg) = sight::module::ui::qml::activity::sequencer::validateActivity(activity);
    if(ok)
    {
        m_sigActivityCreated->async_emit(activity);

        m_currentActivity = _index;
        Q_EMIT select(_index);
    }
    else
    {
        sight::ui::dialog::message::show("Activity not valid", error_msg);
        m_sigDataRequired->async_emit(activity);
    }
}

//------------------------------------------------------------------------------

void sequencer::checkNext()
{
    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Store current activity data before checking the next one,
    // new data can be added in the current activity during the process.
    if(m_currentActivity >= 0)
    {
        this->storeActivityData(*activity_set, std::size_t(m_currentActivity));
    }

    const auto next_idx = static_cast<std::size_t>(m_currentActivity) + 1;
    if(next_idx < m_activityIds.size())
    {
        data::activity::sptr next_activity = this->getActivity(*activity_set, next_idx, slot(service::slots::UPDATE));

        bool ok = true;
        std::string error_msg;

        std::tie(ok, error_msg) = sight::module::ui::qml::activity::sequencer::validateActivity(next_activity);

        if(ok)
        {
            Q_EMIT enable(m_currentActivity + 1);
        }
    }
}

//------------------------------------------------------------------------------

void sequencer::next()
{
    this->goTo(m_currentActivity + 1);
}

//------------------------------------------------------------------------------

void sequencer::previous()
{
    this->goTo(m_currentActivity - 1);
}

//------------------------------------------------------------------------------

service::connections_t sequencer::auto_connections() const
{
    connections_t connections;
    connections.push(s_ACTIVITY_SET_INOUT, data::activity_set::ADDED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(s_ACTIVITY_SET_INOUT, data::activity_set::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::activity
