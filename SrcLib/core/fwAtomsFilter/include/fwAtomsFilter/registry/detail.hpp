/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSFILTER_REGISTRY_DETAIL_HPP__
#define __FWATOMSFILTER_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwAtomsFilter/config.hpp"

namespace fwAtomsFilter
{

class IFilter;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwAtomsFilter::IFilter) () , KeyType > Type;

FWATOMSFILTER_API SPTR(Type) get();

} // namespace registry

} // namespace fwAtomsFilter

#endif /* __FWATOMSFILTER_REGISTRY_DETAIL_HPP__ */

