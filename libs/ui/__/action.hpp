/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include <data/boolean.hpp>

#include <service/base.hpp>

namespace sight::ui
{

namespace detail::registry
{

class action;

}

/**
 * @brief   Defines the service interface managing menu or toolbar items. It can be used with properties or only with
 * signals.
 *
 * @section Signals Signals
 * - \b is_checked(bool): Emitted when the action is checked or unchecked.
 * - \b checked(): Emitted when the action is checked.
 * - \b unchecked(): Emitted when the action is unchecked.
 * - \b is_enabled(bool): Emitted when the action is enabled or disabled.
 * - \b enabled(): Emitted when the action is enabled.
 * - \b disabled(): Emitted when the action is disabled.
 * - \b is_visible(bool): Emitted when the action is visible or invisible
 *
 * @section Slots Slots
 * - \b set_checked(bool): sets whether the action is checked.
 * - \b check(): check the action.
 * - \b uncheck(): uncheck the action.
 * - \b set_visible(bool): sets whether the action is visible in its container.
 * - \b set_hidden(bool): sets whether the action is hidden in its container.
 * - \b show(): make the action visible.
 * - \b hide(): make the action invisible.
 * - \b toggle_visibility(): make the action visible if it was invisible or invisible if it was visible
 * - \b set_enabled(bool): sets whether the action can be interacted with.
 * - \b set_disabled(bool): opposite of setEnabled(bool).
 * - \b enable(): make the action interactive.
 * - \b disable(): make the action not interactive.
 *
 * Example of configuration using properties
 * @code{.xml}
    <service uid="item" type="sight::module::ui::action" >
        <properties checked="false" enabled="false" inverse="true" visible="true" />
        <confirmation message="..." />
    </service>
   @endcode
 *
 * Example of configuration using signals
 * @code{.xml}
    <service uid="item" type="sight::module::ui::action" >
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
class SIGHT_UI_CLASS_API action : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(action, service::base);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    struct signals
    {
        using bool_t = core::com::signal<void (bool)>;
        using void_t = core::com::signal<void ()>;

        static inline const core::com::signals::key_t IS_ENABLED = "is_enabled";
        static inline const core::com::signals::key_t ENABLED    = "enabled";
        static inline const core::com::signals::key_t DISABLED   = "disabled";
        static inline const core::com::signals::key_t IS_CHECKED = "is_checked";
        static inline const core::com::signals::key_t CHECKED    = "checked";
        static inline const core::com::signals::key_t UNCHECKED  = "unchecked";
        static inline const core::com::signals::key_t IS_VISIBLE = "is_visible";
    };

    struct slots
    {
        static inline const core::com::slots::key_t SET_CHECKED       = "set_checked";
        static inline const core::com::slots::key_t CHECK             = "check";
        static inline const core::com::slots::key_t UNCHECK           = "uncheck";
        static inline const core::com::slots::key_t APPLY_CHECKED     = "apply_checked";
        static inline const core::com::slots::key_t SET_VISIBLE       = "set_visible";
        static inline const core::com::slots::key_t SET_HIDDEN        = "set_hidden";
        static inline const core::com::slots::key_t SHOW              = "show";
        static inline const core::com::slots::key_t HIDE              = "hide";
        static inline const core::com::slots::key_t TOGGLE_VISIBILITY = "toggle_visibility";
        static inline const core::com::slots::key_t APPLY_VISIBLE     = "apply_visible";
        static inline const core::com::slots::key_t SET_ENABLED       = "set_enabled";
        static inline const core::com::slots::key_t SET_DISABLED      = "set_disabled";
        static inline const core::com::slots::key_t ENABLE            = "enable";
        static inline const core::com::slots::key_t DISABLE           = "disable";
        static inline const core::com::slots::key_t APPLY_ENABLED     = "apply_enabled";
    };

    /// Method called when the action service is stopping
    SIGHT_UI_API void action_service_stopping();

    /// Method called when the action service is starting
    SIGHT_UI_API void action_service_starting();

    /// Checks or unchecks the action service.
    SIGHT_UI_API virtual void set_checked(bool _checked);

    /// Sets the action service executable or not.
    [[nodiscard]] SIGHT_UI_API bool checked() const;

    /// Enables or disables the action service.
    SIGHT_UI_API void set_enabled(bool _enabled);

    /// Sets the action service executable or not.
    [[nodiscard]] SIGHT_UI_API bool enabled() const;

    /// Shows or hides the action.
    SIGHT_UI_API void set_visible(bool _is_visible);

    /// Shows the action.
    SIGHT_UI_API void show();

    /// Hides the action.
    SIGHT_UI_API void hide();

    /// Returns true if action is visible
    [[nodiscard]] SIGHT_UI_API bool visible() const;

    /// Returns true if the active state is inverted.
    [[nodiscard]] SIGHT_UI_API bool inverted() const;

    /**
     * @brief Confirms that the action must be executed.
     *
     * If action is configured to be confirmed : show a dialog box to confirm execution.
     * Else return true
     *
     * @return true if user click on 'true' button.
     */
    SIGHT_UI_API bool confirm_action();

protected:

    SIGHT_UI_API action();
    SIGHT_UI_API ~action() override = default;

    /// Initializes the action. This should be called in the configuring() method in derived classes.
    SIGHT_UI_API void initialize();

    /// Connects the properties
    SIGHT_UI_API service::connections_t auto_connections() const override;

private:

    SPTR(ui::detail::registry::action) m_registry;

    sight::data::property<sight::data::boolean> m_checked {this, "checked", false};
    sight::data::property<sight::data::boolean> m_enabled {this, "enabled", true};
    sight::data::property<sight::data::boolean> m_visible {this, "visible", true};
    sight::data::property<sight::data::boolean> m_inverse {this, "inverse", false};

    std::optional<bool> m_prev_checked;
    std::optional<bool> m_prev_enabled;
    std::optional<bool> m_prev_visible;

    bool m_confirm_action {false};
    bool m_default_button {false};
    std::string m_confirm_message;
};

//-----------------------------------------------------------------------------

class lock_action
{
public:

    lock_action(action::wptr _action) :
        m_action(_action)
    {
        m_action.lock()->set_enabled(false);
    }

    ~lock_action()
    {
        m_action.lock()->set_enabled(true);
    }

private:

    action::wptr m_action;
};

//-----------------------------------------------------------------------------

} // namespace sight::ui
