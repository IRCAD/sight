/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/StringMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::StringMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string StringMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

StringMsg::StringMsg(::fwServices::ObjectMsg::Key key)
{
}

//-----------------------------------------------------------------------------

StringMsg::~StringMsg() throw()
{
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

