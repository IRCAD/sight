/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/FloatMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::FloatMsg, ::fwComEd::FloatMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string FloatMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

FloatMsg::FloatMsg() throw()
{}

//-----------------------------------------------------------------------------

FloatMsg::~FloatMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

