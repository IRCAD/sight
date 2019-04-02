/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGuiQml/Cursor.hpp"

#include <fwGui/registry/macros.hpp>

#include <QCursor>
#include <QGuiApplication>

fwGuiRegisterMacro( ::fwGuiQml::Cursor, ::fwGui::ICursor::REGISTRY_KEY );

namespace fwGuiQml
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

    QCursor qmlCursor(Qt::ArrowCursor);
    switch (cursor)
    {
        case ICursor::BUSY:
            qmlCursor.setShape(Qt::BusyCursor);
            break;
        case ICursor::WAIT:
            qmlCursor.setShape(Qt::WaitCursor);
            break;
        case ICursor::CROSS:
            qmlCursor.setShape(Qt::CrossCursor);
            break;
        case ICursor::DEFAULT:
            qmlCursor.setShape(Qt::BusyCursor);
            break;
    }

    qGuiApp->setOverrideCursor(qmlCursor);

}

//-----------------------------------------------------------------------------

void Cursor::setDefaultCursor()
{
    qGuiApp->restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

} // namespace fwGuiQml
