/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "scene2D/data/ViewportMsg.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::scene2D::data::ViewportMsg, ::scene2D::data::ViewportMsg );

namespace scene2D
{
namespace data
{


std::string ViewportMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

ViewportMsg::ViewportMsg() throw()
{
}

ViewportMsg::~ViewportMsg() throw()
{
}


}   // namespace data
}   // namespace scene2D

