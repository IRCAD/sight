/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/LocationMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::LocationMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string LocationMsg::LOCATION_IS_MODIFIED = "LOCATION_IS_MODIFIED";

//-----------------------------------------------------------------------------

LocationMsg::LocationMsg(::fwServices::ObjectMsg::Key key)
{
}

//-----------------------------------------------------------------------------

LocationMsg::~LocationMsg() throw()
{
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

