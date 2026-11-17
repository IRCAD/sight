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

#include "core/com/signal_base.hpp"

#include <core/com/slot_connection.hpp>
#include <core/mt/types.hpp>

#include <core/com/exception/already_connected.hpp>
#include <core/com/exception/bad_slot.hpp>
#include <core/com/slot.hpp>
#include <core/com/util/remove_last_arg.hpp>

#include <list>
#include <map>
#include <type_traits>
#include <utility>

namespace sight::core::com
{

template<typename F>
struct signal;

/**
 * @brief Signal implementation.
 * Template parameter T must always be void.
 */
template<typename R, typename ... A>
struct SIGHT_CORE_CLASS_API signal<R(A ...)>: signal_base
{
    /**
     * @name Typedefs
     * @{ */
    using signature_type = R(A ...);

    using self_t = signal<signature_type>;

    using sptr = std::shared_ptr<self_t>;
    using wptr = std::weak_ptr<self_t>;

    using slot_run_type = slot_run<signature_type>;
    using slot_sptr     = std::shared_ptr<slot_run_type>;

    using pair_type        = std::pair<bool, std::weak_ptr<slot_run_type> >;
    using slot_container_t = std::list<pair_type*>;

    using connection_map_type = std::map<std::weak_ptr<slot_base>, std::weak_ptr<slot_connection_base>,
                                         std::owner_less<std::weak_ptr<slot_base> > >;
    /**  @} */

    /// Destructor : disconnects all remaining connections.
    ~signal() override
    {
        this->disconnect_all();
    }

    /**
     * @brief Connects the given slot.
     *
     * @return an object managing the connection.
     * @throws BadSlot If given slot doesn't match signal type.
     * @throws AlreadyConnected If given slot is already connected.
     */
    connection connect(sight::sptr<slot_base> _slot) override;

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect(sight::sptr<slot_base> _slot) override;

    /// Disconnects all slots.
    void disconnect_all();

    /// Requests execution of slots with given arguments.
    void emit(A ... _a) const;

    /// Requests asynchronous execution of slots with given arguments.
    void async_emit(A ... _a) const;

    /// Returns number of connected slots.
    std::size_t num_connections() const override
    {
        core::mt::read_lock lock(m_connections_mutex);
        return m_slots.size();
    }

    /**
     * @brief Returns the connection handler matching given slot.
     * @throws BadSlot if given slot is not connected and `throws` is true.
     */
    connection get_connection(sight::sptr<slot_base> _slot, bool _throws = false) override;

    protected:

        template<typename F>
        friend struct slot_connection;
        /**
         * @brief Connects the given slot.
         * Tries to connect a slot of type R (A1, A2, ..., Am, An) :
         * returns a connection on success, otherwise try to connect
         * the same slot with the type R (A1, A2, ..., Am).
         *
         * @throws BadSlot If given slot doesn't match signal type.
         * @throws AlreadyConnected If given slot is already connected.
         */
        template<typename FROM_F>
        connection connect(sight::sptr<slot_base> _slot);

    private:

        /// Connected slots.
        slot_container_t m_slots {};

        /// Container of current connections.
        connection_map_type m_connections {};

        mutable core::mt::read_write_mutex m_connections_mutex;

        static_assert(std::is_same_v<void, R>);
};

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

    // Make a copy of the connections to avoid iterator invalidation when disconnecting slots
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

    for(const auto& slot : m_slots)
    {
        if(slot->first)
        {
            slot->second.lock()->run(_a ...);
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
    std::vector<sight::sptr<slot_run_type> > keep_slots_alive;
    {
        core::mt::read_lock lock(m_connections_mutex);

        for(const auto& slot : m_slots)
        {
            if(slot->first)
            {
                auto slot_ptr = slot->second.lock();
                keep_slots_alive.push_back(slot_ptr);
                slot_ptr->async_run(_a ...);
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

        if(m_connections.contains(_slot))
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
            m_connections.insert(connection_map_type::value_type(_slot, slot_connection));
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
            m_connections.insert(connection_map_type::value_type(_slot, slot_connection));
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

} // namespace sight::core::com
