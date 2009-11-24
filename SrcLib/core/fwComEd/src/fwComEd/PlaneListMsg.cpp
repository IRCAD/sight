/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwComEd/PlaneListMsg.hpp"


namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string PlaneListMsg::ADD_PLANE = "PLANE_IS_ADDED";
std::string PlaneListMsg::REMOVE_PLANE = "REMOVE_PLANE";
std::string PlaneListMsg::PLANELIST_VISIBILITY = "PLANELIST_VISIBILITY";

//-----------------------------------------------------------------------------

PlaneListMsg::PlaneListMsg() throw()
{}

//-----------------------------------------------------------------------------

PlaneListMsg::~PlaneListMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

