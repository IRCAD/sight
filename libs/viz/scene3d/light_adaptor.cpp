/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/light_adaptor.hpp"

#include <service/registry.hpp>

#include <string>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

const std::string light_adaptor::REGISTRY_KEY = "sight::viz::scene3d::light_adaptor::REGISTRY_KEY";

const std::string light_adaptor::POINT_LIGHT       = "Point";
const std::string light_adaptor::DIRECTIONAL_LIGHT = "Directional";
const std::string light_adaptor::SPOT_LIGHT        = "Spot";

const int viz::scene3d::light_adaptor::OFFSET_RANGE = 180;

//-----------------------------------------------------------------------------

viz::scene3d::light_adaptor::sptr light_adaptor::create_light_adaptor(
    data::color::sptr _diffuse,
    data::color::sptr _specular
)
{
    viz::scene3d::light_adaptor::sptr light_adaptor = viz::scene3d::light_factory::make(
        viz::scene3d::light_adaptor::REGISTRY_KEY
    );
    service::register_service(light_adaptor);
    SIGHT_ASSERT("The factory process to create an light_adaptor failed.", light_adaptor);
    SIGHT_ASSERT("The light adaptor must be registered with existing data objects.", _diffuse && _specular);

    light_adaptor->set_inout(_diffuse, "diffuseColor");
    light_adaptor->set_inout(_specular, "specularColor");

    return light_adaptor;
}

//-----------------------------------------------------------------------------

void light_adaptor::destroy_light_adaptor(light_adaptor::sptr _light_adaptor)
{
    if(_light_adaptor)
    {
        _light_adaptor->stop();
        service::unregister_service(_light_adaptor);
    }
}

//-----------------------------------------------------------------------------

viz::scene3d::light_adaptor::light_adaptor()
= default;

//-----------------------------------------------------------------------------

viz::scene3d::light_adaptor::~light_adaptor()
= default;

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
