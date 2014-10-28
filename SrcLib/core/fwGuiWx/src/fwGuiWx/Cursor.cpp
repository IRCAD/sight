/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/utils.h>
#include <wx/cursor.h>

#include <fwGui/registry/macros.hpp>

#include <fwGuiWx/convert.hpp>

#include "fwGuiWx/Cursor.hpp"


fwGuiRegisterMacro( ::fwGuiWx::Cursor, ::fwGui::ICursor::REGISTRY_KEY );

namespace fwGuiWx
{
//-----------------------------------------------------------------------------

Cursor::Cursor(::fwGui::GuiBaseObject::Key key)
{}

//-----------------------------------------------------------------------------

Cursor::~Cursor()
{}

//-----------------------------------------------------------------------------

void Cursor::setCursor( ::fwGui::ICursor::CursorType cursor)
{
    if (cursor == ICursor::DEFAULT)
    {
        this->setDefaultCursor();
    }
    else if (cursor == ICursor::BUSY)
    {
        wxBeginBusyCursor(wxHOURGLASS_CURSOR);
    }
    else if (cursor == ICursor::WAIT)
    {
        wxBeginBusyCursor(new wxCursor(wxCURSOR_WAIT));
    }
    else if (cursor == ICursor::CROSS)
    {
        wxBeginBusyCursor(new wxCursor(wxCURSOR_CROSS));
    }
}

//-----------------------------------------------------------------------------

void Cursor::setDefaultCursor()
{
    wxEndBusyCursor();
}

//-----------------------------------------------------------------------------

} // namespace fwGuiWx



