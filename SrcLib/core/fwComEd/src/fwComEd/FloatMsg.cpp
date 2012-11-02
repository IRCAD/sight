/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/FloatMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::FloatMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string FloatMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

FloatMsg::FloatMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

FloatMsg::~FloatMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

