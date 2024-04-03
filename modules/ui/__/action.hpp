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
#include <ui/__/parameter.hpp>

namespace sight::module::ui
{

/**
 * @brief  Default action for a simple toolbar button or menu item.*
 * Common signals and slots options can be found in the interface @see action.
 *
 * @section Signals Signals
 * - \b clicked(): Emitted when the action is clicked.
 * - \b parameter_changed(parameter_t, std::string): Emitted when the action is clicked or setChecked(true|false).
 *
 * Example of configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::action" >
        <state checked="false" enabled="false" inverse="true" visible="true" />
        <sync>true</sync>
        <confirmation message="..." />
        <!-- will send parameterChanged("left", "position") when checked and parameterChanged("center", "position") when
 * unchecked -->
        <parameter key="position" checked="left" unchecked="center" />
    </service>
   @endcode
 *
 * All configurations options are optional. Common action options can be found in @see action.
 * - \b sync: set to true to emit the 'clicked' signals synchronously instead of the default, asynchronously.
 * - \b parameter: set the key and values to send in parameterChanged signal on click and setChecked(true|false).
 *   - \b key: the key used in parameterChanged signal
 *   - \b clicked: the value used in parameterChanged signal on click
 *   - \b checked: the value used in parameterChanged signal on setChecked(true)
 *   - \b unchecked: the value used in parameterChanged signal on setChecked(false)
 */
class action final : public sight::ui::action
{
public:

    struct signals final
    {
        using key_t = core::com::signals::key_t;
        inline static const key_t CLICKED           = "clicked";
        inline static const key_t PARAMETER_CHANGED = "parameter_changed";

        using changed_t = core::com::signal<void (sight::ui::parameter_t, std::string)>;
    };

    SIGHT_DECLARE_SERVICE(action, sight::ui::action);

    action() noexcept        = default;
    ~action() noexcept final = default;

protected:

    void configuring() final;

    void starting() final;

    void stopping() final;

    /// Emits the clicked signal
    void updating() final;

private:

    /// Signals
    const action::void_signal_t::sptr m_clicked_sig {new_signal<action::void_signal_t>(signals::CLICKED)};
    const signals::changed_t::sptr m_parameter_changed_sig {
        new_signal<signals::changed_t>(signals::PARAMETER_CHANGED)
    };

    /// Configuration option to emit the clicked signal synchronously or asynchronously
    bool m_sync {false};

    /// The optional key used in parameterChanged signal
    std::optional<std::string> m_key;

    /// The optional values used in parameterChanged signal on click and setChecked(true|false)
    std::optional<std::string> m_clicked;
    std::optional<std::string> m_checked;
    std::optional<std::string> m_unchecked;
};

} // namespace sight::module::ui
