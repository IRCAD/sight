/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#ifndef __FWGUI_IACTIONSRV_HPP__
#define __FWGUI_IACTIONSRV_HPP__

#include "fwGui/config.hpp"
#include "fwGui/registrar/ActionRegistrar.hpp"

#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IService.hpp>

namespace fwGui
{

/**
 * @brief   Defines the service interface managing the menu items.
 */
class FWGUI_CLASS_API IActionSrv : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IActionSrv)(::fwServices::IService) );
    fwCoreAllowSharedFromThis();

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when action is enabled
    typedef ::fwCom::Signal< void ()> EnabledSignalType;
    static const ::fwCom::Signals::SignalKeyType s_ENABLED_SIG;

    /// Signal emitted when action is disabled
    typedef ::fwCom::Signal< void ()> DisabledSignalType;
    static const ::fwCom::Signals::SignalKeyType s_DISABLED_SIG;

    /// Signal emitted when action is checked
    typedef ::fwCom::Signal< void ()> CheckedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_CHECKED_SIG;

    /// Signal emitted when action is unchecked
    typedef ::fwCom::Signal< void ()> UncheckedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_UNCHECKED_SIG;
    /**
     * @}
     */

    /**
     * @name Slots Keys
     * @{
     */
    /// Slot to show/hide the action
    static const ::fwCom::Slots::SlotKeyType s_SET_VISIBLE_SLOT;

    /// Slot to disable the action
    static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT;

    /// Slot to disable the action
    static const ::fwCom::Slots::SlotKeyType s_HIDE_SLOT;

    /// Slot to activate/deactivate the action
    static const ::fwCom::Slots::SlotKeyType s_SET_IS_ACTIVE_SLOT;

    /// Slot to activate the action
    static const ::fwCom::Slots::SlotKeyType s_ACTIVATE_SLOT;

    /// Slot to desactivate the action
    static const ::fwCom::Slots::SlotKeyType s_DEACTIVATE_SLOT;

    /// Slot to enable/disable the action
    static const ::fwCom::Slots::SlotKeyType s_SET_IS_EXECUTABLE_SLOT;

    /// Slot to enable the action
    static const ::fwCom::Slots::SlotKeyType s_SET_EXECUTABLE_SLOT;

    /// Slot to disable the action
    static const ::fwCom::Slots::SlotKeyType s_SET_INEXECUTABLE_SLOT;
    /**
     * @}
     */

    /// Method called when the action service is stopping
    FWGUI_API void actionServiceStopping();

    /// Method called when the action service is starting
    FWGUI_API void actionServiceStarting();

    /// Set the action service is active/inactive.
    FWGUI_API virtual void setIsActive(bool isActive);

    /// Set the action service is active.
    FWGUI_API virtual void activate();

    /// Set the action service is not active.
    FWGUI_API virtual void deactivate();

    /// Return true if action service is active.
    FWGUI_API bool getIsActive() const;

    /// Return true if action 'check' feedback is inverted.
    bool getActiveStateValue() const
    {
        return m_activeStateValue;
    }

    /// Set the action service executable or not.
    FWGUI_API virtual void setIsExecutable(bool isExecutable);

    /// Set the action service executable.
    FWGUI_API virtual void setExecutable();

    /// Set the action service not executable.
    FWGUI_API virtual void setInexecutable();

    /// Return action service is executable.
    FWGUI_API bool getIsExecutable() const;

    /// Show or hide the action.
    FWGUI_API void setVisible(bool isVisible);

    /// Show the action.
    FWGUI_API void show();

    /// Hide the action.
    FWGUI_API void hide();

    /// Returns true if action is visible
    FWGUI_API bool isVisible() const;

    /**
     * @brief Confirm that the action must be executed.
     *
     * If action is configured to be confirmed : show a dialog box to confirm execution.
     * Else return true
     *
     * @return true if user click on 'yes' button.
     */
    FWGUI_API bool confirmAction();

protected:

    FWGUI_API IActionSrv();

    FWGUI_API virtual ~IActionSrv();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief Initialize the action.
     *
     * Example of configuration
     * @code{.xml}
           <service uid="item" type="::gui::action::SDefault" autoConnect="no" >
              <state active="false" executable="false" inverse="true" visible="true" />
              <confirmation value="yes" message="..." />
           </service>
       @endcode
     * - \<state active="false" executable="false" /\> : fix the state of the action in the menu and Toolbar.
     *   - \b executable not mandatory (Default value true ) : allows to enable/disable the execution of the action.
     *     If the action appears in the interface it will be enabled/disabled.
     *   - \b active not mandatory (Default value false ):
     *     If the action appears in the interface it will be checked/unchecked.
     *   - \b visible not mandatory (Default value true ):
     *     If true, the action is visible in the interface (and if the action is associated to a menu and/or a toolbar).
     *   - \b inverse not mandatory (Default value true) : allow to invert the state of the action (if "check")
     * - \<confirmation value="yes" message="..." /\> : configure if the action must be confirmed
     *                                                  by the user before executing it.
     *   - \b value : if true the action will show a confirmation message before executing it.
     *   - \b message not mandatory : if not empty the message is shown in dialog box.
     *   - \b defaultbutton not mandatory (default defined by underlying gui backend) :
     *                      specify the dialog default button (yes or no)
     */
    FWGUI_API void initialize();

private:

    ::fwGui::registrar::ActionRegistrar::sptr m_registrar;

    /// Value for which the action will be considered 'active'
    bool m_activeStateValue;
    bool m_isActive;
    bool m_isExecutable;
    bool m_isVisible;
    bool m_confirmAction;
    std::string m_confirmMessage;
    std::string m_defaultButton;

    /// Signal emitted when action is enabled
    EnabledSignalType::sptr m_sigEnabled;
    /// Signal emitted when action is disabled
    DisabledSignalType::sptr m_sigDisabled;
    /// Signal emitted when action is checked
    CheckedSignalType::sptr m_sigChecked;
    /// Signal emitted when action is unchecked
    UncheckedSignalType::sptr m_sigUnchecked;
};

//-----------------------------------------------------------------------------

class LockAction
{
public:
    LockAction(IActionSrv::wptr action) :
        m_action(action)
    {
        m_action.lock()->setIsExecutable(false);
    }
    ~LockAction()
    {
        m_action.lock()->setIsExecutable(true);
    }
private:
    IActionSrv::wptr m_action;
};

//-----------------------------------------------------------------------------

} // namespace fwGui

#endif /*__FWGUI_IACTIONSRV_HPP__*/

