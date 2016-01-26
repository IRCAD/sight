/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_MAPPER_REGISTRY_DETAIL_HPP__
#define __FWATOMCONVERSION_MAPPER_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{
namespace mapper
{

class Base;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwAtomConversion::mapper::Base)(), KeyType > Type;

FWATOMCONVERSION_API SPTR(Type) get();

} // namespace mapper
} // namespace registry
} // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_MAPPER_REGISTRY_DETAIL_HPP__ */


