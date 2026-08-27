/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "base.hpp"

#include <cstdint>
#include <functional>
#include <future>
#include <string>

namespace sight::core::notification
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
class SIGHT_CORE_CLASS_API monitor : public base
{
public:

    SIGHT_DECLARE_CLASS(monitor, base);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @name Typedef used in monitor
     * @{ */
    /// Log callback type for log hook
    using log_hook = std::function<void (monitor&, const std::string&)>;

    /// Done work callback type for done work hook
    using done_work_hook = std::function<void (monitor&, std::uint64_t)>;

    /// Work units callback type for total work unit hook
    using total_work_units_hook = std::function<void (monitor&, std::uint64_t)>;

    /// State callback type for state hook
    using state_hook = base::state_hook;

    /// Done work callback sequence type for done work hook
    using done_work_hook_seq = std::vector<done_work_hook>;

    /// Work units callback sequence type for total work unit hook
    using total_work_units_hook_seq = std::vector<total_work_units_hook>;

    /// Log callback sequence type for log hook
    using log_hook_seq = std::vector<log_hook>;

    /// Log container type
    using logs = std::vector<std::string>;

    /// Future type
    using shared_future = std::shared_future<void>;
    /**  @} */

    struct signals final : public base::signals
    {
        /// Done work signal type
        using done_work_t = core::com::signal<void (std::uint64_t, std::uint64_t)>;

        /// Log signal type
        using log_t = core::com::signal<void (std::string)>;

        static inline const core::com::signals::key_t DONE_WORK = "done_work";
        static inline const core::com::signals::key_t LOGGED    = "logged";
    };

    /**
     * @brief Default constructor.
     *
     * @param _name The name of the monitor.
     */
    SIGHT_CORE_API explicit monitor(
        std::string _name        = {},
        bool _cancelable         = false,
        cancel_hook _cancel_hook = nullptr
    );

    /// Default destructor.
    SIGHT_CORE_API ~monitor() override = default;

    /**
     * @name Not implemented
     * @{ */
    monitor(monitor&)                      = delete;
    monitor& operator=(const monitor&)     = delete;
    monitor(monitor&&) noexcept            = delete;
    monitor& operator=(monitor&&) noexcept = delete;
    /**  @} */

    /// Getter on the name of the monitor.
    SIGHT_CORE_API const std::string& name() const;

    /// Getter on the number of done work units.
    SIGHT_CORE_API std::uint64_t get_done_work_units() const;

    /// Getter on the total number of work units.
    SIGHT_CORE_API std::uint64_t get_total_work_units() const;

    /// Getter on the log container
    SIGHT_CORE_API logs get_logs() const;

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
     * @brief Log a message.
     *
     * Push the message in the log container and emit a logged
     * signal with log as parameter. finally call all log callbacks
     *
     * @param _message the message to log
     */
    SIGHT_CORE_API void log(const std::string& _message);

protected:

    /// Add done work units
    SIGHT_CORE_API void add_done_work(std::uint64_t _units);

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

    /// Getter on the number of done work units.
    SIGHT_CORE_API std::uint64_t get_done_work_units_no_lock() const;

    /// Getter on the total number of work units.
    SIGHT_CORE_API std::uint64_t get_total_work_units_no_lock() const;

    /**
     * @brief Add monitor log callback to sequence without mutex lock for log hook
     *
     * @param _callback monitor log callback, taking a std::string message as parameter
     */
    SIGHT_CORE_API void add_log_hook_no_lock(log_hook _callback);

    /**
     * @brief Add a message to the log sequence.
     *
     * @param _message the message to add to the sequence
     */
    SIGHT_CORE_API void log_no_lock(const std::string& _message);

private:

    /// monitor's name
    std::string m_name;

    /// Logs container
    logs m_logs;

    /// Number of work units already reached.
    std::uint64_t m_done_work_units {0};

    /// Number of work units to reach to complete the monitor.
    std::uint64_t m_total_work_units {0};

    /// Container of done work callbacks. These callbacks take nothing or a monitor as parameter.
    done_work_hook_seq m_done_work_hooks;

    /// Container of total work unit callbacks. These callbacks take a monitor as parameter.
    total_work_units_hook_seq m_total_work_units_hooks;

    /// Container of log callbacks. These callbacks take a std::string as parameter.
    log_hook_seq m_log_hooks;
};

} //namespace sight::core::notification
