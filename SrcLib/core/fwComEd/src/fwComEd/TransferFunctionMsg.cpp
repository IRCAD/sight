/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/TransferFunctionMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::TransferFunctionMsg );

namespace fwComEd
{

std::string TransferFunctionMsg::MODIFIED_POINTS = "MODIFIED_POINTS";
std::string TransferFunctionMsg::WINDOWING       = "WINDOWING";

//-----------------------------------------------------------------------------

TransferFunctionMsg::TransferFunctionMsg(::fwServices::ObjectMsg::Key key)
{
}

//-----------------------------------------------------------------------------

TransferFunctionMsg::~TransferFunctionMsg() throw()
{
}

//-----------------------------------------------------------------------------

void TransferFunctionMsg::setWindowLevel( double window, double level )
{
    this->addEvent( ::fwComEd::TransferFunctionMsg::WINDOWING );
    m_level  = level;
    m_window = window;
}

//-----------------------------------------------------------------------------

double TransferFunctionMsg::getWindow() const
{
    SLM_ASSERT( "WINDOWING Event not found in msg", this->hasEvent( ::fwComEd::TransferFunctionMsg::WINDOWING ));
    return(m_window);
}

//-----------------------------------------------------------------------------

double TransferFunctionMsg::getLevel() const
{
    SLM_ASSERT( "WINDOWING Event not found in msg", this->hasEvent( ::fwComEd::TransferFunctionMsg::WINDOWING ));
    return (m_level);
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

