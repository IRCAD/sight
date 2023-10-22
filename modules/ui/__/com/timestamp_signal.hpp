/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "modules/ui/__/config.hpp"

#include <ui/__/action.hpp>

namespace sight::module::ui::com
{

/**
 * @brief action that sends a timestamp-signal when it is triggered.
 *
 * @section Signals Signals
 * - \b triggered(type) : Emitted when the action is triggered, with the timestamp as a high_res_clock_t.
 * - \b triggeredAsString(string) : Emitted when the action is triggered, with the timestamp as a string.
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

class MODULE_UI_CLASS_API timestamp_signal : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(timestamp_signal, sight::ui::action);

    /// Type of triggered signal
    typedef core::com::signal<void (sight::core::hires_clock::type)> triggered_signal_t;
    typedef core::com::signal<void (std::string)> TriggeredAsStringSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_API timestamp_signal() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_API ~timestamp_signal() noexcept override;

protected:

    /// Configures the service
    MODULE_UI_API void configuring() override;

    /// Register the action and check if the action is executable.
    MODULE_UI_API void starting() override;

    /// Unregister the action.
    MODULE_UI_API void stopping() override;

    /// Emit the signal
    MODULE_UI_API void updating() override;

    /// Give information about the class. Do nothing.
    MODULE_UI_API void info(std::ostream& _sstream) override;

    /// Whether to use system clock or high resolution clock
    /// to compute the timestamp.
    bool m_useSystemClock {false};

    /// Whether to format the timestamp as a date representation (true),
    /// or to convert the timestamp plainly.
    bool m_formatStringAsDate {true};

    /// Signal triggered when action has been triggered
    SPTR(triggered_signal_t) m_sigTriggered;

    /// Signal triggered when action has been triggered (message sent as string)
    SPTR(TriggeredAsStringSignalType) m_sigTriggeredAsString;
};

} // namespace sight::module::ui::com
