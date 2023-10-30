/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/qt/cursor.hpp"

#include <ui/__/macros.hpp>

#include <QApplication>

SIGHT_REGISTER_GUI(sight::ui::qt::cursor, sight::ui::cursor_base::REGISTRY_KEY);

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

void cursor::set_cursor(ui::cursor_base::state_t _cursor, bool _set_overriden_as_default)
{
    QCursor qt_cursor(Qt::ArrowCursor);
    switch(_cursor)
    {
        case cursor_base::busy:
            qt_cursor.setShape(Qt::BusyCursor);
            break;

        case cursor_base::wait:
            qt_cursor.setShape(Qt::WaitCursor);
            break;

        case cursor_base::cross:
            qt_cursor.setShape(Qt::CrossCursor);
            break;

        case cursor_base::open_hand:
            qt_cursor.setShape(Qt::OpenHandCursor);
            break;

        case cursor_base::closed_hand:
            qt_cursor.setShape(Qt::ClosedHandCursor);
            break;

        case cursor_base::DEFAULT:
            qt_cursor.setShape(Qt::BusyCursor);
            break;
    }

    if(_set_overriden_as_default)
    {
        qApp->setOverrideCursor(qt_cursor);
    }
    else
    {
        qApp->changeOverrideCursor(qt_cursor);
    }
}

//-----------------------------------------------------------------------------

void cursor::set_default_cursor()
{
    qApp->restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt
