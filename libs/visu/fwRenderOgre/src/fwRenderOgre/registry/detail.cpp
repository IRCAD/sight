/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

struct FwRenderOgreOffscreenMgrRegistryInstantiatorTag {};

SPTR(OffscreenMgrType) getOffscreenMgr()
{
    using InstantiatorType = ::fwCore::util::LazyInstantiator<OffscreenMgrType,
                                                              FwRenderOgreOffscreenMgrRegistryInstantiatorTag>;
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
