/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_POLICY_REGISTRY_DETAIL_HPP__
#define __FWMEMORY_POLICY_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{

class IPolicy;

namespace policy
{
namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwMemory::IPolicy) () , KeyType > Type;

FWMEMORY_API SPTR(Type) get();

} // namespace registry
} // namespace policy
} // namespace fwMemory

#endif /* __FWMEMORY_POLICY_REGISTRY_DETAIL_HPP__ */


