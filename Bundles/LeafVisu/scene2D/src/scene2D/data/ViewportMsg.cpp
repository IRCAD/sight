/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/registry/message/macros.hpp>

#include "scene2D/data/ViewportMsg.hpp"


fwServicesMessageRegisterMacro( ::scene2D::data::ViewportMsg );

namespace scene2D
{
namespace data
{


std::string ViewportMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

ViewportMsg::ViewportMsg(::fwServices::ObjectMsg::Key key)
{
}

ViewportMsg::~ViewportMsg()
{
}


}   // namespace data
}   // namespace scene2D

