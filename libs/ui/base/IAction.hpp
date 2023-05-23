/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/base/config.hpp"
#include "ui/base/registry/Action.hpp"

#include <service/IService.hpp>

namespace sight::ui::base
{

/**
 * @brief   Defines the service interface managing menu or toolbar items.
 *
 * @section Signals Signals
 * - \b isChecked(bool): Emitted when the action is checked or unchecked.
 * - \b checked(): Emitted when the action is checked.
 * - \b unchecked(): Emitted when the action is unchecked.
 * - \b isEnabled(bool): Emitted when the action is enabled or disabled.
 * - \b enabled(): Emitted when the action is enabled.
 * - \b disabled(): Emitted when the action is disabled.
 *
 * @section Slots Slots
 * - \b setChecked(bool): sets whether the action is checked.
 * - \b check(): check the action.
 * - \b uncheck(): uncheck the action.
 * - \b setVisible(bool): sets whether the action is visible in its container.
 * - \b show(): make the action visible.
 * - \b hide(): make the action invisible.
 * - \b toggleVisibility(): make the action visible if it was invisible or invisible if it was visible
 * - \b setEnabled(bool): sets whether the action can be interacted with.
 * - \b setDisabled(bool): opposite of setEnabled(bool).
 * - \b enable(): make the action interactive.
 * - \b disable(): make the action not interactive.
 * - \b setIsActive(bool): deprecated, synonym of setChecked().
 * - \b activate(): deprecated, synonym of check().
 * - \b deactivate(): deprecated, synonym of uncheck().
 * - \b setIsExecutable(bool): deprecated, synonym of setEnabled()
 * - \b setExecutable(): deprecated, synonym of enable().
 * - \b setInexecutable(): deprecated, synonym of disable().
 *
 * Example of configuration
 * @code{.xml}
    <service uid="item" type="sight::module::ui::base::SDefaultAction" autoConnect="false" >
        <state checked="false" enabled="false" inverse="true" visible="true" />
        <confirmation message="..." />
    </service>
   @endcode
 *
 * All configurations options are optional.
 * - \<state checked="false" enabled="false" /\> : fix the state of the action in the menu and Toolbar.
 *   - \b enabled not mandatory (Default value true ) : allows to enable/disable the execution of the action.
 *     If the action appears in the interface it will be enabled/disabled.
 *   - \b checked not mandatory (Default value false ):
 *     If the action appears in the interface it will be checked/unchecked.
 *   - \b visible not mandatory (Default value true ):
 *     If true, the action is visible in the interface (and if the action is associated to a menu and/or a toolbar).
 *   - \b inverse not mandatory (Default value true) : allow to invert the state of the action (if "check")
 * - \b sync: set to true to emit the 'checked' signals synchronously instead of the default, asynchronously.
 * - \<confirmation message="..." /\> : configure if the action must be confirmed by the user before executing it.
 *   - \b message not mandatory : if not empty the message is shown in dialog box.
 *   - \b defaultButton (optional) (default defined by the gui backend): dialog default button (true or false)
 */
class UI_BASE_CLASS_API IAction : public sight::service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IAction, service::IService);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @name Signals
     * @{
     */
    typedef core::com::Signal<void (bool)> bool_signal_t;
    typedef core::com::Signal<void ()> void_signal_t;

    /// Signal emitted when action is checked/unchecked
    static const core::com::Signals::SignalKeyType s_IS_ENABLED_SIG;

    /// Signal emitted when action is enabled
    static const core::com::Signals::SignalKeyType s_ENABLED_SIG;

    /// Signal emitted when action is disabled
    static const core::com::Signals::SignalKeyType s_DISABLED_SIG;

    /// Signal emitted when action is checked/unchecked
    static const core::com::Signals::SignalKeyType s_IS_CHECKED_SIG;

    /// Signal emitted when action is checked
    static const core::com::Signals::SignalKeyType s_CHECKED_SIG;

    /// Signal emitted when action is unchecked
    static const core::com::Signals::SignalKeyType s_UNCHECKED_SIG;
    /**
     * @}
     */

    /**
     * @name Slots Keys
     * @{
     */

    /// Slot to show/hide the action
    static const core::com::Slots::SlotKeyType s_SET_VISIBLE_SLOT;

    /// Slot to disable the action
    static const core::com::Slots::SlotKeyType s_SHOW_SLOT;

    /// Slot to disable the action
    static const core::com::Slots::SlotKeyType s_HIDE_SLOT;

    /// Slot to make the action visible when it is invisible and invisible when it is visible
    static const core::com::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;

    /// Deprecated: Slot to check or uncheck the action
    static const core::com::Slots::SlotKeyType s_SET_IS_ACTIVE_SLOT;

    /// Deprecated: Slot to check the action
    static const core::com::Slots::SlotKeyType s_ACTIVATE_SLOT;

    /// Deprecated: Slot to check the action
    static const core::com::Slots::SlotKeyType s_DEACTIVATE_SLOT;

    /// Slot to check or uncheck the action
    static const core::com::Slots::SlotKeyType s_SET_CHECKED_SLOT;

    /// Slot to check the action
    static const core::com::Slots::SlotKeyType s_CHECK_SLOT;

    /// Slot to check the action
    static const core::com::Slots::SlotKeyType s_UNCHECK_SLOT;

    /// Deprecated: Slot to enable/disable the action
    static const core::com::Slots::SlotKeyType s_SET_IS_EXECUTABLE_SLOT;

    /// Deprecated: Slot to enable the action
    static const core::com::Slots::SlotKeyType s_SET_EXECUTABLE_SLOT;

    /// Deprecated: Slot to disable the action
    static const core::com::Slots::SlotKeyType s_SET_INEXECUTABLE_SLOT;

    /// Slot to enable or disable the action
    static const core::com::Slots::SlotKeyType s_SET_ENABLED_SLOT;

    /// Slot to enable or disable the action
    static const core::com::Slots::SlotKeyType s_SET_DISABLED_SLOT;

    /// Slot to enable the action
    static const core::com::Slots::SlotKeyType s_ENABLE_SLOT;

    /// Slot to disable the action
    static const core::com::Slots::SlotKeyType s_DISABLE_SLOT;
    /**
     * @}
     */

    /// Method called when the action service is stopping
    UI_BASE_API void actionServiceStopping();

    /// Method called when the action service is starting
    UI_BASE_API void actionServiceStarting();

    /// Checks or unchecks the action service.
    UI_BASE_API virtual void setChecked(bool checked);

    /// Sets the action service executable or not.
    [[nodiscard]] UI_BASE_API bool checked() const;

    /// Sets the action service is active/inactive.
    [[deprecated("Removed in sight 23.0.")]] UI_BASE_API virtual void setIsActive(bool isActive);

    /// Returns true if action service is active.
    [[deprecated("Removed in sight 23.0.")]] UI_BASE_API bool getIsActive() const;

    /// Enables or disables the action service.
    UI_BASE_API void setEnabled(bool enabled);

    /// Sets the action service executable or not.
    [[nodiscard]] UI_BASE_API bool enabled() const;

    /// Sets the action service executable or not.
    [[deprecated("Removed in sight 23.0.")]] UI_BASE_API void setIsExecutable(bool isExecutable);

    /// Returns action service is executable.
    [[deprecated("Removed in sight 23.0.")]] UI_BASE_API bool getIsExecutable() const;

    /// Shows or hides the action.
    UI_BASE_API void setVisible(bool isVisible);

    /// Shows the action.
    UI_BASE_API void show();

    /// Hides the action.
    UI_BASE_API void hide();

    /// Returns true if action is visible
    [[deprecated("Removed in sight 23.0.")]] UI_BASE_API bool isVisible() const;

    /// Returns true if action is visible
    [[nodiscard]] UI_BASE_API bool visible() const;

    /// Returns true if the active state is inverted.
    [[deprecated("Removed in sight 23.0.")]] UI_BASE_API bool isInverted() const;

    /// Returns true if the active state is inverted.
    [[nodiscard]] UI_BASE_API bool inverted() const;

    /**
     * @brief Confirms that the action must be executed.
     *
     * If action is configured to be confirmed : show a dialog box to confirm execution.
     * Else return true
     *
     * @return true if user click on 'true' button.
     */
    UI_BASE_API bool confirmAction();

protected:

    UI_BASE_API IAction();
    UI_BASE_API ~IAction() override;

    /// Initializes the action. This should be called in the configuring() method in derived classes.
    UI_BASE_API void initialize();

private:

    ui::base::registry::Action::sptr m_registry;

    /// Handles the information of the action state inversion.
    bool m_inverted {false};
    bool m_checked {false};
    bool m_enabled {true};
    bool m_visible {true};
    bool m_confirmAction {false};
    bool m_defaultButton {false};
    std::string m_confirmMessage;
};

//-----------------------------------------------------------------------------

class LockAction
{
public:

    LockAction(IAction::wptr action) :
        m_action(action)
    {
        m_action.lock()->setEnabled(false);
    }

    ~LockAction()
    {
        m_action.lock()->setEnabled(true);
    }

private:

    IAction::wptr m_action;
};

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
