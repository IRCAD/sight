/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "fwRenderOgre/config.hpp"

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include <string>

namespace fwRenderOgre
{

class IRenderWindowInteractorManager;
class ICamera;
class ILight;

namespace interactor
{
class IInteractor;
}

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::IRenderWindowInteractorManager) (), KeyType > Type;
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::IRenderWindowInteractorManager)(std::pair<unsigned int,
                                                                                                        unsigned int>),
                                         KeyType> OffscreenMgrType;
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::interactor::IInteractor) (),
                                         KeyType > InteractorFactoryType;
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::ICamera) (), KeyType > CameraFactoryType;
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::ILight) (), KeyType > LightFactoryType;

FWRENDEROGRE_API SPTR(Type) get();
FWRENDEROGRE_API SPTR(OffscreenMgrType) getOffscreenMgr();
FWRENDEROGRE_API SPTR(InteractorFactoryType) getInteractorRegistry();
FWRENDEROGRE_API SPTR(CameraFactoryType) getCameraRegistry();
FWRENDEROGRE_API SPTR(LightFactoryType) getLightRegistry();

} // namespace registry

} // namespace fwRenderOgre
