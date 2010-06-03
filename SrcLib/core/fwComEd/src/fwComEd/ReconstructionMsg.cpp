/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/ReconstructionMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::ReconstructionMsg, ::fwComEd::ReconstructionMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------
std::string ReconstructionMsg::MESH = "MESH";
std::string ReconstructionMsg::VISIBILITY = "VISIBILITY";
//-----------------------------------------------------------------------------

ReconstructionMsg::ReconstructionMsg() throw()
{}

//-----------------------------------------------------------------------------

ReconstructionMsg::~ReconstructionMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

