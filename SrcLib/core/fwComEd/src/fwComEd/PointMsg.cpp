/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/PointMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::PointMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string PointMsg::VALUE_IS_MODIFIED       = "VALUE_IS_MODIFIED";
std::string PointMsg::POINT_IS_MODIFIED       = "POINT_IS_MODIFIED";
std::string PointMsg::START_POINT_INTERACTION = "START_POINT_INTERACTION";

//-----------------------------------------------------------------------------

PointMsg::PointMsg(::fwServices::ObjectMsg::Key key)
{
}

//-----------------------------------------------------------------------------

PointMsg::~PointMsg() throw()
{
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

