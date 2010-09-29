/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/ColorMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::ColorMsg, ::fwComEd::ColorMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ColorMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

ColorMsg::ColorMsg() throw()
{}

//-----------------------------------------------------------------------------

ColorMsg::~ColorMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

