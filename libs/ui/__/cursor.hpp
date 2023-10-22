/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/cursor_base.hpp"

#include <core/base.hpp>

namespace sight::ui
{

/**
 * @brief   Defines the generic cursor for IHM.
 * Use the Delegate design pattern
 */
class UI_CLASS_API cursor : public ui::cursor_base
{
public:

    SIGHT_DECLARE_CLASS(cursor, ui::cursor_base);

    /// Constructor. Create the implementation of the specific cursor
    UI_API cursor();
    /// Destructor. Do nothing
    UI_API ~cursor() override = default;

    // TODO : update API to reflect the cursor stack
    /// Set the cursor
    UI_API void setCursor(ui::cursor_base::state_t _cursor, bool _set_overriden_as_default = true) override;

    /// Set the default cursor
    UI_API void setDefaultCursor() override;

protected:

    /// Implementation of a cursor in a specific IHM (wx/Qt)
    ui::cursor_base::sptr m_implementation;
};

/// Convenience "busy" cursor  that will reset to default once deleted
/// It can be useful in case of exception
class UI_CLASS_API BusyCursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(BusyCursor, cursor);

    /// Constructor. Sets cursor to "busy" state
    UI_API BusyCursor()
    {
        setCursor(state_t::BUSY);
    }

    /// Destructor. Reset cursor to default
    UI_API ~BusyCursor() override
    {
        setDefaultCursor();
    }
};

class UI_CLASS_API WaitCursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(WaitCursor, cursor);

    /// Constructor. Sets cursor to "wait" state
    UI_API WaitCursor()
    {
        setCursor(state_t::WAIT);
    }

    /// Destructor. Reset cursor to default
    UI_API ~WaitCursor() override
    {
        setDefaultCursor();
    }
};

class UI_CLASS_API CrossCursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(CrossCursor, cursor);

    /// Constructor. Sets cursor to "cross" state
    UI_API CrossCursor()
    {
        setCursor(state_t::CROSS);
    }

    /// Destructor. Reset cursor to default
    UI_API ~CrossCursor() override
    {
        setDefaultCursor();
    }
};

class UI_CLASS_API OpenHandCursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(OpenHandCursor, cursor);

    /// Constructor. Sets cursor to "cross" state
    UI_API OpenHandCursor()
    {
        setCursor(state_t::OPEN_HAND);
    }

    /// Destructor. Reset cursor to default
    UI_API ~OpenHandCursor() override
    {
        setDefaultCursor();
    }
};

class UI_CLASS_API ClosedHandCursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(ClosedHandCursor, cursor);

    /// Constructor. Sets cursor to "cross" state
    UI_API ClosedHandCursor()
    {
        setCursor(state_t::CLOSED_HAND);
    }

    /// Destructor. Reset cursor to default
    UI_API ~ClosedHandCursor() override
    {
        setDefaultCursor();
    }
};

} // namespace sight::ui
