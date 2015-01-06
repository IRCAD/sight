/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/MeshMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::MeshMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string MeshMsg::NEW_MESH                 = "NEW_MESH";
std::string MeshMsg::VERTEX_MODIFIED          = "VERTEX_MODIFIED";
std::string MeshMsg::POINT_COLORS_MODIFIED    = "POINT_COLORS_MODIFIED";
std::string MeshMsg::CELL_COLORS_MODIFIED     = "CELL_COLORS_MODIFIED";
std::string MeshMsg::POINT_NORMALS_MODIFIED   = "POINT_NORMALS_MODIFIED";
std::string MeshMsg::CELL_NORMALS_MODIFIED    = "CELL_NORMALS_MODIFIED";
std::string MeshMsg::POINT_TEXCOORDS_MODIFIED = "POINT_TEXCOORDS_MODIFIED";
std::string MeshMsg::CELL_TEXCOORDS_MODIFIED  = "CELL_TEXCOORDS_MODIFIED";


//-----------------------------------------------------------------------------

FWCOMED_API MeshMsg::MeshMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

MeshMsg::~MeshMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

