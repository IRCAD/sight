/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <core/lazy_instantiator.hpp>

namespace sight::viz::scene3d::registry
{

struct FwRenderOgreRegistryInstantiatorTag {};

SPTR(Type) get()
{
    using InstantiatorType = core::lazy_instantiator<Type, FwRenderOgreRegistryInstantiatorTag>;
    return InstantiatorType::get_instance();
}

struct InteractorOffscreenMgrRegistryInstantiatorTag {};

SPTR(OffscreenMgrType) getOffscreenMgr()
{
    using InstantiatorType = core::lazy_instantiator<OffscreenMgrType,
                                                     InteractorOffscreenMgrRegistryInstantiatorTag>;
    return InstantiatorType::get_instance();
}

struct InteractorRegistryInstantiatorTag {};

SPTR(InteractorFactoryType) getInteractorRegistry()
{
    using InstantiatorType = core::lazy_instantiator<InteractorFactoryType, InteractorRegistryInstantiatorTag>;
    return InstantiatorType::get_instance();
}

struct CameraRegistryInstantiatorTag {};

SPTR(CameraFactoryType) getCameraRegistry()
{
    using InstantiatorType = core::lazy_instantiator<CameraFactoryType, CameraRegistryInstantiatorTag>;
    return InstantiatorType::get_instance();
}

struct LightRegistryInstantiatorTag {};

SPTR(LightFactoryType) getLightRegistry()
{
    using InstantiatorType = core::lazy_instantiator<LightFactoryType, LightRegistryInstantiatorTag>;
    return InstantiatorType::get_instance();
}

struct TextRegistryInstantiatorTag {};

SPTR(TextFactoryType) getTextRegistry()
{
    using InstantiatorType = core::lazy_instantiator<TextFactoryType, TextRegistryInstantiatorTag>;
    return InstantiatorType::get_instance();
}

} // namespace sight::viz::scene3d::registry
