/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/LocationMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::LocationMsg, ::fwComEd::LocationMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string LocationMsg::LOCATION_IS_MODIFIED = "LOCATION_IS_MODIFIED";

//-----------------------------------------------------------------------------

LocationMsg::LocationMsg() throw()
{}

//-----------------------------------------------------------------------------

LocationMsg::~LocationMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

