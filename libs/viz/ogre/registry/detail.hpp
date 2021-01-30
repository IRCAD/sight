/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "viz/ogre/config.hpp"

#include <core/macros.hpp>
#include <core/util/FactoryRegistry.hpp>

#include <string>

namespace sight::viz::ogre
{

class IWindowInteractor;
class ICamera;
class ILight;

namespace interactor
{
class IInteractor;
}

namespace registry
{

typedef std::string KeyType;

typedef core::util::FactoryRegistry< SPTR(viz::ogre::IWindowInteractor) (), KeyType > Type;
typedef core::util::FactoryRegistry< SPTR(viz::ogre::IWindowInteractor)(std::pair<unsigned int,
                                                                                  unsigned int>),
                                     KeyType> OffscreenMgrType;
typedef core::util::FactoryRegistry< SPTR(viz::ogre::interactor::IInteractor) (),
                                     KeyType > InteractorFactoryType;
typedef core::util::FactoryRegistry< SPTR(viz::ogre::ICamera) (), KeyType > CameraFactoryType;
typedef core::util::FactoryRegistry< SPTR(viz::ogre::ILight) (), KeyType > LightFactoryType;

VIZ_OGRE_API SPTR(Type) get();
VIZ_OGRE_API SPTR(OffscreenMgrType) getOffscreenMgr();
VIZ_OGRE_API SPTR(InteractorFactoryType) getInteractorRegistry();
VIZ_OGRE_API SPTR(CameraFactoryType) getCameraRegistry();
VIZ_OGRE_API SPTR(LightFactoryType) getLightRegistry();

} // namespace registry

} // namespace sight::viz::ogre
