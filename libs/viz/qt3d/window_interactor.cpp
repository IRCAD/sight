/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "window_interactor.hpp"

namespace sight::viz::qt3d
{

//-----------------------------------------------------------------------------

const window_interactor::FactoryRegistryKeyType window_interactor::REGISTRY_KEY =
    "::viz::qt3d::window_interactor::REGISTRY_KEY";

//-----------------------------------------------------------------------------

window_interactor::sptr window_interactor::createManager()
{
    window_interactor::sptr manager = viz::qt3d::factory::make(
        viz::qt3d::window_interactor::REGISTRY_KEY
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

} // namespace sight::viz::qt3d.
