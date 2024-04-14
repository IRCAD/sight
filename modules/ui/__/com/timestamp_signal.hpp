/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include <ui/__/action.hpp>

namespace sight::module::ui::com
{

/**
 * @brief action that sends a timestamp-signal when it is triggered.
 *
 * @section Signals Signals
 * - \b triggered(type) : Emitted when the action is triggered, with the timestamp as a high_res_clock_t.
 * - \b triggered_as_string(string) : Emitted when the action is triggered, with the timestamp as a string.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::com::timestamp_signal">
            <useSystemClock>false</useSystemClock>
        </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b useSystemClock (optional, default=false): Whether to use the system clock or high resolution clock
 *      for the emitted timestamp.
 *      If the timestamp is used to compute a date since epoch, set to true to use system clock,
 *      as it is the only one guaranteeing it, especially on Windows.
 *      If the timestamp is used to compute an interval, set to false to use high resolution clock
 *      for the best precision.
 *
 * See also sight::ui::action::initialize for more configuration parameters.
 * @see action::initialize
 */

class timestamp_signal : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(timestamp_signal, sight::ui::action);

    /// Type of triggered signal
    using triggered_signal_t_t         = core::com::signal<void (sight::core::clock::type)>;
    using triggered_as_string_signal_t = core::com::signal<void (std::string)>;

    /**
     * @brief Constructor. Do nothing.
     */
    timestamp_signal() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    ~timestamp_signal() noexcept override = default;

protected:

    /// Configures the service
    void configuring() override;

    /// Register the action and check if the action is executable.
    void starting() override;

    /// Unregister the action.
    void stopping() override;

    /// Emit the signal
    void updating() override;

    /// Give information about the class. Do nothing.
    void info(std::ostream& _sstream) override;

    /// Whether to use system clock or high resolution clock
    /// to compute the timestamp.
    bool m_use_system_clock {true};

    /// Whether to format the timestamp as a date representation (true),
    /// or to convert the timestamp plainly.
    bool m_format_string_as_date {true};

    /// Signal triggered when action has been triggered
    SPTR(triggered_signal_t_t) m_sig_triggered;

    /// Signal triggered when action has been triggered (message sent as string)
    SPTR(triggered_as_string_signal_t) m_sig_triggered_as_string;
};

} // namespace sight::module::ui::com
