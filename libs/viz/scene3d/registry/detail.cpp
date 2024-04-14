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

struct render_ogre_registry_instantiator_tag {};

SPTR(type) get()
{
    using instantiator_t = core::lazy_instantiator<type, render_ogre_registry_instantiator_tag>;
    return instantiator_t::get_instance();
}

struct interactor_offscreen_mgr_registry_instantiator_tag {};

SPTR(offscreen_mgr_t) get_offscreen_mgr()
{
    using instantiator_t = core::lazy_instantiator<offscreen_mgr_t,
                                                   interactor_offscreen_mgr_registry_instantiator_tag>;
    return instantiator_t::get_instance();
}

struct interactor_registry_instantiator_tag {};

SPTR(interactor_factory_t) get_interactor_registry()
{
    using instantiator_t = core::lazy_instantiator<interactor_factory_t, interactor_registry_instantiator_tag>;
    return instantiator_t::get_instance();
}

struct camera_registry_instantiator_tag {};

SPTR(camera_factory_t) get_camera_registry()
{
    using instantiator_t = core::lazy_instantiator<camera_factory_t, camera_registry_instantiator_tag>;
    return instantiator_t::get_instance();
}

struct light_registry_instantiator_tag {};

SPTR(light_factory_t) get_light_registry()
{
    using instantiator_t = core::lazy_instantiator<light_factory_t, light_registry_instantiator_tag>;
    return instantiator_t::get_instance();
}

struct text_registry_instantiator_tag {};

SPTR(text_factory_t) get_text_registry()
{
    using instantiator_t = core::lazy_instantiator<text_factory_t, text_registry_instantiator_tag>;
    return instantiator_t::get_instance();
}

} // namespace sight::viz::scene3d::registry
