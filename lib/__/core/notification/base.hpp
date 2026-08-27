/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/base_object.hpp>
#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include "function.hpp"

#include <cstdint>
#include <functional>
#include <optional>

namespace sight::core::notification
{

/**
 * @brief Base class for all notifications.
 *
 * The base class provides common functionality for all notification types,
 * including state management, lifecycle hooks, and finish/finished status tracking.
 * It serves as the root interface for different notification implementations
 * like monitor, message, error, etc.
 */
class SIGHT_CORE_CLASS_API base : public core::base_object,
                                  public core::com::has_signals
{
public:

    SIGHT_DECLARE_CLASS(base, core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief notification states
     *
     * State :
     * - WAITING : The notification is ready to be used
     * - RUNNING : The notification is currently running
     * - CANCELING : A cancel has been requested
     * - CANCELED : The notification has been canceled
     * - FINISHED : The notification has finished
     */
    enum state_t : std::uint8_t
    {
        waiting = 0,
        running,
        canceling,
        canceled,
        finished
    };

    /**
     * @name Typedef used in base
     * @{ */

    /// Cancel callback type for cancel hooks
    using cancel_hook = cancel_callback_t;

    /// Cancel callback sequence type
    using cancel_hook_seq = std::vector<cancel_hook>;

    /// State callback type for state hooks
    using state_hook = std::function<void (state_t)>;

    /// State callback sequence type
    using state_hook_seq = std::vector<state_hook>;

    /**  @} */

    struct signals
    {
        using void_t = core::com::signal<void ()>;

        static inline const core::com::signals::key_t STARTED          = "started";
        static inline const core::com::signals::key_t FINISHED         = "finished";
        static inline const core::com::signals::key_t CANCEL_REQUESTED = "cancel_requested";
        static inline const core::com::signals::key_t CANCELED         = "canceled";
    };

    /// Default constructor.
    SIGHT_CORE_API explicit base(bool _cancelable = false, cancel_hook _cancel_hook = nullptr);

    /// Default destructor.
    SIGHT_CORE_API ~base() override;

    /**
     * @name Not implemented
     * @{ */
    base(base&)                      = delete;
    base& operator=(const base&)     = delete;
    base(base&&) noexcept            = delete;
    base& operator=(base&&) noexcept = delete;
    /**  @} */

    /// Getter on the current State. @see State
    SIGHT_CORE_API state_t state() const;

    /**
     * @brief Returns whether the notification is finished.
     *
     * @return A boolean: true if the notification is in finished state.
     */
    SIGHT_CORE_API bool is_finished() const;

    /**
     * @brief Returns whether cancellation has been requested.
     *
     * @return A boolean: true if cancel has been requested.
     */
    SIGHT_CORE_API bool cancel_requested() const;

    /**
     * @brief Returns a callback to check cancellation request status.
     */
    SIGHT_CORE_API cancel_request_callback_t cancel_requested_callback() const;

    /// Add a cancel hook callback.
    SIGHT_CORE_API void add_cancel_hook(cancel_hook _callback);

    /// Add a state hook callback.
    SIGHT_CORE_API void add_state_hook(state_hook _callback);

    /// Request cancellation and call all available cancel hooks.
    SIGHT_CORE_API virtual void cancel();

    /// Finish the notification: set state to finished.
    SIGHT_CORE_API virtual void finish();

    /// Getter on cancelable, returns whether the monitor is cancelable.
    SIGHT_CORE_API bool cancelable() const;

    /// Setter on cancelable.
    SIGHT_CORE_API void set_cancelable(bool _cancel);

protected:

    /// Run the notification: set state to running.
    SIGHT_CORE_API void run();

    /// Getter on the state without mutex lock
    SIGHT_CORE_API state_t get_state_no_lock() const;

    /// Setter on the state
    SIGHT_CORE_API void set_state(state_t _state);

    /// Setter on the state without mutex lock. Does not emit the state signal nor call state hooks:
    /// callers must call notify_state() once the mutex is released, see set_state_no_lock()'s usages.
    SIGHT_CORE_API void set_state_no_lock(state_t _state);

    /// Add a cancel hook callback without lock.
    SIGHT_CORE_API void add_cancel_hook_no_lock(cancel_hook _callback);

    /// Add a state hook callback without lock.
    SIGHT_CORE_API void add_state_hook_no_lock(state_hook _callback);

    /**
     * @brief Finish the notification without mutex lock: set the state to finished (or canceled if a
     * cancellation was in progress).
     *
     * Does not emit the state signal nor call state hooks: callers must call notify_state() with the
     * returned state once the mutex is released. Returns std::nullopt if the notification was already
     * finished or canceled, in which case notify_state() must not be called.
     */
    SIGHT_CORE_API std::optional<state_t> finish_no_lock();

    /**
     * @brief Emit the state signal and call state hooks for the given state.
     *
     * Must be called with the mutex unlocked: state hooks may call back into this object (e.g.
     * state()/is_finished()), and the mutex is not recursive.
     */
    SIGHT_CORE_API void notify_state(state_t _state);

    SIGHT_CORE_API core::mt::read_write_mutex& mutex() const;

private:

    /// Container of cancel callbacks.
    cancel_hook_seq m_cancel_hooks;

    /// Container of state callbacks.
    state_hook_seq m_state_hooks;

    /// Determines if cancellation has been requested.
    bool m_cancel_requested {false};

    /// Determines if the monitor can be cancelled. The monitor is cancelable if true.
    bool m_cancelable {false};

    /// notification's state
    state_t m_state {waiting};

    /// Mutex to protect object access.
    mutable core::mt::read_write_mutex m_mutex;
};

//------------------------------------------------------------------------------

inline core::mt::read_write_mutex& base::mutex() const
{
    return m_mutex;
}

} //namespace sight::core::notification
