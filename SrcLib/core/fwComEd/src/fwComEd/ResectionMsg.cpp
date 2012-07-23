/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/ResectionMsg.hpp"

fwServicesMessageRegisterMacro(::fwComEd::ResectionMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ResectionMsg::ADD_RECONSTRUCTION = "ADD_RECONSTRUCTION";
std::string ResectionMsg::VISIBILITY = "VISIBILITY";
std::string ResectionMsg::NEW_RECONSTRUCTION_SELECTED = "NEW_RECONSTRUCTION_SELECTED";
std::string ResectionMsg::MODIFIED = "MODIFIED";


//-----------------------------------------------------------------------------

ResectionMsg::ResectionMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

ResectionMsg::~ResectionMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

