/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_REGISTRY_DETAIL_HPP__
#define __FWCAMP_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwCamp/config.hpp"

namespace camp
{
class UserObject;
}
namespace fwTools
{
class Object;
}

namespace fwCamp
{
namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::camp::UserObject)(::fwTools::Object*), KeyType > Type;

FWCAMP_API SPTR(Type) get();

} // namespace registry
} // namespace fwCamp

#endif /* __FWCAMP_REGISTRY_DETAIL_HPP__ */


