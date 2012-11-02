/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_REGISTRY_DETAIL_HPP__
#define __FWGUI_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{

class GuiBaseObject;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwGui::GuiBaseObject) () , KeyType > Type;

FWGUI_API SPTR(Type) get();

} // namespace registry

} // namespace fwGui

#endif /* __FWGUI_REGISTRY_DETAIL_HPP__ */


