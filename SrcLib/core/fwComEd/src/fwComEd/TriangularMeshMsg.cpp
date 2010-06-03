/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/TriangularMeshMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::TriangularMeshMsg, ::fwComEd::TriangularMeshMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string TriangularMeshMsg::NEW_MESH = "NEW_MESH";

//-----------------------------------------------------------------------------

FWCOMED_API TriangularMeshMsg::TriangularMeshMsg() throw()
{}

//-----------------------------------------------------------------------------

TriangularMeshMsg::~TriangularMeshMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

