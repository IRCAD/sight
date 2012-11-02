/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/TransformationMatrix3DMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::TransformationMatrix3DMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string TransformationMatrix3DMsg::MATRIX_IS_MODIFIED = "MATRIX_IS_MODIFIED";

//-----------------------------------------------------------------------------

TransformationMatrix3DMsg::TransformationMatrix3DMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

TransformationMatrix3DMsg::~TransformationMatrix3DMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

