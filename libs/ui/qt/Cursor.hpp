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

#include <ui/base/ICursor.hpp>

namespace sight::ui::qt
{

/**
 * @brief   Defines the generic cursor.
 */
class UI_QT_CLASS_API Cursor : public ui::base::ICursor
{
public:

    SIGHT_DECLARE_CLASS(Cursor, ui::base::ICursor, ui::base::factory::New<Cursor>);

    UI_QT_API Cursor(ui::base::GuiBaseObject::Key key);

    UI_QT_API ~Cursor() override;

    /// Set the cursor
    UI_QT_API void setCursor(ui::base::ICursor::CursorType cursor, bool setOverridenAsDefault = true) override;

    /// Set the default cursor
    UI_QT_API void setDefaultCursor() override;
};

} // namespace sight::ui::qt
