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

/**
 * @brief   Defines the application manipulation API.
 */
class GUI_CLASS_API IApplication : public gui::GuiBaseObject
{
public:
    fwCoreClassMacro(IApplication, gui::GuiBaseObject)

    /// Constructor. Do nothing.
    GUI_API IApplication();
    /// Destructor. Do nothing.
    GUI_API virtual ~IApplication();

    /// Tells the application to exit with a returncode
    GUI_API virtual void exit( int returncode ) = 0;

    /// Sets if confirmation is required when exiting application
    void setConfirm(bool confirm)
    {
        m_confirm = confirm;
    }

    typedef std::string FactoryRegistryKeyType;
    GUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

protected:
    /// Asks confirmation before exiting application.
    bool m_confirm;
};

} // namespace sight::gui
