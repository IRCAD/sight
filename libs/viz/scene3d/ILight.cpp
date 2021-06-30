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

#include "viz/scene3d/ILight.hpp"

#include <service/registry/ObjectService.hpp>

#include <string>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

const std::string ILight::REGISTRY_KEY = "sight::viz::scene3d::ILight::REGISTRY_KEY";

const std::string ILight::s_POINT_LIGHT       = "Point";
const std::string ILight::s_DIRECTIONAL_LIGHT = "Directional";
const std::string ILight::s_SPOT_LIGHT        = "Spot";

const int viz::scene3d::ILight::s_OFFSET_RANGE = 180;

//-----------------------------------------------------------------------------

viz::scene3d::ILight::sptr ILight::createLightAdaptor(
    data::Color::sptr _diffuse,
    data::Color::sptr _specular
)
{
    viz::scene3d::ILight::sptr light = viz::scene3d::lightFactory::New(viz::scene3d::ILight::REGISTRY_KEY);
    SIGHT_ASSERT("The factory process to create an ILight failed.", light);
    SIGHT_ASSERT("The light adaptor must be registered with existing data objects.", _diffuse && _specular);

    light->setInOut(_diffuse, "diffuseColor");
    light->setInOut(_specular, "specularColor");

    return light;
}

//-----------------------------------------------------------------------------

void ILight::destroyLightAdaptor(ILight::sptr _lightAdaptor)
{
    if(_lightAdaptor)
    {
        _lightAdaptor->stop();
        service::OSR::unregisterService(_lightAdaptor);
    }
}

//-----------------------------------------------------------------------------

viz::scene3d::ILight::ILight()
{
}

//-----------------------------------------------------------------------------

viz::scene3d::ILight::~ILight()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
