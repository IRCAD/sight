/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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



