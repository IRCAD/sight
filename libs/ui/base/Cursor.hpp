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

#include "ui/base/config.hpp"
#include "ui/base/ICursor.hpp"

#include <core/base.hpp>

namespace sight::ui::base
{

/**
 * @brief   Defines the generic cursor for IHM.
 * Use the Delegate design pattern
 */
class UI_BASE_CLASS_API Cursor : public ui::base::ICursor
{
public:

    SIGHT_DECLARE_CLASS(Cursor, ui::base::ICursor, new Cursor);

    /// Constructor. Create the implementation of the specific cursor
    UI_BASE_API Cursor();
    /// Destructor. Do nothing
    UI_BASE_API virtual ~Cursor();

    // TODO : update API to reflect the cursor stack
    /// Set the cursor
    UI_BASE_API void setCursor(ui::base::ICursor::CursorType cursor) override;

    /// Set the default cursor
    UI_BASE_API void setDefaultCursor() override;

protected:

    /// Implementation of a cursor in a specific IHM (wx/Qt)
    ui::base::ICursor::sptr m_implementation;
};

/// Convenience "busy" cursor  that will reset to default once deleted
/// It can be useful in case of exception
class UI_BASE_CLASS_API BusyCursor : public Cursor
{
public:

    SIGHT_DECLARE_CLASS(BusyCursor, Cursor, new BusyCursor);

    /// Constructor. Sets cursor to "busy" state
    UI_BASE_API BusyCursor() :
        Cursor()
    {
        setCursor(CursorType::BUSY);
    }

    /// Destructor. Reset cursor to default
    UI_BASE_API ~BusyCursor() override
    {
        setDefaultCursor();
    }
};

class UI_BASE_CLASS_API WaitCursor : public Cursor
{
public:

    SIGHT_DECLARE_CLASS(WaitCursor, Cursor, new WaitCursor);

    /// Constructor. Sets cursor to "wait" state
    UI_BASE_API WaitCursor() :
        Cursor()
    {
        setCursor(CursorType::WAIT);
    }

    /// Destructor. Reset cursor to default
    UI_BASE_API ~WaitCursor() override
    {
        setDefaultCursor();
    }
};

class UI_BASE_CLASS_API CrossCursor : public Cursor
{
public:

    SIGHT_DECLARE_CLASS(CrossCursor, Cursor, new CrossCursor);

    /// Constructor. Sets cursor to "cross" state
    UI_BASE_API CrossCursor() :
        Cursor()
    {
        setCursor(CursorType::CROSS);
    }

    /// Destructor. Reset cursor to default
    UI_BASE_API ~CrossCursor() override
    {
        setDefaultCursor();
    }
};

} // namespace sight::ui::base
