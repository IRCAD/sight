/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/TriangularMeshMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::TriangularMeshMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string TriangularMeshMsg::NEW_MESH = "NEW_MESH";
std::string TriangularMeshMsg::VERTEX_MODIFIED = "VERTEX_MODIFIED";

//-----------------------------------------------------------------------------

TriangularMeshMsg::TriangularMeshMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

TriangularMeshMsg::~TriangularMeshMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

