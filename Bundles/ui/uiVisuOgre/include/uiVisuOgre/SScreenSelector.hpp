/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "uiVisuOgre/config.hpp"

#include <fwCom/Signals.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiVisuOgre
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
    <service uid="..." type="::uiVisuOgre::SScreenSelector" >
        <config mode="select" />
    </service>
 *
 * @subsection Configuration Configuration
 * - \b mode (optional, values=select|current|neighbor): screen selection method:
 *   - \b select: pop a dialog to let the user choose between all available screens (if there are more than one).
 *   - \b current: select the screen on which the application is currently displayed.
 *   - \b neighbor: select a neighboring screen if there is one.
 */
class UIVISUOGRE_CLASS_API SScreenSelector : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceMacro(SScreenSelector, ::fwGui::IActionSrv)

    /// Constructor.
    UIVISUOGRE_API SScreenSelector();

    /// Destructor.
    UIVISUOGRE_API virtual ~SScreenSelector() final;

protected:

    /// Configures the selection mode.
    UIVISUOGRE_API virtual void configuring() final;

    /// Starts the action service.
    UIVISUOGRE_API virtual void starting() final;

    /// Selects a screen based on the selection mode.
    UIVISUOGRE_API virtual void updating() final;

    /// Stops the action service.
    UIVISUOGRE_API virtual void stopping() final;

private:

    /// Type of signal sent when a screen is selected.
    using ScreenSelectedSignalType = ::fwCom::Signal<void (int)>;

    /// Prompts a dialog letting the user select a screen if there are multiple monitors.
    int selectScreen() const;

    /// Signal sent when a screen is selected.
    ScreenSelectedSignalType::sptr m_screenSelectedSig;

    /// Selection mode can be 'select', 'current' or 'neighbor' (cf XML configuration).
    std::string m_mode { "select" };

};

} // namespace uiVisuOgre
