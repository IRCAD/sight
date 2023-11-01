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

const core::com::signals::key_t ACTIVITY_CREATED_SIG = "activity_created";
const core::com::signals::key_t DATA_REQUIRED_SIG    = "data_required";

const core::com::slots::key_t GO_TO_SLOT      = "go_to";
const core::com::slots::key_t CHECK_NEXT_SLOT = "check_next";
const core::com::slots::key_t NEXT_SLOT       = "next";
const core::com::slots::key_t PREVIOUS_SLOT   = "previous";

//------------------------------------------------------------------------------

sequencer::sequencer() :
    m_sig_activity_created(new_signal<activity_created_signal_t>(ACTIVITY_CREATED_SIG)),
    m_sig_data_required(new_signal<data_required_signal_t>(DATA_REQUIRED_SIG))
{
    new_slot(GO_TO_SLOT, &sequencer::go_to, this);
    new_slot(CHECK_NEXT_SLOT, &sequencer::check_next, this);
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
    for(const auto& activity_id : m_activity_list)
    {
        m_activity_ids.push_back(activity_id.toStdString());
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
        SIGHT_ASSERT("Missing '" << ACTIVITY_SET_INOUT << "' activity_set", activity_set);

        m_current_activity = this->parse_activities(*activity_set);
    }

    if(m_current_activity >= 0)
    {
        for(int i = 0 ; i <= m_current_activity ; ++i)
        {
            Q_EMIT enable(i);
        }

        this->go_to(m_current_activity);
    }
    else
    {
        // launch the first activity
        Q_EMIT enable(0);
        this->go_to(0);
    }
}

//------------------------------------------------------------------------------

void sequencer::go_to(int _index)
{
    if(_index < 0 || _index >= static_cast<int>(m_activity_ids.size()))
    {
        SIGHT_ERROR("no activity to launch at index " << _index)
        return;
    }

    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    if(m_current_activity >= 0)
    {
        store_activity_data(*activity_set, std::size_t(m_current_activity));
    }

    auto activity = get_activity(*activity_set, std::size_t(_index), slot(service::slots::UPDATE));

    bool ok = true;
    std::string error_msg;

    std::tie(ok, error_msg) = sight::module::ui::qml::activity::sequencer::validate_activity(activity);
    if(ok)
    {
        m_sig_activity_created->async_emit(activity);

        m_current_activity = _index;
        Q_EMIT select(_index);
    }
    else
    {
        sight::ui::dialog::message::show("Activity not valid", error_msg);
        m_sig_data_required->async_emit(activity);
    }
}

//------------------------------------------------------------------------------

void sequencer::check_next()
{
    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Store current activity data before checking the next one,
    // new data can be added in the current activity during the process.
    if(m_current_activity >= 0)
    {
        this->store_activity_data(*activity_set, std::size_t(m_current_activity));
    }

    const auto next_idx = static_cast<std::size_t>(m_current_activity) + 1;
    if(next_idx < m_activity_ids.size())
    {
        data::activity::sptr next_activity = this->get_activity(*activity_set, next_idx, slot(service::slots::UPDATE));

        bool ok = true;
        std::string error_msg;

        std::tie(ok, error_msg) = sight::module::ui::qml::activity::sequencer::validate_activity(next_activity);

        if(ok)
        {
            Q_EMIT enable(m_current_activity + 1);
        }
    }
}

//------------------------------------------------------------------------------

void sequencer::next()
{
    this->go_to(m_current_activity + 1);
}

//------------------------------------------------------------------------------

void sequencer::previous()
{
    this->go_to(m_current_activity - 1);
}

//------------------------------------------------------------------------------

service::connections_t sequencer::auto_connections() const
{
    connections_t connections;
    connections.push(ACTIVITY_SET_INOUT, data::activity_set::ADDED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(ACTIVITY_SET_INOUT, data::activity_set::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::activity
