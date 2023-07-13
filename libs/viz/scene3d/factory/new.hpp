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

class IWindowInteractor;
class ILight;
class IText;
class Layer;

namespace factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR(CLASSNAME) viz::scene3d::factory::New();

Key()
= default;
};

VIZ_SCENE3D_API SPTR(viz::scene3d::IWindowInteractor) New(
    const viz::scene3d::registry::KeyType& classname
);

template<class CLASSNAME>
SPTR(CLASSNAME)  New()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>(Key());
    return obj;
}

} // namespace factory

namespace offscreenInteractorMgrFactory
{

template<class CLASSNAME>
SPTR(CLASSNAME) New(std::pair<unsigned int, unsigned int> _dims);

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR(CLASSNAME) viz::scene3d::offscreenInteractorMgrFactory::New(
    std::pair<unsigned int,
              unsigned int> /*_dims*/
);

Key()
= default;
};

VIZ_SCENE3D_API SPTR(viz::scene3d::IWindowInteractor) New(
    const viz::scene3d::registry::KeyType& classname,
    std::pair<unsigned int, unsigned int> _dims
);

template<class CLASSNAME>
SPTR(CLASSNAME) New(std::pair<unsigned int, unsigned int> _dims)
{
    return std::make_shared<CLASSNAME>(Key(), _dims.first, _dims.second);
}

} // namespace offscreenInteractorMgrFactory

namespace lightFactory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR(CLASSNAME) viz::scene3d::lightFactory::New();
Key() = default;
};

VIZ_SCENE3D_API SPTR(viz::scene3d::ILight) New(
    const viz::scene3d::registry::KeyType& classname
);

template<class CLASSNAME>
SPTR(CLASSNAME) New()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>(Key());
    return obj;
}

} // namespace lightFactory

namespace textFactory
{

template<class CLASSNAME>
SPTR(CLASSNAME) New(const SPTR(sight::viz::scene3d::Layer) & _layer);

class Key
{
template<typename CLASSNAME>
friend SPTR(CLASSNAME) viz::scene3d::textFactory::New(const SPTR(sight::viz::scene3d::Layer) & _layer);
Key() = default;
};

VIZ_SCENE3D_API SPTR(viz::scene3d::IText) New(
    const viz::scene3d::registry::KeyType& classname,
    const SPTR(sight::viz::scene3d::Layer) & _layer
);

template<class CLASSNAME>
SPTR(CLASSNAME) New(const SPTR(sight::viz::scene3d::Layer) & _layer)
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>(Key(), _layer);
    return obj;
}

} // namespace textFactory

} // namespace sight::viz::scene3d
