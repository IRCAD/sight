/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_REGISTRY_DETAIL_HPP__
#define __FWDATA_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwData/config.hpp"

namespace fwData
{

class Object;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwData::Object) (), KeyType > Type;

FWDATA_API SPTR(Type) get();

} // namespace registry

} // namespace fwData

#endif /* __FWDATA_REGISTRY_DETAIL_HPP__ */


