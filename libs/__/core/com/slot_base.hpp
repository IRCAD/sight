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

#define FWCOM_SLOTBASE_HPP

#include "core/com/util/convert_function_type.hpp"
#include "core/config.hpp"
#include "core/thread/worker.hpp"

#include <core/base_object.hpp>
#include <core/mt/types.hpp>
#include <core/spy_log.hpp>

#include <future>
#include <queue>
#include <set>

namespace sight::core::thread
{

class worker;

} // namespace sight::core::thread

namespace sight::core::com
{

namespace util
{

template<typename T, typename R>
struct weak_call;

} // namespace util

template<typename F>
struct slot_run;

template<typename F>
class slot;

struct slot_connection_base;

/**
 * @brief Base class for Slot implementations.
 */
struct CORE_CLASS_API slot_base : virtual core::base_object
{
    /**
     * @name typedefs
     * @{
     * Slot pointer types. */
    using sptr  = std::shared_ptr<slot_base>;
    using wptr  = std::weak_ptr<slot_base>;
    using csptr = std::shared_ptr<const slot_base>;
    using cwptr = std::weak_ptr<const slot_base>;

    using idtype = std::string;
    /**  @} */

    /// SlotBase::async_run return type.
    using void_shared_future_type = std::shared_future<void>;

    /// Connections container type
    using connection_set_type = std::set<std::shared_ptr<const slot_connection_base> >;

    ~slot_base() override
    = default;

    /**
     * @brief Returns Slot's arity.
     * The arity defines the number of parameters in Slot signature.
     */
    unsigned int arity() const
    {
        return m_arity;
    }

    /// Sets Slot's Worker.
    void set_worker(const SPTR(core::thread::worker)& _worker)
    {
        core::mt::write_lock lock(m_worker_mutex);
        m_worker = _worker;
    }

    /// Returns Slot's Worker.
    SPTR(core::thread::worker) get_worker() const
    {
        core::mt::read_lock lock(m_worker_mutex);
        return m_worker;
    }

    /**
     * @brief  Run the Slot.
     * @throw  BadRun if given arguments do not match the slot implementation
     */
    /**
     * @name Run helpers
     * @{ */
    template<typename A1, typename A2, typename A3>
    void run(A1 _a1, A2 _a2, A3 _a3) const;

    template<typename A1, typename A2>
    void run(A1 _a1, A2 _a2) const;

    template<typename A1>
    void run(A1 _a1) const;

    CORE_API void run() const;
    /**  @} */

    /**
     * @brief  Call the Slot (with return value).
     * @throw  BadCall if given arguments do not match the slot implementation
     */
    /**
     * @name Call helpers
     * @{ */
    template<typename R, typename A1, typename A2, typename A3>
    R call(A1 _a1, A2 _a2, A3 _a3) const;

    template<typename R, typename A1, typename A2>
    R call(A1 _a1, A2 _a2) const;

    template<typename R, typename A1>
    R call(A1 _a1) const;

    template<typename R>
    R call() const;
    /**  @} */

    /**
     * @brief  Run the Slot asynchronously.
     * @throw  BadRun if given arguments do not match the slot implementation
     * @throws NoWorker if given worker is not valid.
     */
    /**
     * @name Asynchronous run helpers
     * @{ */
    template<typename A1, typename A2, typename A3>
    void_shared_future_type async_run(A1 _a1, A2 _a2, A3 _a3) const;

    template<typename A1, typename A2>
    void_shared_future_type async_run(A1 _a1, A2 _a2) const;

    template<typename A1>
    void_shared_future_type async_run(A1 _a1) const;

    CORE_API void_shared_future_type async_run() const;
    /**  @} */

    /**
     * @brief  Call the Slot asynchronously (with return value).
     * @throw  BadCall if given arguments do not match the slot implementation
     * @throws NoWorker if given worker is not valid.
     */
    /**
     * @name Asynchronous call helpers
     * @{ */
    template<typename R, typename A1, typename A2, typename A3>
    std::shared_future<R> async_call(A1 _a1, A2 _a2, A3 _a3) const;

    template<typename R, typename A1, typename A2>
    std::shared_future<R> async_call(A1 _a1, A2 _a2) const;

    template<typename R, typename A1>
    std::shared_future<R> async_call(A1 _a1) const;

    template<typename R>
    std::shared_future<R> async_call() const;
    /**  @} */

    /// Returns number of connections.
    std::size_t num_connections() const
    {
        core::mt::read_lock lock(m_connections_mutex);
        return m_connections.size();
    }

    protected:

        /// Copy constructor forbidden
        slot_base(const slot_base&);

        /// Copy operator forbidden
        slot_base& operator=(const slot_base&);

        /**
         * @name SlotBase's friends
         * @{ */
        template<typename F>
        friend struct slot_connection;

        template<typename F>
        friend struct signal;

        template<typename T, typename R>
        friend struct util::weak_call;

        /**  @} */

        /// Returns F typeid name.
        template<typename F>
        std::string get_type_name() const
        {
            return std::string("function_type(") + std::string(typeid(F).name()) + std::string(")");
        }

        slot_base(unsigned int _arity) :
            m_arity(_arity)
        {
        }

        /// Slot's signature based on typeid.
        std::string m_signature;

        /// Slot's arity.
        const unsigned int m_arity;

        /// Slot's Worker.
        SPTR(core::thread::worker) m_worker;

        /// When the slot is wrapped to reduce the number of arguments in a connection, this stores a pointer
        /// to the original slot. This is important in the mechanism used to keep the slot alive during an async call.
        WPTR(slot_base) m_source_slot;

        /// Container of current connections.
        connection_set_type m_connections;

        mutable core::mt::read_write_mutex m_connections_mutex;
        mutable core::mt::read_write_mutex m_worker_mutex;
};

} // namespace sight::core::com
