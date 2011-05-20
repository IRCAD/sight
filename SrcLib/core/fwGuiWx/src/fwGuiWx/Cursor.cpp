/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/utils.h>
#include <wx/cursor.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwWX/convert.hpp>

#include "fwGuiWx/Cursor.hpp"


REGISTER_BINDING( ::fwGui::ICursor, ::fwGuiWx::Cursor, ::fwGui::ICursor::FactoryRegistryKeyType , ::fwGui::ICursor::REGISTRY_KEY );

namespace fwGuiWx
{
//-----------------------------------------------------------------------------

Cursor::Cursor()
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



