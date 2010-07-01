/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/ModelMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::ModelMsg, ::fwComEd::ModelMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ModelMsg::NEW_MODEL = "NEW_MODEL";

//-----------------------------------------------------------------------------

ModelMsg::ModelMsg() throw()
{}

//-----------------------------------------------------------------------------

ModelMsg::~ModelMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

