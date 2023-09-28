/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/config.hpp"

#include <core/com/signal.hpp>
#include <core/mt/types.hpp>

#include <cstdint>
#include <functional>
#include <future>
#include <string>
#include <vector>

namespace sight::core::jobs
{

/**
 * @brief This class is an interface for class managing job.
 */
class CORE_CLASS_API base
{
public:

    /**
     * @brief Job's states
     *
     * State :
     * - WAITING : The job is ready to be used
     * - RUNNING : The job is currently running
     * - CANCELING : The job received a cancel request
     * - CANCELED : The job is canceled
     * - FINISHED : The job have finished to run his task
     */
    enum state
    {
        WAITING = 0,
        RUNNING,
        CANCELING,
        CANCELED,
        FINISHED
    };

    /**
     * @name Typedef used in base
     * @{ */
    typedef SPTR(core::jobs::base) sptr;
    typedef WPTR(core::jobs::base) wptr;

    /// Cancel request callback type
    typedef std::function<bool ()> cancel_request_callback;

    /// Job cancel callback type for cancel hook
    typedef std::function<void (base&)> job_cancel_hook;

    /// Log callback type for log hook
    typedef std::function<void (base&, const std::string&)> log_hook;

    /// Done work callback type for done work hook
    typedef std::function<void (base&, std::uint64_t /*oldDoneWork*/)> done_work_hook;

    /// Work units callback type for total work unit hook
    typedef std::function<void (base&, std::uint64_t /*oldTotalWorkUnits*/)> total_work_units_hook;

    /// Cancel callback type for cancel hook
    typedef std::function<void ()> cancel_hook;

    /// State callback type for state hook
    typedef std::function<void (state)> state_hook;

    /// Job cancel callback sequence type for cancel hook
    typedef std::vector<job_cancel_hook> cancel_hook_seq;

    /// Done work callback sequence type for done work hook
    typedef std::vector<done_work_hook> done_work_hook_seq;

    /// Work units callback sequence type for total work unit hook
    typedef std::vector<total_work_units_hook> total_work_units_hook_seq;

    /// Log callback sequence type for log hook
    typedef std::vector<log_hook> log_hook_seq;

    /// Log callback sequence type for state hook
    typedef std::vector<state_hook> state_hook_seq;

    /// Log container type
    typedef std::vector<std::string> logs;

    /// State signal type
    typedef core::com::signal<void ()> state_signal;

    /// Cancel request signal type
    typedef core::com::signal<void ()> cancel_requested_signal;

    /// Done work signal type
    typedef core::com::signal<void (std::uint64_t, std::uint64_t)> done_work_signal;

    /// Log signal type
    typedef core::com::signal<void (std::string)> log_signal;

    /// Future type
    typedef std::shared_future<void> shared_future;
    /**  @} */

    /**
     * @brief Default constructor.
     *
     * @param name The name of the job.
     */
    CORE_API base(std::string name = "");

    /// Default destructor.
    CORE_API virtual ~base();

    /// Getter on the number of done work units.
    CORE_API std::uint64_t get_done_work_units() const;

    /// Getter on the total number of work units.
    CORE_API std::uint64_t get_total_work_units() const;

    /// Getter on the current State. @see State
    CORE_API state get_state() const;

    ///Getter on the name of the job.
    CORE_API const std::string& name() const;

    /// Getter on the log container
    CORE_API logs get_logs() const;

    /// Getter on cancelable, returns whether the job is cancelable.
    CORE_API bool is_cancelable() const;

    /// Setter on cancelable.
    CORE_API void set_cancelable(bool cancel);

    /**
     * @brief Run the current job
     *
     * @return future on the current running job
     */
    CORE_API shared_future run();

    /**
     * @brief Wait job execution ending.
     *
     * This method raise a core::jobs::exception::Waiting Exception if the job is
     * not running.
     *
     */
    CORE_API void wait();

    /**
     * @brief Returns the job canceling status.
     *
     * @return A boolean: true if cancel have been requested.
     */
    CORE_API bool cancel_requested() const;

    /**
     * @brief Returns a callback on job canceling status.
     * This callback can only be used if the job is still instantiated.
     * @return A callback to check if the job have been requested.
     */
    CORE_API cancel_request_callback cancel_requested_callback() const;

    /**
     * @brief Cancel the current job and call all available cancel callbacks
     *
     * @return  A future on the current running job or an empty future if the job
     * is canceled
     */
    CORE_API virtual shared_future cancel();

    /**
     * @brief Add cancel callback to sequence for cancel hook
     *
     * @param callback cancel callback
     */
    CORE_API void add_simple_cancel_hook(cancel_hook callback);

    /**
     * @brief Add cancel callback to sequence for cancel hook
     *
     * @param callback job cancel callback, taking a base as parameter
     */
    CORE_API void add_cancel_hook(job_cancel_hook callback);

    /**
     * @brief Add job done work unit callback to sequence for done work hook
     *
     * @param callback job done work unit callback, taking a base as parameter
     */
    CORE_API void add_done_work_hook(done_work_hook callback);

    /**
     * @brief Add job work unit callback to sequence for total work unit hook
     *
     * @param callback job work unit callback, taking a base as parameter
     */
    CORE_API void add_total_work_units_hook(total_work_units_hook callback);

    /**
     * @brief Add job log callback to sequence for log hook
     *
     * @param callback job log callback, taking a std::string message as parameter
     */
    CORE_API void add_log_hook(log_hook callback);

    /**
     * @brief Add job state callback to sequence for state hook
     *
     * @param callback job state callback, taking a State as parameter
     */
    CORE_API void add_state_hook(state_hook callback);

    /**
     * @brief Log a message.
     *
     * Push the message in the log container and emit a logged
     * signal with log as parameter. finally call all log callbacks
     *
     * @param log the message to log
     */
    CORE_API void log(const std::string& message);

protected:

    /**
     * @name Not implemented
     * @{ */
    base(base&);
    base& operator=(const base&);
    base(base&&) noexcept;
    base& operator=(base&&) noexcept;
    /**  @} */

    /// Run an instantiated job
    CORE_API virtual shared_future run_impl() = 0;

    /// Finish the job: set state to finished or canceled.
    CORE_API virtual void finish();

    /// Finish the job without mutex lock: set the state to finished or canceled.
    CORE_API void finish_no_lock();

    /// Getter on the state without mutex lock
    CORE_API state get_state_no_lock() const;

    /// Setter on the state
    CORE_API void set_state(state state);

    /// Setter on the state without mutex lock
    CORE_API void set_state_no_lock(state state);

    /// Setter on done work units
    CORE_API void done_work(std::uint64_t units);

    /**
     * @brief Setter on done work units
     * @warning This method leaves \p lock in an unlocked state
     *
     * @param units new done work units
     * @param lock mutex to upgrade to write lock
     */
    CORE_API void done_work(std::uint64_t units, core::mt::read_to_write_lock& lock);

    /// Set done work units to total work units
    CORE_API void done();

    /**
     * @brief Setter on total work units.
     *
     * @param units New total for work units.
     */
    CORE_API void set_total_work_units(std::uint64_t units);

    /**
     * @brief Setter on total work units
     * @warning This method leaves \p lock in an unlocked state
     *
     * @param units new total work units
     * @param lock mutex to upgrade to write lock
     */
    CORE_API void set_total_work_units_upgrade_lock(std::uint64_t units, core::mt::read_to_write_lock& lock);

    /**
     * @brief Add job cancel callback to sequence without mutex lock for cancel hook
     *
     * @param callback job cancel callback, taking a base as parameter or not
     */
    CORE_API void add_cancel_hook_no_lock(job_cancel_hook callback);

    /**
     * @brief Add job done work unit callback to sequence without mutex lock for done work hook
     *
     * @param callback job done work unit callback, taking a base as parameter
     */
    CORE_API void add_done_work_hook_no_lock(done_work_hook callback);

    /**
     * @brief Add job work unit callback to sequence without mutex lock for total work unit hook
     *
     * @param callback job work unit callback, taking a base as parameter
     */
    CORE_API void add_total_work_units_hook_no_lock(total_work_units_hook callback);

    /**
     * @brief Add job log callback to sequence without mutex lock for log hook
     *
     * @param callback job log callback, taking a std::string message as parameter
     */
    CORE_API void add_log_hook_no_lock(log_hook callback);

    /**
     * @brief Add cancel callback to sequence without mutex lock for state hook
     *
     * @param callback cancel callback, taking a State as parameter
     */
    CORE_API void add_state_hook_no_lock(state_hook callback);

    /**
     * @brief Add a message to thelog sequence.
     *
     * @param message the message to add to the sequence
     */
    CORE_API void log_no_lock(const std::string& message);

    /// Signal emitted when cancel has been requested
    SPTR(cancel_requested_signal) m_sig_cancel_requested;

    /// Signal emitted when te job has been canceled.
    SPTR(state_signal) m_sig_canceled;

    /// Signal emitted when the job has been started.
    SPTR(state_signal) m_sig_started;

    /// Signal emitted when the job has been finished.
    SPTR(state_signal) m_sig_finished;

    /// Signal emitted when done work units have been changed. Takes a std::uint64_t as parameter.
    SPTR(done_work_signal) m_sig_done_work;

    /// Signal emitted when a message has been added to logs. Takes a std::string as parameter.
    SPTR(log_signal) m_sig_logged;

    /// Mutex to protect object access.
    mutable core::mt::read_write_mutex m_mutex;

    /// Job's name
    std::string m_name;

    /// Logs container
    logs m_logs;

    /// Number of work units already reached.
    std::uint64_t m_done_work_units {0};

    /// Number of work units to reach to complete the job.
    std::uint64_t m_total_work_units {0};

    /// Determines if cancellation has been requested. The job is/will be canceling if true.
    bool m_cancel_requested {false};

    /// Determines if the job can be cancelled. The job is cancelable if true.
    bool m_cancelable {true};

    /// Container of cancel callbacks. Cancel callbacks will be run when job is canceling.
    cancel_hook_seq m_cancel_hooks;

    /// Container of done work callbacks. These callbacks take nothing or a base as parameter.
    done_work_hook_seq m_done_work_hooks;

    /// Container of total work unit callbacks. These callbacks take a base as parameter.
    total_work_units_hook_seq m_total_work_units_hooks;

    /// Container of log callbacks. These callbacks take a std::string as parameter.
    log_hook_seq m_log_hooks;

    /// Container of state callbacks. These callbacks take a State as parameter.
    state_hook_seq m_state_hooks;

    /// Job's future returned by 'run'
    shared_future m_run_future;

    /// Job's state
    state m_state {WAITING};
};

} //namespace sight::core::jobs
