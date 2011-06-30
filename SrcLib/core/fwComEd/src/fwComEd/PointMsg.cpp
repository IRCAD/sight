/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/PointMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::PointMsg, ::fwComEd::PointMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string PointMsg::VALUE_IS_MODIFIED       = "VALUE_IS_MODIFIED";
std::string PointMsg::POINT_IS_MODIFIED       = "POINT_IS_MODIFIED";
std::string PointMsg::START_POINT_INTERACTION = "START_POINT_INTERACTION";

//-----------------------------------------------------------------------------

PointMsg::PointMsg() throw()
{}

//-----------------------------------------------------------------------------

PointMsg::~PointMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

