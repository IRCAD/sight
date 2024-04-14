/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "viz/scene3d/window_interactor.hpp"

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

const window_interactor::factory_registry_key_t window_interactor::REGISTRY_KEY =
    "::viz::scene3d::window_interactor::REGISTRY_KEY";

const window_interactor::factory_registry_key_t window_interactor::OFFSCREEN_REGISTRY_KEY =
    "::viz::scene3d::window_interactor::OFFSCREEN_REGISTRY_KEY";

//-----------------------------------------------------------------------------

window_interactor::sptr window_interactor::create_manager()
{
    window_interactor::sptr manager = viz::scene3d::factory::make(
        viz::scene3d::window_interactor::REGISTRY_KEY
    );
    SIGHT_ASSERT(
        "The factory process to create an window_interactor, failed. Missing Qt or Wx component ?",
        manager
    );
    return manager;
}

//-----------------------------------------------------------------------------

window_interactor::sptr window_interactor::create_offscreen_manager(
    unsigned int _w,
    unsigned int _h
)
{
    window_interactor::sptr manager = viz::scene3d::offscreen_interactor_mgr_factory::make(
        viz::scene3d::window_interactor::OFFSCREEN_REGISTRY_KEY,
        std::make_pair(_w, _h)
    );
    SIGHT_ASSERT(
        "The factory process to create an window_interactor, failed. Missing Qt or Wx component ?",
        manager
    );
    return manager;
}

//-----------------------------------------------------------------------------

window_interactor::window_interactor()
= default;

//-----------------------------------------------------------------------------

window_interactor::~window_interactor()
= default;

//-----------------------------------------------------------------------------

void window_interactor::set_fullscreen(bool /*unused*/, int /*unused*/)
{
    SIGHT_ERROR("Fullscreen not supported.");
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
