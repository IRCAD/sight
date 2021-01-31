/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/base/config.hpp"

#include <ui/base/IActionSrv.hpp>

namespace sight::modules::ui::base
{
namespace action
{

/**
 * @brief   Action that sends a signal when it is triggered
 *
 * @section Signals Signals
 * - \b triggered(bool) : Emitted when the action is triggered, with the state of the action as parameter.
 * - \b cancelled(bool) : Emitted when the user cancel the action, if confirmation has been set up in the configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::sight::modules::ui::base::action::SSignal" />
   @endcode
 *
 * See also sight::ui::base::IActionSrv::initialize for more configuration parameters.
 * @see IActionSrv::initialize
 */

class MODULE_UI_BASE_CLASS_API SSignal : public ::sight::ui::base::IActionSrv
{

public:

    fwCoreServiceMacro(SSignal, ::sight::ui::base::IActionSrv)
    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /// Type of triggered signal
    typedef core::com::Signal< void (bool) > TriggeredSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SSignal() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API virtual ~SSignal() noexcept;

protected:

    /// Configures the service
    MODULE_UI_BASE_API void configuring() override;

    /// Register the action and check if the action is executable.
    MODULE_UI_BASE_API virtual void starting() override;

    /// Unregister the action.
    MODULE_UI_BASE_API virtual void stopping() override;

    /// Emit the signal
    MODULE_UI_BASE_API void updating() override;

    /// Give information about the class. Do nothing.
    MODULE_UI_BASE_API virtual void info(std::ostream& _sstream ) override;

    /// Signal trigerred when action has been triggered
    SPTR(TriggeredSignalType) m_sigTriggered;
    /// Signal trigerred when action has been cancelled
    SPTR(TriggeredSignalType) m_sigCancelled;
};

} // namespace action
} // namespace sight::modules::ui::base
