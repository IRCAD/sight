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

#include "ui/base/config.hpp"
#include "ui/base/GuiBaseObject.hpp"

namespace sight::ui::base
{

/**
 * @brief   Defines the generic cursor for IHM.
 */
class UI_BASE_CLASS_API ICursor : public ui::base::GuiBaseObject
{
public:

    SIGHT_DECLARE_CLASS(ICursor, ui::base::GuiBaseObject);

    /// Cursor type
    typedef enum
    {
        DEFAULT,
        BUSY,
        WAIT,
        CROSS
    } CursorType;

    /// Constructor. Do nothing.
    UI_BASE_API ICursor();
    /// Destructor. Do nothing.
    UI_BASE_API virtual ~ICursor();

    typedef std::string FactoryRegistryKeyType;
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /// Set the cursor
    UI_BASE_API virtual void setCursor(CursorType cursor) = 0;

    /// Set the default cursor
    UI_BASE_API virtual void setDefaultCursor() = 0;
};

} // namespace sight::ui::base
