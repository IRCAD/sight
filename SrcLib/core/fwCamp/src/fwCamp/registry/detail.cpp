/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <camp/userobject.hpp>

#include <fwCore/util/LazyInstantiator.hpp>

#include "fwCamp/registry/detail.hpp"

namespace fwCamp
{
namespace registry
{

struct FwCampRegistryInstantiatorTag {} ;

SPTR(Type) get()
{
    typedef ::fwCore::util::LazyInstantiator< Type, FwCampRegistryInstantiatorTag > InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry
} // namespace fwCamp


