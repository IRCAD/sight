/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/ILight.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/registry/ObjectService.hpp>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const ::fwRenderOgre::ILight::FactoryRegistryKeyType ILight::REGISTRY_KEY = "::fwRenderOgre::ILight::REGISTRY_KEY";

//-----------------------------------------------------------------------------

::fwRenderOgre::ILight::sptr ILight::createLightManager(::fwData::Color::sptr _diffuse, ::fwData::Color::sptr _specular)
{
    ::fwRenderOgre::ILight::sptr light = ::fwRenderOgre::lightFactory::New(::fwRenderOgre::ILight::REGISTRY_KEY );
    SLM_ASSERT("The factory process to create an ILight failed.", light);
    SLM_ASSERT("The light adaptor must be registered with existing color data objects.", _diffuse && _specular);

    ::fwData::TransformationMatrix3D::sptr tfMat = ::fwData::TransformationMatrix3D::New();
    ::fwServices::OSR::registerService(tfMat, light);
    ::fwServices::OSR::registerService(_diffuse, "diffuseColor",
                                       ::fwServices::IService::AccessType::INOUT, light);
    ::fwServices::OSR::registerService(_specular, "specularColor",
                                       ::fwServices::IService::AccessType::INOUT, light);

    return light;
}

//-----------------------------------------------------------------------------

::fwRenderOgre::ILight::ILight()
{
}

//-----------------------------------------------------------------------------

::fwRenderOgre::ILight::~ILight()
{
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre



