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

#pragma once

#include "viz/qt3d/config.hpp"
#include "viz/qt3d/registry/detail.hpp"

#include <string>

namespace sight::viz::qt3d
{

class window_interactor;

namespace factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  make();

VIZ_QT3D_API SPTR(viz::qt3d::window_interactor) make(
    const viz::qt3d::registry::key_t& _classname
);

template<class CLASSNAME>
SPTR(CLASSNAME)  make()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>();
    return obj;
}

} // namespace factory

} // namespace sight::viz::qt3d
