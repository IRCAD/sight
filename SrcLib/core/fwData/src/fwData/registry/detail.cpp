/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/detail.hpp"

#include <fwCore/util/LazyInstantiator.hpp>

namespace fwData
{

namespace registry
{

struct FwDataRegistryInstantiatorTag {};

SPTR(Type) get()
{
    typedef ::fwCore::util::LazyInstantiator< Type, FwDataRegistryInstantiatorTag > InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry

} // namespace fwData


