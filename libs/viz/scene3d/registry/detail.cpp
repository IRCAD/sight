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
    using instantiator_t = core::lazy_instantiator<Type, FwRenderOgreRegistryInstantiatorTag>;
    return instantiator_t::get_instance();
}

struct InteractorOffscreenMgrRegistryInstantiatorTag {};

SPTR(offscreen_mgr_t) get_offscreen_mgr()
{
    using instantiator_t = core::lazy_instantiator<offscreen_mgr_t,
                                                   InteractorOffscreenMgrRegistryInstantiatorTag>;
    return instantiator_t::get_instance();
}

struct InteractorRegistryInstantiatorTag {};

SPTR(interactor_factory_t) get_interactor_registry()
{
    using instantiator_t = core::lazy_instantiator<interactor_factory_t, InteractorRegistryInstantiatorTag>;
    return instantiator_t::get_instance();
}

struct CameraRegistryInstantiatorTag {};

SPTR(camera_factory_t) get_camera_registry()
{
    using instantiator_t = core::lazy_instantiator<camera_factory_t, CameraRegistryInstantiatorTag>;
    return instantiator_t::get_instance();
}

struct LightRegistryInstantiatorTag {};

SPTR(light_factory_t) get_light_registry()
{
    using instantiator_t = core::lazy_instantiator<light_factory_t, LightRegistryInstantiatorTag>;
    return instantiator_t::get_instance();
}

struct TextRegistryInstantiatorTag {};

SPTR(text_factory_t) get_text_registry()
{
    using instantiator_t = core::lazy_instantiator<text_factory_t, TextRegistryInstantiatorTag>;
    return instantiator_t::get_instance();
}

} // namespace sight::viz::scene3d::registry
