/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/ui/__/config.hpp>

#include "ui/__/cursor_base.hpp"

#include <core/base.hpp>

namespace sight::ui
{

/**
 * @brief   Defines the generic cursor for UI.
 * Use the Delegate design pattern
 */
class SIGHT_UI_CLASS_API cursor : public ui::cursor_base
{
public:

    SIGHT_DECLARE_CLASS(cursor, ui::cursor_base);

    /// Constructor. Create the implementation of the specific cursor
    SIGHT_UI_API cursor();
    /// Destructor. Do nothing
    SIGHT_UI_API ~cursor() override = default;

    // TODO : update API to reflect the cursor stack
    /// Set the cursor
    SIGHT_UI_API void set_cursor(ui::cursor_base::state_t _cursor, bool _set_overriden_as_default = true) override;

    /// Set the default cursor
    SIGHT_UI_API void set_default_cursor() override;

protected:

    /// Implementation of a cursor in a specific UI (wx/Qt)
    ui::cursor_base::sptr m_implementation;
};

/// Convenience "busy" cursor  that will reset to default once deleted
/// It can be useful in case of exception
class SIGHT_UI_CLASS_API busy_cursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(busy_cursor, cursor);

    /// Constructor. Sets cursor to "busy" state
    SIGHT_UI_API busy_cursor()
    {
        set_cursor(state_t::busy);
    }

    /// Destructor. Reset cursor to default
    SIGHT_UI_API ~busy_cursor() override
    {
        set_default_cursor();
    }
};

class SIGHT_UI_CLASS_API wait_cursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(wait_cursor, cursor);

    /// Constructor. Sets cursor to "wait" state
    SIGHT_UI_API wait_cursor()
    {
        set_cursor(state_t::wait);
    }

    /// Destructor. Reset cursor to default
    SIGHT_UI_API ~wait_cursor() override
    {
        set_default_cursor();
    }
};

class SIGHT_UI_CLASS_API cross_cursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(cross_cursor, cursor);

    /// Constructor. Sets cursor to "cross" state
    SIGHT_UI_API cross_cursor()
    {
        set_cursor(state_t::cross);
    }

    /// Destructor. Reset cursor to default
    SIGHT_UI_API ~cross_cursor() override
    {
        set_default_cursor();
    }
};

class SIGHT_UI_CLASS_API open_hand_cursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(open_hand_cursor, cursor);

    /// Constructor. Sets cursor to "cross" state
    SIGHT_UI_API open_hand_cursor()
    {
        set_cursor(state_t::open_hand);
    }

    /// Destructor. Reset cursor to default
    SIGHT_UI_API ~open_hand_cursor() override
    {
        set_default_cursor();
    }
};

class SIGHT_UI_CLASS_API closed_hand_cursor : public cursor
{
public:

    SIGHT_DECLARE_CLASS(closed_hand_cursor, cursor);

    /// Constructor. Sets cursor to "cross" state
    SIGHT_UI_API closed_hand_cursor()
    {
        set_cursor(state_t::closed_hand);
    }

    /// Destructor. Reset cursor to default
    SIGHT_UI_API ~closed_hand_cursor() override
    {
        set_default_cursor();
    }
};

} // namespace sight::ui
