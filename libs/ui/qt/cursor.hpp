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

#include <sight/ui/qt/config.hpp>

#include <core/base.hpp>

#include <ui/__/cursor_base.hpp>

namespace sight::ui::qt
{

/**
 * @brief   Defines the generic cursor.
 */
class SIGHT_UI_QT_CLASS_API cursor : public ui::cursor_base
{
public:

    SIGHT_DECLARE_CLASS(cursor, ui::cursor_base, ui::factory::make<cursor>);

    SIGHT_UI_QT_API ~cursor() override = default;

    /// Set the cursor
    SIGHT_UI_QT_API void set_cursor(ui::cursor_base::state_t _cursor, bool _set_overriden_as_default = true) override;

    /// Set the default cursor
    SIGHT_UI_QT_API void set_default_cursor() override;
};

} // namespace sight::ui::qt
