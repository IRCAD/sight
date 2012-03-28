/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/ResectionDBMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::ResectionDBMsg, ::fwComEd::ResectionDBMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ResectionDBMsg::NEW_RESECTIONDB_SELECTED = "NEW_RESECTIONDB_SELECTED";
std::string ResectionDBMsg::RESECTIONDB_SELECTED     = "RESECTIONDB_SELECTED";
std::string ResectionDBMsg::NEW_RESECTION_SELECTED   = "NEW_RESECTION_SELECTED";
std::string ResectionDBMsg::NEW_SAFE_PART_SELECTED   = "NEW_SAFE_PART_SELECTED";
std::string ResectionDBMsg::ADD_RESECTION            = "ADD_RESECTION";
std::string ResectionDBMsg::ADD_SAFE_PART            = "ADD_SAFE_PART";
std::string ResectionDBMsg::MODIFIED                 = "MODIFIED";
std::string ResectionDBMsg::RESECTIONDB_INVALIDATED  = "RESECTIONDB_INVALIDATED";

//-----------------------------------------------------------------------------

ResectionDBMsg::ResectionDBMsg() throw()
{}

//-----------------------------------------------------------------------------

ResectionDBMsg::~ResectionDBMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

