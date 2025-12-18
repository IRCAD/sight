/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include <core/com/signal.hpp>
#include <core/mt/types.hpp>
#include <core/progress/function.hpp>

#include <cstdint>
#include <functional>
#include <future>
#include <string>
#include <vector>

namespace sight::core::progress
{

/**
 * @brief  Represents a monitor class for managing long tasks.
 *
 * The monitor class provides functionality for tracking tasks states, progress,
 * and handling callbacks for various events such as logging, cancellation,
 * progress updates, and state changes. It also allows tasks to be canceled and
 * provides mechanisms to hook custom behaviors through callbacks at different
 * stages of the task lifecycle.
 */
class SIGHT_CORE_CLASS_API monitor : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(monitor, core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief monitor's states
     *
     * State :
     * - WAITING : The monitor is ready to be used
     * - RUNNING : The monitor is currently running
     * - CANCELING : The monitor received a cancel request
     * - CANCELED : The monitor is canceled
     * - FINISHED : The monitor have finished to run his task
     */
    enum state
    {
        waiting = 0,
        running,
        canceling,
        canceled,
        finished
    };

    /**
     * @name Typedef used in monitor
     * @{ */
    /// Log callback type for log hook
    using log_hook = std::function<void (monitor&, const std::string&)>;

    /// Done work callback type for done work hook
    using done_work_hook = std::function<void (monitor&, std::uint64_t)>;

    /// Work units callback type for total work unit hook
    using total_work_units_hook = std::function<void (monitor&, std::uint64_t)>;

    /// Cancel callback type for cancel hook
    using cancel_hook = std::function<void ()>;

    /// State callback type for state hook
    using state_hook = std::function<void (state)>;

    /// monitor cancel callback sequence type for cancel hook
    using cancel_hook_seq = std::vector<cancel_hook>;

    /// Done work callback sequence type for done work hook
    using done_work_hook_seq = std::vector<done_work_hook>;

    /// Work units callback sequence type for total work unit hook
    using total_work_units_hook_seq = std::vector<total_work_units_hook>;

    /// Log callback sequence type for log hook
    using log_hook_seq = std::vector<log_hook>;

    /// Log callback sequence type for state hook
    using state_hook_seq = std::vector<state_hook>;

    /// Log container type
    using logs = std::vector<std::string>;

    /// State signal type
    using state_signal = core::com::signal<void ()>;

    /// Cancel request signal type
    using cancel_requested_signal = core::com::signal<void ()>;

    /// Done work signal type
    using done_work_signal = core::com::signal<void (std::uint64_t, std::uint64_t)>;

    /// Log signal type
    using log_signal = core::com::signal<void (std::string)>;

    /// Future type
    using shared_future = std::shared_future<void>;
    /**  @} */

    /**
     * @brief Default constructor.
     *
     * @param _name The name of the monitor.
     */
    SIGHT_CORE_API explicit monitor(std::string _name = "");

    /// Default destructor.
    SIGHT_CORE_API ~monitor() override;

    /**
     * @name Not implemented
     * @{ */
    monitor(monitor&)                      = delete;
    monitor& operator=(const monitor&)     = delete;
    monitor(monitor&&) noexcept            = delete;
    monitor& operator=(monitor&&) noexcept = delete;
    /**  @} */

    /// Getter on the number of done work units.
    SIGHT_CORE_API std::uint64_t get_done_work_units() const;

    /// Getter on the total number of work units.
    SIGHT_CORE_API std::uint64_t get_total_work_units() const;

    /// Getter on the current State. @see State
    SIGHT_CORE_API state get_state() const;

    ///Getter on the name of the monitor.
    SIGHT_CORE_API const std::string& name() const;

    /// Getter on the log container
    SIGHT_CORE_API logs get_logs() const;

    /// Getter on cancelable, returns whether the monitor is cancelable.
    SIGHT_CORE_API bool is_cancelable() const;

    /// Setter on cancelable.
    SIGHT_CORE_API void set_cancelable(bool _cancel);

    /**
     * @brief Returns the monitor canceling status.
     *
     * @return A boolean: true if cancel have been requested.
     */
    SIGHT_CORE_API bool cancel_requested() const;

    /**
     * @brief Returns a callback on monitor canceling status.
     * This callback can only be used if the monitor is still instantiated.
     * @return A callback to check if the monitor have been requested.
     */
    SIGHT_CORE_API cancel_request_callback_t cancel_requested_callback() const;

    /**
     * @brief Cancel the current monitor and call all available cancel callbacks.
     */
    SIGHT_CORE_API virtual void cancel();

    /**
     * @brief Add cancel callback to sequence for cancel hook
     *
     * @param _callback monitor cancel callback
     */
    SIGHT_CORE_API void add_cancel_hook(cancel_hook _callback);

    /**
     * @brief Add monitor done work unit callback to sequence for done work hook
     *
     * @param _callback monitor done work unit callback, taking a monitor as parameter
     */
    SIGHT_CORE_API void add_done_work_hook(done_work_hook _callback);

    /**
     * @brief Add monitor work unit callback to sequence for total work unit hook
     *
     * @param _callback monitor work unit callback, taking a monitor as parameter
     */
    SIGHT_CORE_API void add_total_work_units_hook(total_work_units_hook _callback);

    /**
     * @brief Add monitor log callback to sequence for log hook
     *
     * @param _callback monitor log callback, taking a std::string message as parameter
     */
    SIGHT_CORE_API void add_log_hook(log_hook _callback);

    /**
     * @brief Add monitor state callback to sequence for state hook
     *
     * @param _callback monitor state callback, taking a State as parameter
     */
    SIGHT_CORE_API void add_state_hook(state_hook _callback);

    /**
     * @brief Log a message.
     *
     * Push the message in the log container and emit a logged
     * signal with log as parameter. finally call all log callbacks
     *
     * @param _message the message to log
     */
    SIGHT_CORE_API void log(const std::string& _message);

protected:

    /// Run the monitor: set state to running.
    SIGHT_CORE_API void run();

    /// Finish the monitor: set state to finished or canceled.
    SIGHT_CORE_API virtual void finish();

    /// Finish the monitor without mutex lock: set the state to finished or canceled.
    SIGHT_CORE_API void finish_no_lock();

    /// Getter on the state without mutex lock
    SIGHT_CORE_API state get_state_no_lock() const;

    /// Setter on the state
    SIGHT_CORE_API void set_state(state _state);

    /// Setter on the state without mutex lock
    SIGHT_CORE_API void set_state_no_lock(state _state);

    /// Setter on done work units
    SIGHT_CORE_API void done_work(std::uint64_t _units);

    /**
     * @brief Setter on done work units
     * @warning This method leaves \p lock in an unlocked state
     *
     * @param _units  new done work units
     * @param _lock   mutex to upgrade to write lock
     */
    SIGHT_CORE_API void done_work(std::uint64_t _units, core::mt::read_to_write_lock& _lock);

    /// Set done work units to total work units
    SIGHT_CORE_API void done();

    /**
     * @brief Setter on total work units.
     *
     * @param _units New total for work units.
     */
    SIGHT_CORE_API void set_total_work_units(std::uint64_t _units);

    /**
     * @brief Setter on total work units
     * @warning This method leaves \p lock in an unlocked state
     *
     * @param _units new total work units
     * @param _lock mutex to upgrade to write lock
     */
    SIGHT_CORE_API void set_total_work_units_upgrade_lock(std::uint64_t _units, core::mt::read_to_write_lock& _lock);

    /**
     * @brief Add monitor cancel callback to sequence without mutex lock for cancel hook
     *
     * @param _callback monitor cancel callback, taking a monitor as parameter or not
     */
    SIGHT_CORE_API void add_cancel_hook_no_lock(cancel_hook _callback);

    /**
     * @brief Add monitor done work unit callback to sequence without mutex lock for done work hook
     *
     * @param _callback monitor done work unit callback, taking a monitor as parameter
     */
    SIGHT_CORE_API void add_done_work_hook_no_lock(done_work_hook _callback);

    /**
     * @brief Add monitor work unit callback to sequence without mutex lock for total work unit hook
     *
     * @param _callback monitor work unit callback, taking a monitor as parameter
     */
    SIGHT_CORE_API void add_total_work_units_hook_no_lock(total_work_units_hook _callback);

    /**
     * @brief Add monitor log callback to sequence without mutex lock for log hook
     *
     * @param _callback monitor log callback, taking a std::string message as parameter
     */
    SIGHT_CORE_API void add_log_hook_no_lock(log_hook _callback);

    /**
     * @brief Add cancel callback to sequence without mutex lock for state hook
     *
     * @param _callback cancel callback, taking a State as parameter
     */
    SIGHT_CORE_API void add_state_hook_no_lock(state_hook _callback);

    /**
     * @brief Add a message to the log sequence.
     *
     * @param _message the message to add to the sequence
     */
    SIGHT_CORE_API void log_no_lock(const std::string& _message);

    /// Signal emitted when cancel has been requested
    SPTR(cancel_requested_signal) m_sig_cancel_requested;

    /// Signal emitted when the monitor has been canceled.
    SPTR(state_signal) m_sig_canceled;

    /// Signal emitted when the monitor has been started.
    SPTR(state_signal) m_sig_started;

    /// Signal emitted when the monitor has been finished.
    SPTR(state_signal) m_sig_finished;

    /// Signal emitted when done work units have been changed. Takes a std::uint64_t as parameter.
    SPTR(done_work_signal) m_sig_done_work;

    /// Signal emitted when a message has been added to logs. Takes a std::string as parameter.
    SPTR(log_signal) m_sig_logged;

    /// Mutex to protect object access.
    mutable core::mt::read_write_mutex m_mutex;

    /// monitor's name
    std::string m_name;

    /// Logs container
    logs m_logs;

    /// Number of work units already reached.
    std::uint64_t m_done_work_units {0};

    /// Number of work units to reach to complete the monitor.
    std::uint64_t m_total_work_units {0};

    /// Determines if cancellation has been requested. The monitor is/will be canceling if true.
    bool m_cancel_requested {false};

    /// Determines if the monitor can be cancelled. The monitor is cancelable if true.
    bool m_cancelable {false};

    /// Container of cancel callbacks. Cancel callbacks will be run when monitor is canceling.
    cancel_hook_seq m_cancel_hooks;

    /// Container of done work callbacks. These callbacks take nothing or a monitor as parameter.
    done_work_hook_seq m_done_work_hooks;

    /// Container of total work unit callbacks. These callbacks take a monitor as parameter.
    total_work_units_hook_seq m_total_work_units_hooks;

    /// Container of log callbacks. These callbacks take a std::string as parameter.
    log_hook_seq m_log_hooks;

    /// Container of state callbacks. These callbacks take a State as parameter.
    state_hook_seq m_state_hooks;

    /// monitor's state
    state m_state {waiting};
};

} //namespace sight::core::progress
