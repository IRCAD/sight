/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/StringMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::StringMsg, ::fwComEd::StringMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string StringMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

StringMsg::StringMsg() throw()
{}

//-----------------------------------------------------------------------------

StringMsg::~StringMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

