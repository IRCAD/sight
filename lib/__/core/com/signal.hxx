/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#if !defined(FWCOM_SIGNAL_HPP)
#error core/com/signal.hpp not included
#endif

#include "core/com/exception/already_connected.hpp"
#include "core/com/exception/bad_slot.hpp"
#include "core/com/slot.hpp"
#include "core/com/slot.hxx"
#include "core/com/slot_connection.hpp"
#include "core/com/slot_connection.hxx"
#include "core/com/util/remove_last_arg.hpp"

#include <boost/function_types/function_arity.hpp>

namespace sight::core::com
{

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
connection signal<R(A ...)>::connect(slot_base::sptr _slot)
{
    return this->connect<signature_type>(_slot);
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
void signal<R(A ...)>::disconnect(slot_base::sptr _slot)
{
    core::mt::read_to_write_lock lock(m_connections_mutex);

    auto iter = m_connections.find(_slot);

    if(iter != m_connections.end())
    {
        slot_connection_base::sptr connection(iter->second.lock());
        SIGHT_ASSERT("Connection has been previously destroyed", connection);
        if(connection)
        {
            core::mt::upgrade_to_write_lock write_lock(lock);
            connection->disconnect_weak_lock();
            // m_connections.erase(slot.get()); // done in connection->disconnect
        }
    }
    else
    {
        SIGHT_THROW_EXCEPTION(core::com::exception::bad_slot("No such slot connected"));
    }
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
void signal<R(A ...)>::disconnect_all()
{
    core::mt::write_lock lock(m_connections_mutex);

    connection_map_type connections = m_connections;

    for(const auto& conn : connections)
    {
        slot_connection_base::sptr connection(conn.second.lock());

        if(connection)
        {
            connection->disconnect_weak_lock();
        }
    }
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
void signal<R(A ...)>::emit(A ... _a) const
{
    core::mt::read_lock lock(m_connections_mutex);
    typename slot_container_t::const_iterator iter;
    auto end = m_slots.end();
    for(iter = m_slots.begin() ; iter != end ; ++iter)
    {
        if((*iter)->first)
        {
            (*iter)->second.lock()->run(_a ...);
        }
    }
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
void signal<R(A ...)>::async_emit(A ... _a) const
{
    // We hold the slots alive in case they are destroyed during the emission
    // If we don't do this, we could end up with a deadlock on m_connectionsMutex
    // because of the automatic disconnection from this signal
    // With this vector, we ensure the potential destruction occurs outside the scope of the mutex
    std::vector<SPTR(slot_run_type)> keep_slots_alive;
    {
        core::mt::read_lock lock(m_connections_mutex);
        typename slot_container_t::const_iterator iter;
        auto end = m_slots.end();
        for(iter = m_slots.begin() ; iter != end ; ++iter)
        {
            if((*iter)->first)
            {
                auto slot = (*iter)->second.lock();
                keep_slots_alive.push_back(slot);
                slot->async_run(_a ...);
            }
        }
    }
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
template<typename FROM_F>
connection signal<R(A ...)>::connect(slot_base::sptr _slot)
{
    {
        core::mt::read_lock lock(m_connections_mutex);

        if(m_connections.find(_slot) != m_connections.end())
        {
            SIGHT_THROW_EXCEPTION(core::com::exception::already_connected("Slot already connected"));
        }
    }

    using connection_type = slot_connection<void (A ...)>;
    connection connection;

    unsigned int sig_arity = boost::function_types::function_arity<signature_type>::value;
    if(sig_arity == _slot->arity())
    {
        slot_sptr slot_to_connect = std::dynamic_pointer_cast<slot_run_type>(_slot);
        if(slot_to_connect)
        {
            core::mt::write_lock lock(m_connections_mutex);
            auto sig             = std::dynamic_pointer_cast<signal<R(A ...)> >(this->shared_from_this());
            auto slot_connection = std::make_shared<connection_type>(sig, slot_to_connect);
            _slot->m_connections.insert(slot_connection);
            m_connections.insert(typename connection_map_type::value_type(_slot, slot_connection));
            slot_connection->connect_no_lock();
            connection = core::com::connection(slot_connection);
        }
        else
        {
            SIGHT_THROW_EXCEPTION(core::com::exception::bad_slot("Incompatible slot"));
        }
    }
    else if(sig_arity > _slot->arity())
    {
        using wrapped_slot_run_type = slot_run<FROM_F>;
        auto wrapped_slot = std::dynamic_pointer_cast<wrapped_slot_run_type>(_slot);

        if(wrapped_slot)
        {
            core::mt::write_lock lock(m_connections_mutex);
            using slot_t = core::com::slot<core::com::slot<void (A ...)> >;
            auto slot_to_connect = std::make_shared<slot_t>(wrapped_slot);
            auto sig             = std::dynamic_pointer_cast<signal<R(A ...)> >(this->shared_from_this());
            auto slot_connection = std::make_shared<connection_type>(sig, _slot, slot_to_connect);
            _slot->m_connections.insert(slot_connection);
            m_connections.insert(typename connection_map_type::value_type(_slot, slot_connection));
            slot_connection->connect_no_lock();
            connection = core::com::connection(slot_connection);
        }
        else
        {
            const auto from_f_arity = boost::function_types::function_arity<FROM_F>::value;
            if constexpr(from_f_arity == 0)
            {
                // We can no longer remove arguments, which means we failed to cast because the type of arguments
                // are different
                SIGHT_THROW_EXCEPTION(core::com::exception::bad_slot("Incompatible slot"));
            }
            else
            {
                connection = this->connect<typename core::com::util::remove_last_arg<FROM_F>::type>(_slot);
            }
        }
    }
    else
    {
        SIGHT_THROW_EXCEPTION(core::com::exception::bad_slot("Incompatible slot"));
    }

    return connection;
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
connection signal<R(A ...)>::get_connection(slot_base::sptr _slot, bool _throws)
{
    core::mt::read_lock lock(m_connections_mutex);
    connection connection;

    auto iter = m_connections.find(_slot);

    if(iter == m_connections.end())
    {
        if(_throws)
        {
            SIGHT_THROW_EXCEPTION(core::com::exception::bad_slot("No such slot connected"));
        }
    }
    else
    {
        slot_connection_base::sptr slot_connection(iter->second);
        connection = core::com::connection(slot_connection);
    }

    return connection;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
