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

#include "ui/__/config.hpp"

#include <service/base.hpp>

namespace sight::ui
{

namespace detail::registry
{

class action;

}

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
 * - \b isVisible(bool): Emitted when the action is visible or invisible
 *
 * @section Slots Slots
 * - \b setChecked(bool): sets whether the action is checked.
 * - \b check(): check the action.
 * - \b uncheck(): uncheck the action.
 * - \b setVisible(bool): sets whether the action is visible in its container.
 * - \b setHidden(bool): sets whether the action is hidden in its container.
 * - \b show(): make the action visible.
 * - \b hide(): make the action invisible.
 * - \b toggleVisibility(): make the action visible if it was invisible or invisible if it was visible
 * - \b setEnabled(bool): sets whether the action can be interacted with.
 * - \b setDisabled(bool): opposite of setEnabled(bool).
 * - \b enable(): make the action interactive.
 * - \b disable(): make the action not interactive.
 *
 * Example of configuration
 * @code{.xml}
    <service uid="item" type="sight::module::ui::default_action" auto_connect="false" >
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
class UI_CLASS_API action : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(action, service::base);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @name Signals
     * @{
     */
    typedef core::com::signal<void (bool)> bool_signal_t;
    typedef core::com::signal<void ()> void_signal_t;

    /// Signal emitted when action is checked/unchecked
    static const core::com::signals::key_t IS_ENABLED_SIG;

    /// Signal emitted when action is enabled
    static const core::com::signals::key_t ENABLED_SIG;

    /// Signal emitted when action is disabled
    static const core::com::signals::key_t DISABLED_SIG;

    /// Signal emitted when action is checked/unchecked
    static const core::com::signals::key_t IS_CHECKED_SIG;

    /// Signal emitted when action is checked
    static const core::com::signals::key_t CHECKED_SIG;

    /// Signal emitted when action is unchecked
    static const core::com::signals::key_t UNCHECKED_SIG;

    /// Signal emitted when action is visible/invisible
    static const core::com::signals::key_t IS_VISIBLE_SIG;
    /**
     * @}
     */

    /**
     * @name Slots Keys
     * @{
     */

    /// Slot to show/hide the action
    /// @{
    static const core::com::slots::key_t SET_VISIBLE_SLOT;
    static const core::com::slots::key_t SET_HIDDEN_SLOT;
    static const core::com::slots::key_t SHOW_SLOT;
    static const core::com::slots::key_t HIDE_SLOT;
    static const core::com::slots::key_t TOGGLE_VISIBILITY_SLOT;
    /// @}

    /// Slot to check or uncheck the action
    static const core::com::slots::key_t SET_CHECKED_SLOT;

    /// Slot to check the action
    static const core::com::slots::key_t CHECK_SLOT;

    /// Slot to check the action
    static const core::com::slots::key_t UNCHECK_SLOT;

    /// Slot to enable or disable the action
    static const core::com::slots::key_t SET_ENABLED_SLOT;

    /// Slot to enable or disable the action
    static const core::com::slots::key_t SET_DISABLED_SLOT;

    /// Slot to enable the action
    static const core::com::slots::key_t ENABLE_SLOT;

    /// Slot to disable the action
    static const core::com::slots::key_t DISABLE_SLOT;
    /**
     * @}
     */

    /// Method called when the action service is stopping
    UI_API void actionServiceStopping();

    /// Method called when the action service is starting
    UI_API void actionServiceStarting();

    /// Checks or unchecks the action service.
    UI_API virtual void setChecked(bool _checked);

    /// Sets the action service executable or not.
    [[nodiscard]] UI_API bool checked() const;

    /// Enables or disables the action service.
    UI_API void setEnabled(bool _enabled);

    /// Sets the action service executable or not.
    [[nodiscard]] UI_API bool enabled() const;

    /// Shows or hides the action.
    UI_API void setVisible(bool _is_visible);

    /// Shows the action.
    UI_API void show();

    /// Hides the action.
    UI_API void hide();

    /// Returns true if action is visible
    [[nodiscard]] UI_API bool visible() const;

    /// Returns true if the active state is inverted.
    [[nodiscard]] UI_API bool inverted() const;

    /**
     * @brief Confirms that the action must be executed.
     *
     * If action is configured to be confirmed : show a dialog box to confirm execution.
     * Else return true
     *
     * @return true if user click on 'true' button.
     */
    UI_API bool confirmAction();

protected:

    UI_API action();
    UI_API ~action() override;

    /// Initializes the action. This should be called in the configuring() method in derived classes.
    UI_API void initialize();

private:

    SPTR(ui::detail::registry::action) m_registry;

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

    LockAction(action::wptr _action) :
        m_action(_action)
    {
        m_action.lock()->setEnabled(false);
    }

    ~LockAction()
    {
        m_action.lock()->setEnabled(true);
    }

private:

    action::wptr m_action;
};

//-----------------------------------------------------------------------------

} // namespace sight::ui
