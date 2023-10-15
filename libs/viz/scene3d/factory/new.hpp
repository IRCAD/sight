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

#pragma once

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/registry/detail.hpp"

#include <string>

namespace sight::viz::scene3d
{

class window_interactor;
class ILight;
class IText;
class Layer;

namespace factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  make();

VIZ_SCENE3D_API SPTR(viz::scene3d::window_interactor) make(
    const viz::scene3d::registry::key_t& classname
);

template<class CLASSNAME>
SPTR(CLASSNAME)  make()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>();
    return obj;
}

} // namespace factory

namespace offscreenInteractorMgrFactory
{

template<class CLASSNAME>
SPTR(CLASSNAME) make(std::pair<unsigned int, unsigned int> _dims);

VIZ_SCENE3D_API SPTR(viz::scene3d::window_interactor) make(
    const viz::scene3d::registry::key_t& classname,
    std::pair<unsigned int, unsigned int> _dims
);

template<class CLASSNAME>
SPTR(CLASSNAME) make(std::pair<unsigned int, unsigned int> _dims)
{
    return std::make_shared<CLASSNAME>(_dims.first, _dims.second);
}

} // namespace offscreenInteractorMgrFactory

namespace lightFactory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  make();

VIZ_SCENE3D_API SPTR(viz::scene3d::ILight) make(
    const viz::scene3d::registry::key_t& classname
);

template<class CLASSNAME>
SPTR(CLASSNAME) make()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>();
    return obj;
}

} // namespace lightFactory

namespace textFactory
{

template<class CLASSNAME>
SPTR(CLASSNAME) make(const SPTR(sight::viz::scene3d::Layer) & _layer);

VIZ_SCENE3D_API SPTR(viz::scene3d::IText) make(
    const viz::scene3d::registry::key_t& classname,
    const SPTR(sight::viz::scene3d::Layer) & _layer
);

template<class CLASSNAME>
SPTR(CLASSNAME) make(const SPTR(sight::viz::scene3d::Layer) & _layer)
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>(_layer);
    return obj;
}

} // namespace textFactory

} // namespace sight::viz::scene3d
