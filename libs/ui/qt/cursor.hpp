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

#include "ui/qt/config.hpp"

#include <core/base.hpp>

#include <ui/__/cursor_base.hpp>

namespace sight::ui::qt
{

/**
 * @brief   Defines the generic cursor.
 */
class UI_QT_CLASS_API cursor : public ui::cursor_base
{
public:

    SIGHT_DECLARE_CLASS(cursor, ui::cursor_base, ui::factory::make<cursor>);

    UI_QT_API ~cursor() override = default;

    /// Set the cursor
    UI_QT_API void setCursor(ui::cursor_base::state_t cursor, bool setOverridenAsDefault = true) override;

    /// Set the default cursor
    UI_QT_API void setDefaultCursor() override;
};

} // namespace sight::ui::qt
