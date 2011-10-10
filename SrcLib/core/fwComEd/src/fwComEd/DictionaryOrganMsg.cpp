/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/DictionaryOrganMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::DictionaryOrganMsg, ::fwComEd::DictionaryOrganMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string DictionaryOrganMsg::ROI4ORGAN_EXP_IS_MODIFIED  = "ROI4ORGAN_EXP_IS_MODIFIED" ;
std::string DictionaryOrganMsg::ROI4ORGAN_NODE_IS_MODIFIED = "ROI4ORGAN_NODE_IS_MODIFIED" ;
std::string DictionaryOrganMsg::MASK_DATA_NODE_IS_MODIFIED = "MASK_DATA_NODE_IS_MODIFIED" ;
std::string DictionaryOrganMsg::MESH_DATA_NODE_IS_MODIFIED = "MESH_DATA_NODE_IS_MODIFIED" ;

//-----------------------------------------------------------------------------

DictionaryOrganMsg::DictionaryOrganMsg() throw()
{}

//-----------------------------------------------------------------------------

DictionaryOrganMsg::~DictionaryOrganMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

