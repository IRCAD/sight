/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/com/has_slots.hpp"
#include "core/com/signal.hpp"
#include "core/com/signals.hpp"

namespace sight::core::com
{

struct signal_base;

/**
 * @brief   This class proposes a mapping between a key_t and a signal_base.
 */
class SIGHT_CORE_CLASS_API has_signals
{
public:

    using sptr         = std::shared_ptr<has_signals>;
    using csptr        = std::shared_ptr<const has_signals>;
    using signal_key_t = sight::core::com::signals::key_t;

    has_signals()          = default;
    virtual ~has_signals() = default;

    [[nodiscard]] SPTR(signal_base) signal(const signals::key_t& _key) const
    {
        return m_signals[_key];
    }

    template<typename signal_type>
    [[nodiscard]] SPTR(signal_type) signal(const signals::key_t& _key) const
    {
        auto signal = std::dynamic_pointer_cast<signal_type>(this->signal(_key));
        return signal;
    }

    template<typename signal_type>
    SPTR(signal_type) new_signal(const signals::key_t& _key)
    {
        auto sig = std::make_shared<signal_type>();
        m_signals(_key, sig);
        return sig;
    }

    /**
     * @brief Emit the signal identified by its key
     *
     * @tparam A Type and number of signal arguments
     * @param _key Identifier of the signal
     * @param _a Arguments of the signal
     */
    template<typename ... A>
    void emit(const signals::key_t& _key, A ... _a) const
    {
        auto signal = this->typed_signal<A ...>(_key);
        signal->emit(_a ...);
    }

    /**
     * @brief Emit the signal identified by its key.
     * This alternative signature takes an instance of sight::core::com::has_slots
     * and blocks every slot connected with the signal, preventing infinite loops.
     *
     * @tparam A Type and number of signal arguments
     * @param _caller Caller of the signal
     * @param _key Identifier of the signal
     * @param _a Arguments of the signal
     */
    template<typename ... A>
    void emit(com::has_slots* _caller, const signals::key_t& _key, A ... _a) const
    {
        SIGHT_ASSERT("Caller is null", _caller);

        auto signal   = this->typed_signal<A ...>(_key);
        auto blockers = block_connections(_caller, signal);
        signal->emit(_a ...);
    }

    /**
     * @brief Emit asynchronously the signal identified by its key
     *
     * @tparam A Type and number of signal arguments
     * @param _key Identifier of the signal
     * @param _a Arguments of the signal
     */
    template<typename ... A>
    void async_emit(const signals::key_t& _key, A ... _a) const
    {
        auto signal = this->typed_signal<A ...>(_key);
        signal->async_emit(_a ...);
    }

    /**
     * @brief Emit asynchronously the signal identified by its key.
     * This alternative signature takes an instance of sight::core::com::has_slots
     * and blocks every slot connected with the signal, preventing infinite loops.
     *
     * @tparam A Type and number of signal arguments
     * @param _caller Caller of the signal
     * @param _key Identifier of the signal
     * @param _a Arguments of the signal
     */
    template<typename ... A>
    void async_emit(com::has_slots* _caller, const signals::key_t& _key, A ... _a) const
    {
        SIGHT_ASSERT("Caller is null", _caller);

        auto signal   = this->typed_signal<A ...>(_key);
        auto blockers = block_connections(_caller, signal);
        signal->async_emit(_a ...);
    }

protected:

    /// Copy constructor forbidden
    has_signals(const has_signals&);

    /// Copy operator forbidden
    has_signals& operator=(const has_signals&);

    signals m_signals;

private:

    using blockers_t = std::vector<core::com::connection::blocker>;

    //------------------------------------------------------------------------------

    /**
     * @brief Return the signal with the appropriate function signature matching the arguments.
     *
     * @tparam A Type and number of signal arguments
     * @param _key Identifier of the signal
     * @return signal with the function signature, ready to be emitted.
     */
    template<typename ... A>
    core::com::signal<void(A ...)>::sptr typed_signal(const signals::key_t& _key) const
    {
        auto signal = this->signal(_key);
        SIGHT_ASSERT("Can't find signal " << std::quoted(_key), signal);
        auto typed_signal = std::dynamic_pointer_cast<core::com::signal<void(A ...)> >(signal);
        SIGHT_ASSERT("Signal " << std::quoted(_key) << " does not match the required signature", typed_signal);
        return typed_signal;
    }

    /**
     * @brief Block every slot connected with the caller
     *
     * @param _caller Caller of the signal
     * @param _signal Signal about to be emitted
     */
    blockers_t block_connections(com::has_slots* _caller, const core::com::signal_base::sptr& _signal) const
    {
        blockers_t blockers;
        for(auto& slot : _caller->slots())
        {
            auto connection = _signal->get_connection(slot.second);
            if(!connection.expired())
            {
                blockers.emplace_back(core::com::connection::blocker(connection));
            }
        }

        return blockers;
    }
};

} // namespace sight::core::com
