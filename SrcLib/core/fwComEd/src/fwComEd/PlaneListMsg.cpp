/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/PlaneListMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::PlaneListMsg, ::fwComEd::PlaneListMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string PlaneListMsg::ADD_PLANE            = "PLANE_IS_ADDED";
std::string PlaneListMsg::REMOVE_PLANE         = "REMOVE_PLANE";
std::string PlaneListMsg::PLANELIST_VISIBILITY = "PLANELIST_VISIBILITY";
std::string PlaneListMsg::PLANELIST_MODIFIED   = "PLANELIST_MODIFIED";
std::string PlaneListMsg::DESELECT_ALL_PLANES  = "DESELECT_ALL_PLANES";

//-----------------------------------------------------------------------------

PlaneListMsg::PlaneListMsg() throw()
{}

//-----------------------------------------------------------------------------

PlaneListMsg::~PlaneListMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

