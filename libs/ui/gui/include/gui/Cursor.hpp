/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "gui/config.hpp"
#include "gui/ICursor.hpp"

#include <core/base.hpp>

namespace sight::gui
{

/**
 * @brief   Defines the generic cursor for IHM.
 * Use the Delegate design pattern
 */
class GUI_CLASS_API Cursor : public gui::ICursor
{

public:

    fwCoreClassMacro(Cursor, gui::ICursor, new Cursor)

    /// Constructor. Create the implementation of the specific cursor
    GUI_API Cursor();
    /// Destructor. Do nothing
    GUI_API virtual ~Cursor();

    // TODO : update API to reflect the cursor stack
    /// Set the cursor
    GUI_API virtual void setCursor( gui::ICursor::CursorType cursor) override;

    /// Set the default cursor
    GUI_API virtual void setDefaultCursor() override;

protected:

    /// Implementation of a cursor in a specific IHM (wx/Qt)
    gui::ICursor::sptr m_implementation;
};

} // namespace sight::gui
