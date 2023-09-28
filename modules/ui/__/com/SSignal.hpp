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

#include "modules/ui/__/config.hpp"

#include <ui/__/action.hpp>

namespace sight::module::ui::com
{

/**
 * @brief   Action that sends a signal when it is triggered
 * * @deprecated Replaced by sight::module::ui::SAction since Sight 22.0, will be removed in Sight 24.0
 *
 * @section Signals Signals
 * - \b triggered(bool) : Emitted when the action is triggered, with the state of the action as parameter.
 * - \b cancelled(bool) : Emitted when the user cancel the action, if confirmation has been set up in the configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::com::SSignal" />
   @endcode
 *
 * See also sight::ui::action::initialize for more configuration parameters.
 * @see action::initialize
 */

class MODULE_UI_CLASS_API SSignal : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(SSignal, sight::ui::action);

    /// Type of triggered signal
    typedef core::com::signal<void (bool)> TriggeredSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_API SSignal() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_API ~SSignal() noexcept override;

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

    /// Signal trigerred when action has been triggered
    SPTR(TriggeredSignalType) m_sigTriggered;
    /// Signal trigerred when action has been cancelled
    SPTR(TriggeredSignalType) m_sigCancelled;
};

} // namespace sight::module::ui::com
