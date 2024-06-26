/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#pragma once

#include <sight/viz/scene3d/config.hpp>

#include <core/factory_registry.hpp>
#include <core/macros.hpp>

#include <string>

namespace sight::viz::scene3d
{

class window_interactor;
class ICamera;
class light_adaptor;
class text;
class layer;

namespace interactor
{

class base;

} // namespace interactor

namespace registry
{

using key_t = std::string;

using type            = core::factory_registry<std::shared_ptr<viz::scene3d::window_interactor>(), key_t>;
using offscreen_mgr_t = core::factory_registry<std::shared_ptr<viz::scene3d::window_interactor>(std::pair<unsigned int,
                                                                                                          unsigned int>),
                                               key_t>;
using interactor_factory_t = core::factory_registry<std::shared_ptr<viz::scene3d::interactor::base>(), key_t>;
using camera_factory_t     = core::factory_registry<std::shared_ptr<viz::scene3d::ICamera>(), key_t>;
using light_factory_t      = core::factory_registry<std::shared_ptr<viz::scene3d::light_adaptor>(), key_t>;
using text_factory_t       = core::factory_registry<std::shared_ptr<viz::scene3d::text>(const std::shared_ptr<sight::viz::scene3d::layer>&),
                                                    key_t>;

SIGHT_VIZ_SCENE3D_API SPTR(type) get();
SIGHT_VIZ_SCENE3D_API SPTR(offscreen_mgr_t) get_offscreen_mgr();
SIGHT_VIZ_SCENE3D_API SPTR(interactor_factory_t) get_interactor_registry();
SIGHT_VIZ_SCENE3D_API SPTR(camera_factory_t) get_camera_registry();
SIGHT_VIZ_SCENE3D_API SPTR(light_factory_t) get_light_registry();
SIGHT_VIZ_SCENE3D_API SPTR(text_factory_t) get_text_registry();

} // namespace registry

} // namespace sight::viz::scene3d
