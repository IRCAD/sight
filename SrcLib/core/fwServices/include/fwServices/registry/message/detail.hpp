/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_MESSAGE_DETAIL_HPP__
#define __FWSERVICES_REGISTRY_MESSAGE_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwServices/config.hpp"

namespace fwServices
{

class ObjectMsg;

namespace registry
{

namespace message
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwServices::ObjectMsg) (), KeyType > Type;

FWSERVICES_API SPTR( Type ) get();

} // namespace message

} // namespace registry

} // namespace fwServices

#endif /* __FWSERVICES_REGISTRY_MESSAGE_DETAIL_HPP__ */


