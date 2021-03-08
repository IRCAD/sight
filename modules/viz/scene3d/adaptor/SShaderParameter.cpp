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

#include "modules/viz/scene3d/adaptor/SShaderParameter.hpp"

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

namespace sight::module::viz::scene3d::adaptor
{

static const std::string s_MATERIAL_NAME_CONFIG = "materialName";

//------------------------------------------------------------------------------

SShaderParameter::SShaderParameter() noexcept
{
}

//------------------------------------------------------------------------------

SShaderParameter::~SShaderParameter() noexcept
{
}

//------------------------------------------------------------------------------

void SShaderParameter::configuring()
{
    this->IParameter::configuring();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_materialName = config.get<std::string>(s_MATERIAL_NAME_CONFIG, "");
    SIGHT_ERROR_IF("'" + s_MATERIAL_NAME_CONFIG + "' attribute not set", m_materialName.empty());
}

//------------------------------------------------------------------------------

void SShaderParameter::starting()
{
    this->initialize();

    // Retrieves the associated material
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_materialName,
                                                                                     sight::viz::scene3d::RESOURCE_GROUP);
    this->setMaterial(material);
}

//------------------------------------------------------------------------------

void SShaderParameter::updating()
{
    // This is typically called when the data has changed through autoconnect
    // So set the parameter as dirty and perform the update
    this->setDirty();
    this->IParameter::updating();
}

//------------------------------------------------------------------------------

void SShaderParameter::stopping()
{
    this->IParameter::stopping();
}

//------------------------------------------------------------------------------

void SShaderParameter::swapping()
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
