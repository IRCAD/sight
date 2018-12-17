/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <QApplication>

#include <fwGui/registry/macros.hpp>

#include "fwGuiQt/Cursor.hpp"


fwGuiRegisterMacro( ::fwGuiQt::Cursor, ::fwGui::ICursor::REGISTRY_KEY );

namespace fwGuiQt
{
//-----------------------------------------------------------------------------

Cursor::Cursor(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

Cursor::~Cursor()
{
}

//-----------------------------------------------------------------------------

void Cursor::setCursor( ::fwGui::ICursor::CursorType cursor)
{

    QCursor qtCursor(Qt::ArrowCursor);
    switch (cursor)
    {
        case ICursor::BUSY:
            qtCursor.setShape(Qt::BusyCursor);
            break;
        case ICursor::WAIT:
            qtCursor.setShape(Qt::WaitCursor);
            break;
        case ICursor::CROSS:
            qtCursor.setShape(Qt::CrossCursor);
            break;
        case ICursor::DEFAULT:
            qtCursor.setShape(Qt::BusyCursor);
            break;
    }

    qApp->setOverrideCursor(qtCursor);

}

//-----------------------------------------------------------------------------

void Cursor::setDefaultCursor()
{
    qApp->restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

} // namespace fwGuiQt



