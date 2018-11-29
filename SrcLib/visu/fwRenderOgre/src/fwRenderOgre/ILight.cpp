/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#include "fwRenderOgre/ILight.hpp"

#include <fwServices/registry/ObjectService.hpp>

#include <string>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const ::fwRenderOgre::ILight::FactoryRegistryKeyType ILight::REGISTRY_KEY = "::fwRenderOgre::ILight::REGISTRY_KEY";

const std::string ILight::s_POINT_LIGHT       = "Point";
const std::string ILight::s_DIRECTIONAL_LIGHT = "Directional";
const std::string ILight::s_SPOT_LIGHT        = "Spot";

const int ::fwRenderOgre::ILight::s_OFFSET_RANGE = 180;

//-----------------------------------------------------------------------------

::fwRenderOgre::ILight::sptr ILight::createLightAdaptor(::fwData::TransformationMatrix3D::sptr _transform,
                                                        ::fwData::Color::sptr _diffuse,
                                                        ::fwData::Color::sptr _specular)
{
    ::fwRenderOgre::ILight::sptr light = ::fwRenderOgre::lightFactory::New(::fwRenderOgre::ILight::REGISTRY_KEY );
    SLM_ASSERT("The factory process to create an ILight failed.", light);
    SLM_ASSERT("The light adaptor must be registered with existing data objects.", _transform && _diffuse && _specular);

    light->registerInOut(_transform, "transform", true);
    light->registerInOut(_diffuse, "diffuseColor", true);
    light->registerInOut(_specular, "specularColor", true);

    return light;
}

//-----------------------------------------------------------------------------

void ILight::destroyLightAdaptor(ILight::sptr _lightAdaptor)
{
    if(_lightAdaptor)
    {
        _lightAdaptor->stop();
        ::fwServices::OSR::unregisterService(_lightAdaptor);
    }
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

