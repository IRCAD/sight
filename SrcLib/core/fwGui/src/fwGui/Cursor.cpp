/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/Cursor.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

Cursor::Cursor()
{
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ICursor::REGISTRY_KEY);
    m_implementation = ::fwGui::ICursor::dynamicCast(guiObj);
}

//-----------------------------------------------------------------------------

Cursor::~Cursor()
{}

//-----------------------------------------------------------------------------

void Cursor::setCursor( ::fwGui::ICursor::CursorType cursor)
{
    if(m_implementation)
    {
        m_implementation->setCursor(cursor);
    }
}

//-----------------------------------------------------------------------------

void Cursor::setDefaultCursor()
{
    if(m_implementation)
    {
        m_implementation->setDefaultCursor();
    }
}

//-----------------------------------------------------------------------------

} // namespace fwGui



