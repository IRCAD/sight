/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#define FWCOM_SIGNAL_HPP

#include "core/com/signal_base.hpp"

#include <core/com/slot_connection.hpp>
#include <core/mt/types.hpp>

#include <list>
#include <map>
#include <type_traits>
#include <utility>

namespace sight::core::com
{

struct slot_connection_base;
struct slot_base;

template<typename F>
struct signal;

template<typename F>
struct slot_run;

/**
 * @brief Signal implementation.
 * Template parameter T must always be void.
 */
template<typename R, typename ... A>
struct CORE_CLASS_API signal<R(A ...)>: signal_base
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
    connection connect(SPTR(slot_base) _slot) override;

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    void disconnect(SPTR(slot_base) _slot) override;

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
    connection get_connection(SPTR(slot_base) _slot, bool _throws = false);

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
        connection connect(SPTR(slot_base) _slot);

        /// Connected slots.
        slot_container_t m_slots;

        /// Container of current connections.
        connection_map_type m_connections;

        mutable core::mt::read_write_mutex m_connections_mutex;

    private:

        static_assert((std::is_same<void, R>::value));
};

} // namespace sight::core::com
