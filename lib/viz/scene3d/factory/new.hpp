/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#pragma once

#include <sight/viz/scene3d/config.hpp>

#include "viz/scene3d/registry/detail.hpp"

#include <string>

namespace sight::viz::scene3d
{

class window_interactor;
class light_adaptor;
class text;
class layer;

namespace factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  make();

SIGHT_VIZ_SCENE3D_API SPTR(viz::scene3d::window_interactor) make(
    const viz::scene3d::registry::key_t& _classname
);

template<class CLASSNAME>
SPTR(CLASSNAME)  make()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>();
    return obj;
}

} // namespace factory

namespace offscreen_interactor_mgr_factory
{

template<class CLASSNAME>
SPTR(CLASSNAME) make(std::pair<unsigned int, unsigned int> _dims);

SIGHT_VIZ_SCENE3D_API SPTR(viz::scene3d::window_interactor) make(
    const viz::scene3d::registry::key_t& _classname,
    std::pair<unsigned int, unsigned int> _dims
);

template<class CLASSNAME>
SPTR(CLASSNAME) make(std::pair<unsigned int, unsigned int> _dims)
{
    return std::make_shared<CLASSNAME>(_dims.first, _dims.second);
}

} // namespace offscreenInteractorMgrFactory

namespace light_factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  make();

SIGHT_VIZ_SCENE3D_API SPTR(viz::scene3d::light_adaptor) make(
    const viz::scene3d::registry::key_t& _classname
);

template<class CLASSNAME>
SPTR(CLASSNAME) make()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>();
    return obj;
}

} // namespace lightFactory

namespace text_factory
{

template<class CLASSNAME>
SPTR(CLASSNAME) make(const SPTR(sight::viz::scene3d::layer) & _layer);

SIGHT_VIZ_SCENE3D_API SPTR(viz::scene3d::text) make(
    const viz::scene3d::registry::key_t& _classname,
    const SPTR(sight::viz::scene3d::layer) & _layer
);

template<class CLASSNAME>
SPTR(CLASSNAME) make(const SPTR(sight::viz::scene3d::layer) & _layer)
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>(_layer);
    return obj;
}

} // namespace textFactory

} // namespace sight::viz::scene3d
