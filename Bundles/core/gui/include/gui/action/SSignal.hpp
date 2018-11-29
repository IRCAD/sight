/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __GUI_ACTION_SSIGNAL_HPP__
#define __GUI_ACTION_SSIGNAL_HPP__

#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace gui
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
        <service type="::gui::action::SSignal" />
   @endcode
 *
 * See also ::fwGui::IActionSrv::initialize for more configuration parameters.
 * @see IActionSrv::initialize
 */

class GUI_CLASS_API SSignal : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SSignal)(::fwGui::IActionSrv) );
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Type of triggered signal
    typedef ::fwCom::Signal< void (bool) > TriggeredSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SSignal() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SSignal() noexcept;

protected:

    /// Configures the service
    GUI_API void configuring() override;

    /// Register the action and check if the action is executable.
    GUI_API virtual void starting() override;

    /// Unregister the action.
    GUI_API virtual void stopping() override;

    /// Emit the signal
    GUI_API void updating() override;

    /// Give information about the class. Do nothing.
    GUI_API virtual void info(std::ostream& _sstream ) override;

    /// Signal trigerred when action has been triggered
    SPTR(TriggeredSignalType) m_sigTriggered;
    /// Signal trigerred when action has been cancelled
    SPTR(TriggeredSignalType) m_sigCancelled;
};

} // namespace action
} // namespace gui

#endif /* __GUI_ACTION_SSIGNAL_HPP__ */

