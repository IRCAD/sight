/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/util/LazyInstantiator.hpp>

#include "fwRenderOgre/registry/detail.hpp"


namespace fwRenderOgre
{

namespace registry
{

struct FwRenderOgreRegistryInstantiatorTag {};

SPTR(Type) get()
{
    typedef ::fwCore::util::LazyInstantiator< Type, FwRenderOgreRegistryInstantiatorTag > InstantiatorType;
    return InstantiatorType::getInstance();
}

struct InteractorRegistryInstantiatorTag {};

SPTR(InteractorFactoryType) getInteractorRegistry()
{
    typedef ::fwCore::util::LazyInstantiator< InteractorFactoryType, InteractorRegistryInstantiatorTag >
        InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry

} // namespace fwRenderOgre


