/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/SplineMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::SplineMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string SplineMsg::NEW_SPLINE="NEW_SPLINE";

//-----------------------------------------------------------------------------

SplineMsg::SplineMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

SplineMsg::~SplineMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

