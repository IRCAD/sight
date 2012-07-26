/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/DictionaryOrganMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::DictionaryOrganMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string DictionaryOrganMsg::ROI4ORGAN_EXP_IS_MODIFIED  = "ROI4ORGAN_EXP_IS_MODIFIED" ;
std::string DictionaryOrganMsg::ROI4ORGAN_NODE_IS_MODIFIED = "ROI4ORGAN_NODE_IS_MODIFIED" ;
std::string DictionaryOrganMsg::MASK_DATA_NODE_IS_MODIFIED = "MASK_DATA_NODE_IS_MODIFIED" ;
std::string DictionaryOrganMsg::MESH_DATA_NODE_IS_MODIFIED = "MESH_DATA_NODE_IS_MODIFIED" ;

//-----------------------------------------------------------------------------

DictionaryOrganMsg::DictionaryOrganMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

DictionaryOrganMsg::~DictionaryOrganMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

