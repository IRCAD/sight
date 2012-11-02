/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_CUSTOM_REGISTRY_DETAIL_HPP__
#define __FWATOMCONVERSION_CUSTOM_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{
namespace custom
{

class Mapper;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwAtomConversion::custom::Mapper)() , KeyType > Type;

FWATOMCONVERSION_API SPTR(Type) get();

} // namespace custom
} // namespace registry
} // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_CUSTOM_REGISTRY_DETAIL_HPP__ */


