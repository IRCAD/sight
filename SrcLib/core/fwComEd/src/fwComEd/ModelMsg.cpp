/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/ModelMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::ModelMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string ModelMsg::NEW_MODEL = "NEW_MODEL";

//-----------------------------------------------------------------------------

ModelMsg::ModelMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

ModelMsg::~ModelMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

