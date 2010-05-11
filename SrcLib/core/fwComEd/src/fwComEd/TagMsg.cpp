/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/TagMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::TagMsg, ::fwComEd::TagMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string TagMsg::TAG_IS_MODIFIED="TAG_IS_MODIFIED";

//-----------------------------------------------------------------------------

TagMsg::TagMsg() throw()
{}

//-----------------------------------------------------------------------------

TagMsg::~TagMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

