/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "core/com/helper/sig_slot_connection.hpp"

#include <core/com/exception/already_connected.hpp>
#include <core/com/exception/bad_slot.hpp>
#include <core/com/signal_base.hpp>
#include <core/object.hpp>
#include <core/spy_log.hpp>

#include <ranges>

namespace sight::core::com::helper
{

//-----------------------------------------------------------------------------

sig_slot_connection::sig_slot_connection()
= default;

//-----------------------------------------------------------------------------

sig_slot_connection::~sig_slot_connection()
{
    this->disconnect();
}

//-----------------------------------------------------------------------------

void sig_slot_connection::connect(
    const core::com::has_signals::csptr& _has_signals,
    core::com::signals::signal_key_type _signal_key,
    const core::com::has_slots::csptr& _has_slots,
    core::com::slots::key_t _slot_key
)
{
    try
    {
        core::com::connection connection = _has_signals->signal(_signal_key)->connect(_has_slots->slot(_slot_key));
        m_connections.push_back(connection);
    }
    catch(core::com::exception::bad_slot& e)
    {
        SIGHT_ERROR(
            std::string("Can't connect signal '") + _signal_key + "' with slot '" + _slot_key
            + "' : " << e.what() << "."
        );
    }
    catch(core::com::exception::already_connected& e)
    {
        const auto source = std::dynamic_pointer_cast<const core::object>(_has_signals);
        auto source_id    = source ? source->get_id() : "";

        const auto target = std::dynamic_pointer_cast<const core::object>(_has_slots);
        auto target_id    = target ? target->get_id() : "";

        SIGHT_ERROR(
            std::string("Can't connect signal '") + source_id + "/" + _signal_key + "' with slot '"
            + target_id + "/" + _slot_key + "' : " << e.what() << "."
        );
    }
}

//-----------------------------------------------------------------------------

void sig_slot_connection::connect(
    const core::com::has_signals::csptr& _has_signals,
    const core::com::has_slots::csptr& _has_slots,
    const key_connections_t& _key_connections
)
{
    SIGHT_ASSERT("Signal source is NULL", _has_signals);
    SIGHT_ASSERT("Slot destination is NULL", _has_slots);

    for(const key_connection_t& keys : _key_connections)
    {
        auto signal = _has_signals->signal(keys.first);
        SIGHT_ASSERT("Signal '" + keys.first + "' not found.", signal);
        auto slot = _has_slots->slot(keys.second);
        SIGHT_ASSERT("Slot '" + keys.second + "' not found.", slot);

        try
        {
            core::com::connection connection = signal->connect(slot);
            m_connections.push_back(connection);
        }
        catch(core::com::exception::bad_slot& e)
        {
            SIGHT_ERROR(
                "Can't connect signal '" + keys.first + "' with slot '" + keys.second + "' : "
                << e.what() << "."
            );
        }
        catch(core::com::exception::already_connected& e)
        {
            const auto source = std::dynamic_pointer_cast<const core::object>(_has_signals);
            auto source_id    = source ? source->get_id() : "";

            const auto target = std::dynamic_pointer_cast<const core::object>(_has_slots);
            auto target_id    = target ? target->get_id() : "";

            SIGHT_ERROR(
                std::string(
                    "Can't connect signal '"
                ) << source_id << "/" << keys.first << "' with slot '" << target_id << "/" << keys.second
                << "' : " << e.what() << "."
            );
        }
    }
}

//-----------------------------------------------------------------------------

void sig_slot_connection::add_connection(core::com::connection _connection)
{
    m_connections.push_back(_connection);
}

//-----------------------------------------------------------------------------

void sig_slot_connection::disconnect()
{
    // NOLINTNEXTLINE(bugprone-branch-clone)
    for(auto& connection : std::views::reverse(m_connections))
    {
        connection.disconnect();
    }

    m_connections.clear();
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com::helper
