/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/ColorMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::ColorMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ColorMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

ColorMsg::ColorMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

ColorMsg::~ColorMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

