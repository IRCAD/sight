/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/ogre/ILight.hpp"

#include <services/registry/ObjectService.hpp>

#include <string>

namespace sight::viz::ogre
{

//-----------------------------------------------------------------------------

const std::string ILight::REGISTRY_KEY = "::sight::viz::ogre::ILight::REGISTRY_KEY";

const std::string ILight::s_POINT_LIGHT       = "Point";
const std::string ILight::s_DIRECTIONAL_LIGHT = "Directional";
const std::string ILight::s_SPOT_LIGHT        = "Spot";

const int viz::ogre::ILight::s_OFFSET_RANGE = 180;

//-----------------------------------------------------------------------------

viz::ogre::ILight::sptr ILight::createLightAdaptor(data::Color::sptr _diffuse,
                                                   data::Color::sptr _specular)
{
    viz::ogre::ILight::sptr light = viz::ogre::lightFactory::New(viz::ogre::ILight::REGISTRY_KEY );
    SLM_ASSERT("The factory process to create an ILight failed.", light);
    SLM_ASSERT("The light adaptor must be registered with existing data objects.", _diffuse && _specular);

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
        services::OSR::unregisterService(_lightAdaptor);
    }
}

//-----------------------------------------------------------------------------

viz::ogre::ILight::ILight()
{
}

//-----------------------------------------------------------------------------

viz::ogre::ILight::~ILight()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::ogre
