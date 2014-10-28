/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_REGISTRY_DETAIL_HPP__
#define __FWATOMSPATCH_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{

namespace patcher
{

class IPatcher;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwAtomsPatch::patcher::IPatcher) () , KeyType > Type;

FWATOMSPATCH_API SPTR(Type) get();

} // namespace registry

} // namespace patcher

} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCHE_REGISTRY_DETAIL_HPP__ */


