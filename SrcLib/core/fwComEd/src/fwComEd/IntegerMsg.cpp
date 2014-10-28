/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/IntegerMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::IntegerMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string IntegerMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

IntegerMsg::IntegerMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

IntegerMsg::~IntegerMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

