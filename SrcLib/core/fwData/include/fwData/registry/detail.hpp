/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_REGISTRY_DETAIL_HPP__
#define __FWDATA_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/LazyInstantiator.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwData/config.hpp"

namespace fwData
{

class Object;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwData::Object) () , KeyType > Type;

typedef ::fwCore::util::LazyInstantiator< Type > InstantiatorType;

} // namespace registry

} // namespace fwData

FWDATA_TEMPLATE_API template class FWDATA_API ::fwCore::util::LazyInstantiator<
                                                            ::fwData::registry::Type >;

#endif /* __FWDATA_REGISTRY_DETAIL_HPP__ */


