/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/TagMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::TagMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string TagMsg::TAG_IS_MODIFIED="TAG_IS_MODIFIED";

//-----------------------------------------------------------------------------

TagMsg::TagMsg(::fwServices::ObjectMsg::Key key)
{}

//-----------------------------------------------------------------------------

TagMsg::~TagMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

