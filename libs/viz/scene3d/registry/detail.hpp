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

#pragma once

#include "viz/scene3d/config.hpp"

#include <core/FactoryRegistry.hpp>
#include <core/macros.hpp>

#include <string>

namespace sight::viz::scene3d
{

class IWindowInteractor;
class ICamera;
class ILight;
class IText;
class Layer;

namespace interactor
{

class IInteractor;

} // namespace interactor

namespace registry
{

typedef std::string KeyType;

typedef core::FactoryRegistry<SPTR(viz::scene3d::IWindowInteractor)(), KeyType> Type;
typedef core::FactoryRegistry<SPTR(viz::scene3d::IWindowInteractor)(std::pair<unsigned int, unsigned int>),
                              KeyType> OffscreenMgrType;
typedef core::FactoryRegistry<SPTR(viz::scene3d::interactor::IInteractor)(), KeyType> InteractorFactoryType;
typedef core::FactoryRegistry<SPTR(viz::scene3d::ICamera)(), KeyType> CameraFactoryType;
typedef core::FactoryRegistry<SPTR(viz::scene3d::ILight)(), KeyType> LightFactoryType;
typedef core::FactoryRegistry<SPTR(viz::scene3d::IText)(const SPTR(sight::viz::scene3d::Layer)& _layer),
                              KeyType> TextFactoryType;

VIZ_SCENE3D_API SPTR(Type) get();
VIZ_SCENE3D_API SPTR(OffscreenMgrType) getOffscreenMgr();
VIZ_SCENE3D_API SPTR(InteractorFactoryType) getInteractorRegistry();
VIZ_SCENE3D_API SPTR(CameraFactoryType) getCameraRegistry();
VIZ_SCENE3D_API SPTR(LightFactoryType) getLightRegistry();
VIZ_SCENE3D_API SPTR(TextFactoryType) getTextRegistry();

} // namespace registry

} // namespace sight::viz::scene3d
