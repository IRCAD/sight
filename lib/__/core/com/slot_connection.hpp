/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "core/com/slot_base.hpp"
#include "core/com/slot_connection_base.hpp"

namespace sight::core::com
{

template<typename F>
struct signal;

template<typename F>
struct slot_connection;

/**
 * @brief Slot connection implementation.
 * This class is for internal use purpose.
 */
template<typename ... A>
struct SIGHT_CORE_CLASS_API slot_connection<void(A ...)>: slot_connection_base
{
    /**
     * @name Typedefs
     * @{ */
    using signature_type = void (A ...);

    using self_t = slot_connection<signature_type>;

    using sptr = std::shared_ptr<self_t>;

    using signal_type      = signal<signature_type>;
    using signal_sptr_type = std::shared_ptr<signal_type>;
    using signal_wptr_type = std::weak_ptr<signal_type>;

    using slot_wrapper_type      = slot_run<signature_type>;
    using slot_wrapper_sptr_type = std::shared_ptr<slot_wrapper_type>;

    using slot_run_type      = slot_run<signature_type>;
    using slot_run_sptr_type = std::shared_ptr<slot_run_type>;

    using pair_type = std::pair<bool, std::weak_ptr<slot_run_type> >;
    /**  @} */

    /// Disconnect the related slot.
    void disconnect() override;

    ~slot_connection() override;

    /// Build a new connection with the given signal and slot.
    slot_connection(const signal_sptr_type& _signal, const slot_run_sptr_type& _slot);

    /// Build a new connection with the given signal, slot and wrapper.
    slot_connection(
        const signal_sptr_type& _signal,
        const SPTR(slot_base)& _slot,
        const slot_wrapper_sptr_type& _slot_wrapper
    );

    /**
     * @brief Returns a blocker on the connection.
     * The connection is blocked until the returned shared pointer dies.
     */
    slot_connection_base::blocker_sptr_type get_blocker() override;

    protected:

        template<typename F>
        friend struct signal;

        /// Connect the related Signal and Slot together.
        void connect_no_lock() override;

        /// *NOT THREAD SAFE* Disconnect the related signal.
        void disconnect_signal_no_lock(const signal_sptr_type& _sig);

        /// *NOT THREAD SAFE* Disconnect the related slot.
        void disconnect_slot_no_lock(const SPTR(slot_base)& _slot);

        /// *NOT THREAD SAFE* Disconnect the related slot and signal.
        void disconnect_weak_lock() override;

        /// Unblock this connection.
        void unblock();

    private:

        /// Related Signal.
        signal_wptr_type m_signal;

        /// Related Slot.
        WPTR(slot_base) m_connected_slot;

        /// Slot wrapper.
        SPTR(slot_base) m_slot_wrapper;

        /**
         * @brief Associates state of this connection to related Slot.
         * If m_pair.first is false, the connection is blocked.
         */
        pair_type m_pair;

        /// Connection blocker.
        slot_connection_base::blocker_wptr_type m_weak_blocker;

        mutable core::mt::read_write_mutex m_mutex;
};

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
                static_cast<void*>(nullptr),
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
