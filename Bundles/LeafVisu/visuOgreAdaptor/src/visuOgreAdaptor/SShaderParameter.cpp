/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

fwServicesRegisterMacro( ::visuOgreAdaptor::IParameter, ::visuOgreAdaptor::SShaderParameter, ::fwData::Object);

//------------------------------------------------------------------------------

SShaderParameter::SShaderParameter() throw()
{
}

//------------------------------------------------------------------------------

SShaderParameter::~SShaderParameter() throw()
{
}

//------------------------------------------------------------------------------

void SShaderParameter::setMaterialName(const std::string& matName)
{
    m_materialName = matName;
}

//------------------------------------------------------------------------------

void SShaderParameter::doConfigure() throw(::fwTools::Failed)
{
    this->IParameter::doConfigure();

    m_materialName = m_configuration->getAttributeValue("materialName");
    OSLM_ERROR_IF("material attribute not set", m_materialName.empty());
}

//------------------------------------------------------------------------------

void SShaderParameter::doStart() throw(::fwTools::Failed)
{
    // Retrieves the associated material
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_materialName);
    this->setMaterial(material);
}

//------------------------------------------------------------------------------

void SShaderParameter::doStop() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SShaderParameter::doSwap() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
