/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/ICamera.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/registry/ObjectService.hpp>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const ::fwRenderOgre::ICamera::FactoryRegistryKeyType ICamera::REGISTRY_KEY = "::fwRenderOgre::ICamera::REGISTRY_KEY";

//-----------------------------------------------------------------------------

::fwRenderOgre::ICamera::sptr ICamera::createCameraManager()
{
    ::fwRenderOgre::ICamera::sptr camera = ::fwRenderOgre::cameraFactory::New(::fwRenderOgre::ICamera::REGISTRY_KEY);
    SLM_ASSERT("The factory process to create an ICamera failed.", camera);

    ::fwData::TransformationMatrix3D::sptr tfMat = ::fwData::TransformationMatrix3D::New();
    ::fwServices::OSR::registerService(tfMat, camera);

    return camera;
}

//-----------------------------------------------------------------------------

void ICamera::destroyCameraManager(ICamera::sptr _cameraManager)
{
    if(_cameraManager)
    {
        _cameraManager->stop();
        ::fwServices::OSR::unregisterService(_cameraManager);
    }
}

//-----------------------------------------------------------------------------

::fwRenderOgre::ICamera::ICamera()
{
}

//-----------------------------------------------------------------------------

::fwRenderOgre::ICamera::~ICamera()
{
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
