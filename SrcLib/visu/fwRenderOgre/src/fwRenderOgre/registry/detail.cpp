/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/registry/detail.hpp"

#include <fwCore/util/LazyInstantiator.hpp>

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

struct CameraRegistryInstantiatorTag {};

SPTR(CameraFactoryType) getCameraRegistry()
{
    typedef ::fwCore::util::LazyInstantiator< CameraFactoryType, CameraRegistryInstantiatorTag >
        InstantiatorType;
    return InstantiatorType::getInstance();
}

struct LightRegistryInstantiatorTag {};

SPTR(LightFactoryType) getLightRegistry()
{
    typedef ::fwCore::util::LazyInstantiator< LightFactoryType, LightRegistryInstantiatorTag >
        InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry

} // namespace fwRenderOgre

