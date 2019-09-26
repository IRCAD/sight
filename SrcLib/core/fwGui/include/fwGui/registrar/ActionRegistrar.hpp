/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGui/config.hpp"
#include "fwGui/GuiBaseObject.hpp"

namespace fwGui
{
namespace registrar
{

/**
 * @brief   Defines the action registrar for IHM.
 */
class FWGUI_CLASS_API ActionRegistrar : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassMacro(ActionRegistrar, ::fwGui::GuiBaseObject);

    //------------------------------------------------------------------------------

    static sptr New(const std::string& sid)
    {
        return std::make_shared<ActionRegistrar>(sid);
    }

    /// Constructor.
    FWGUI_API ActionRegistrar( const std::string& sid);

    /// Destructor. Do nothing
    FWGUI_API virtual ~ActionRegistrar();

    /// Method called when the action service is stopping
    FWGUI_API void actionServiceStopping();

    /// Method called when the action service is starting
    FWGUI_API void actionServiceStarting();

    /// Method called when the action service is activated or not
    FWGUI_API void actionServiceSetActive(bool isActive);

    /// Method called when the action service is executable or not
    FWGUI_API void actionServiceSetExecutable(bool isExecutable);

    /// Method called when the action service is visible or not
    FWGUI_API void actionServiceSetVisible(bool isVisible);

protected:

    /// Main service ID associate with this ActionRegistrar
    std::string m_sid;
};

} // namespace registrar
} // namespace fwGui
