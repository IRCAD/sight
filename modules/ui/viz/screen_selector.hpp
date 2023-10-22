/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "modules/ui/viz/config.hpp"

#include <core/com/signals.hpp>

#include <ui/__/action.hpp>

namespace sight::module::ui::viz
{

/**
 * @brief Selects a screen and sends its index.
 *
 * This service is initially meant to choose a screen for fullscreen rendering.
 *
 * @section Signals Signals
 * - \b screenSelected(int): sends the selected screen's index.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::viz::screen_selector" >
        <config mode="select" />
    </service>
 *
 * @subsection Configuration Configuration
 * - \b mode (optional, values=select|current|neighbor): screen selection method:
 *   - \b select: pop a dialog to let the user choose between all available screens (if there are more than one).
 *   - \b current: select the screen on which the application is currently displayed.
 *   - \b neighbor: select a neighboring screen if there is one.
 */
class MODULE_UI_VIZ_CLASS_API screen_selector final : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(screen_selector, sight::ui::action);

    /// Constructor.
    MODULE_UI_VIZ_API screen_selector();

    /// Destructor.
    MODULE_UI_VIZ_API ~screen_selector() final;

protected:

    /// Configures the selection mode.
    MODULE_UI_VIZ_API void configuring() final;

    /// Starts the action service.
    MODULE_UI_VIZ_API void starting() final;

    /// Selects a screen based on the selection mode.
    MODULE_UI_VIZ_API void updating() final;

    /// Stops the action service.
    MODULE_UI_VIZ_API void stopping() final;

private:

    /// Type of signal sent when a screen is selected.
    using screen_selected_signal_t = core::com::signal<void (int)>;

    /// Prompts a dialog letting the user select a screen if there are multiple monitors.
    static int selectScreen();

    /// Signal sent when a screen is selected.
    screen_selected_signal_t::sptr m_screenSelectedSig;

    /// Selection mode can be 'select', 'current' or 'neighbor' (cf XML configuration).
    std::string m_mode {"select"};
};

} // namespace sight::module::ui::viz
