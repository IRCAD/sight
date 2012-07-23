/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/PointListMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::PointListMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string PointListMsg::ELEMENT_MODIFIED = "ELEMENT_MODIFIED";
std::string PointListMsg::ELEMENT_ADDED    = "ELEMENT_ADDED";
std::string PointListMsg::ELEMENT_REMOVED  = "ELEMENT_REMOVED";

//-----------------------------------------------------------------------------

PointListMsg::PointListMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

PointListMsg::~PointListMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

