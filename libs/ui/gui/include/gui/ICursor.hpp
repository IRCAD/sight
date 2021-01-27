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
 * @brief   Defines the generic cursor for IHM.
 */
class GUI_CLASS_API ICursor : public gui::GuiBaseObject
{
public:
    fwCoreClassMacro(ICursor, gui::GuiBaseObject)

    /// Cursor type
    typedef enum
    {
        DEFAULT,
        BUSY,
        WAIT,
        CROSS
    } CursorType;

    /// Constructor. Do nothing.
    GUI_API ICursor();
    /// Destructor. Do nothing.
    GUI_API virtual ~ICursor();

    typedef std::string FactoryRegistryKeyType;
    GUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the cursor
    GUI_API virtual void setCursor( CursorType cursor) = 0;

    /// Set the default cursor
    GUI_API virtual void setDefaultCursor() = 0;

};

} // namespace sight::gui
