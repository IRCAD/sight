/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/ResectionMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::ResectionMsg, ::fwComEd::ResectionMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ResectionMsg::ADD_RECONSTRUCTION = "ADD_RECONSTRUCTION";
std::string ResectionMsg::VISIBILITY = "VISIBILITY";
std::string ResectionMsg::NEW_RECONSTRUCTION_SELECTED = "NEW_RECONSTRUCTION_SELECTED";
std::string ResectionMsg::MODIFIED = "MODIFIED";


//-----------------------------------------------------------------------------

ResectionMsg::ResectionMsg() throw()
{}

//-----------------------------------------------------------------------------

ResectionMsg::~ResectionMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

