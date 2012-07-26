/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/ReconstructionMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::ReconstructionMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------
std::string ReconstructionMsg::MESH = "MESH";
std::string ReconstructionMsg::VISIBILITY = "VISIBILITY";
//-----------------------------------------------------------------------------

ReconstructionMsg::ReconstructionMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

ReconstructionMsg::~ReconstructionMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

