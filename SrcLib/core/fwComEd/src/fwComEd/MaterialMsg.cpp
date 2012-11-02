/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/MaterialMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::MaterialMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------
std::string MaterialMsg::MATERIAL_IS_MODIFIED = "MATERIAL_IS_MODIFIED";
//-----------------------------------------------------------------------------

MaterialMsg::MaterialMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

MaterialMsg::~MaterialMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

