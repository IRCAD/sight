/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "viz/scene3d/registry/detail.hpp"

#include <core/LazyInstantiator.hpp>

namespace sight::viz::scene3d::registry
{

struct FwRenderOgreRegistryInstantiatorTag {};

SPTR(Type) get()
{
    using InstantiatorType = core::LazyInstantiator<Type, FwRenderOgreRegistryInstantiatorTag>;
    return InstantiatorType::getInstance();
}

struct FwRenderOgreOffscreenMgrRegistryInstantiatorTag {};

SPTR(OffscreenMgrType) getOffscreenMgr()
{
    using InstantiatorType = core::LazyInstantiator<OffscreenMgrType,
                                                    FwRenderOgreOffscreenMgrRegistryInstantiatorTag>;
    return InstantiatorType::getInstance();
}

struct InteractorRegistryInstantiatorTag {};

SPTR(InteractorFactoryType) getInteractorRegistry()
{
    using InstantiatorType = core::LazyInstantiator<InteractorFactoryType, InteractorRegistryInstantiatorTag>;
    return InstantiatorType::getInstance();
}

struct CameraRegistryInstantiatorTag {};

SPTR(CameraFactoryType) getCameraRegistry()
{
    using InstantiatorType = core::LazyInstantiator<CameraFactoryType, CameraRegistryInstantiatorTag>;
    return InstantiatorType::getInstance();
}

struct LightRegistryInstantiatorTag {};

SPTR(LightFactoryType) getLightRegistry()
{
    using InstantiatorType = core::LazyInstantiator<LightFactoryType, LightRegistryInstantiatorTag>;
    return InstantiatorType::getInstance();
}

} // namespace sight::viz::scene3d::registry
