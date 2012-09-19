/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETACONVERSION_CUSTOM_REGISTRY_DETAIL_HPP__
#define __FWMETACONVERSION_CUSTOM_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwMetaConversion/config.hpp"

namespace fwMetaConversion
{
namespace custom
{

class Mapper;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwMetaConversion::custom::Mapper)() , KeyType > Type;

FWMETACONVERSION_API SPTR(Type) get();

} // namespace custom
} // namespace registry
} // namespace fwMetaConversion

#endif /* __FWMETACONVERSION_CUSTOM_REGISTRY_DETAIL_HPP__ */


