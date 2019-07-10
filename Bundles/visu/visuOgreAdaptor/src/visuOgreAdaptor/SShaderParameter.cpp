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

#include "visuOgreAdaptor/SShaderParameter.hpp"

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwServices/macros.hpp>

#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IParameter, ::visuOgreAdaptor::SShaderParameter, ::fwData::Object);

//------------------------------------------------------------------------------

SShaderParameter::SShaderParameter() noexcept
{
}

//------------------------------------------------------------------------------

SShaderParameter::~SShaderParameter() noexcept
{
}

//------------------------------------------------------------------------------

void SShaderParameter::setMaterialName(const std::string& matName)
{
    m_materialName = matName;
}

//------------------------------------------------------------------------------

void SShaderParameter::configuring()
{
    this->IParameter::configuring();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_materialName = config.get<std::string>("materialName", "");
    OSLM_ERROR_IF("material attribute not set", m_materialName.empty());
}

//------------------------------------------------------------------------------

void SShaderParameter::starting()
{
    this->initialize();

    // Retrieves the associated material
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_materialName);
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

} // namespace visuOgreAdaptor
