/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_REGISTRY_DETAIL_HPP__
#define __FWRENDEROGRE_REGISTRY_DETAIL_HPP__

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
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::interactor::IInteractor) (),
                                         KeyType > InteractorFactoryType;
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::ICamera) (), KeyType > CameraFactoryType;
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::ILight) (), KeyType > LightFactoryType;

FWRENDEROGRE_API SPTR(Type) get();
FWRENDEROGRE_API SPTR(InteractorFactoryType) getInteractorRegistry();
FWRENDEROGRE_API SPTR(CameraFactoryType) getCameraRegistry();
FWRENDEROGRE_API SPTR(LightFactoryType) getLightRegistry();

} // namespace registry

} // namespace fwRenderOgre

#endif /* __FWRENDEROGRE_REGISTRY_DETAIL_HPP__ */

