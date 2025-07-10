/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#if !defined(FWCOM_SLOTCONNECTION_HPP)
#error core/com/slot_connection.hpp not included
#endif

#include "core/com/slot_run.hpp"

namespace sight::core::com
{

//-----------------------------------------------------------------------------

template<typename ... A>
inline slot_connection<void(A ...)>::slot_connection(
    const signal_sptr_type& _signal,
    const slot_run_sptr_type& _slot
) :
    m_signal(_signal),
    m_connected_slot(_slot),
    m_pair(true, _slot)
{
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline slot_connection<void(A ...)>::slot_connection(
    const signal_sptr_type& _signal,
    const slot_base::sptr& _slot,
    const slot_wrapper_sptr_type& _slot_wrapper
) :
    m_signal(_signal),
    m_connected_slot(_slot),
    m_slot_wrapper(_slot_wrapper),
    m_pair(true, _slot_wrapper)
{
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline slot_connection<void(A ...)>::~slot_connection()
{
    this->disconnect();
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline void slot_connection<void(A ...)>::connect_no_lock()
{
    signal_sptr_type sig(m_signal);
    sig->m_slots.push_back(&m_pair);
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline void slot_connection<void(A ...)>::disconnect_signal_no_lock(const signal_sptr_type& _sig)
{
    _sig->m_slots.remove(&m_pair);
    _sig->m_connections.erase(m_connected_slot);
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline void slot_connection<void(A ...)>::disconnect_slot_no_lock(const slot_base::sptr& _slot)
{
    try
    {
        std::shared_ptr<const slot_connection<void(A ...)> > this_sptr =
            std::dynamic_pointer_cast<const slot_connection<void(A ...)> >(this->shared_from_this());
        _slot->m_connections.erase(this_sptr);
    }
    catch(const boost::bad_weak_ptr&)
    {
        // SlotConnection destruction is under way, no need to remove
        // shared_ptr from connections
    }
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline void slot_connection<void(A ...)>::disconnect()
{
    core::mt::write_lock lock(m_mutex);

    signal_sptr_type sig(m_signal.lock());
    slot_base::sptr slot(m_connected_slot.lock());

    if(sig)
    {
        core::mt::write_lock lock_connection(sig->m_connections_mutex);
        this->disconnect_signal_no_lock(sig);
    }

    m_slot_wrapper.reset();

    if(slot)
    {
        core::mt::write_lock lock_connection(slot->m_connections_mutex);
        this->disconnect_slot_no_lock(slot);
    }

    m_signal.reset();
    m_connected_slot.reset();
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline void slot_connection<void(A ...)>::disconnect_weak_lock()
{
    core::mt::write_lock lock(m_mutex);

    signal_sptr_type sig(m_signal.lock());
    slot_base::sptr slot(m_connected_slot.lock());

    if(sig)
    {
        this->disconnect_signal_no_lock(sig);
    }

    m_slot_wrapper.reset();

    if(slot)
    {
        core::mt::write_lock lock_connection(slot->m_connections_mutex);
        this->disconnect_slot_no_lock(slot);
    }

    m_signal.reset();
    m_connected_slot.reset();
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline slot_connection_base::blocker_sptr_type slot_connection<void(A ...)>::get_blocker()
{
    core::mt::read_to_write_lock lock(m_mutex);

    slot_connection_base::blocker_sptr_type blocker(m_weak_blocker.lock());
    if(!blocker)
    {
        core::mt::upgrade_to_write_lock write_lock(lock);

        blocker = m_weak_blocker.lock();

        if(!blocker)
        {
            blocker = slot_connection_base::blocker_sptr_type(
                (void*) nullptr,
                [this](auto&& ...){unblock();});
            m_weak_blocker = blocker;

            // signal has to be locked : signal got a pointer on m_pair
            signal_sptr_type sig(m_signal);
            core::mt::read_lock lock_connection(sig->m_connections_mutex);
            m_pair.first = false;
        }
    }

    return blocker;
}

//-----------------------------------------------------------------------------

template<typename ... A>
inline void slot_connection<void(A ...)>::unblock()
{
    core::mt::write_lock lock(m_mutex);
    // signal has to be locked : signal got a pointer on m_pair
    signal_sptr_type sig(m_signal);
    core::mt::read_lock connection_lock(sig->m_connections_mutex);
    m_pair.first = true;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
