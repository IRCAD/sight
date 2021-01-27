/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "gui/config.hpp"
#include "gui/GuiBaseObject.hpp"

namespace sight::gui
{
namespace registrar
{

/**
 * @brief   Defines the action registrar for IHM.
 */
class GUI_CLASS_API ActionRegistrar : public gui::GuiBaseObject
{

public:

    fwCoreClassMacro(ActionRegistrar, gui::GuiBaseObject)

    //------------------------------------------------------------------------------

    static sptr New(const std::string& sid)
    {
        return std::make_shared<ActionRegistrar>(sid);
    }

    /// Constructor.
    GUI_API ActionRegistrar( const std::string& sid);

    /// Destructor. Do nothing
    GUI_API virtual ~ActionRegistrar();

    /// Method called when the action service is stopping
    GUI_API void actionServiceStopping();

    /// Method called when the action service is starting
    GUI_API void actionServiceStarting();

    /// Method called when the action service is activated or not
    GUI_API void actionServiceSetActive(bool isActive);

    /// Method called when the action service is executable or not
    GUI_API void actionServiceSetExecutable(bool isExecutable);

    /// Method called when the action service is visible or not
    GUI_API void actionServiceSetVisible(bool isVisible);

protected:

    /// Main service ID associate with this ActionRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace sight::gui
