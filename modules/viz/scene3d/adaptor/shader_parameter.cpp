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

#include "modules/viz/scene3d/adaptor/shader_parameter.hpp"

#include "modules/viz/scene3d/adaptor/material.hpp"

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

namespace sight::module::viz::scene3d::adaptor
{

//------------------------------------------------------------------------------

void shader_parameter::configuring()
{
    this->parameter_adaptor::configuring();

    const config_t config = this->get_config();

    m_material_name = config.get<std::string>(CONFIG + "materialName");
}

//------------------------------------------------------------------------------

void shader_parameter::starting()
{
    this->initialize();

    // Retrieves the associated material
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(
        m_material_name,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    this->set_material(material);
}

//------------------------------------------------------------------------------

void shader_parameter::updating()
{
    // This is typically called when the data has changed through autoconnect
    // So set the parameter as dirty and perform the update
    this->set_dirty();
    this->parameter_adaptor::updating();
}

//------------------------------------------------------------------------------

void shader_parameter::stopping()
{
    this->parameter_adaptor::stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
