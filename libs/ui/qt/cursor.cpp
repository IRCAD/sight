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

void cursor::setCursor(ui::cursor_base::state_t cursor, bool setOverridenAsDefault)
{
    QCursor qtCursor(Qt::ArrowCursor);
    switch(cursor)
    {
        case cursor_base::BUSY:
            qtCursor.setShape(Qt::BusyCursor);
            break;

        case cursor_base::WAIT:
            qtCursor.setShape(Qt::WaitCursor);
            break;

        case cursor_base::CROSS:
            qtCursor.setShape(Qt::CrossCursor);
            break;

        case cursor_base::OPEN_HAND:
            qtCursor.setShape(Qt::OpenHandCursor);
            break;

        case cursor_base::CLOSED_HAND:
            qtCursor.setShape(Qt::ClosedHandCursor);
            break;

        case cursor_base::DEFAULT:
            qtCursor.setShape(Qt::BusyCursor);
            break;
    }

    if(setOverridenAsDefault)
    {
        qApp->setOverrideCursor(qtCursor);
    }
    else
    {
        qApp->changeOverrideCursor(qtCursor);
    }
}

//-----------------------------------------------------------------------------

void cursor::setDefaultCursor()
{
    qApp->restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt
