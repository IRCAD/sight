/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/MeshMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::MeshMsg, ::fwComEd::MeshMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string MeshMsg::NEW_MESH = "NEW_MESH";

//-----------------------------------------------------------------------------

FWCOMED_API MeshMsg::MeshMsg() throw()
{}

//-----------------------------------------------------------------------------

MeshMsg::~MeshMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

