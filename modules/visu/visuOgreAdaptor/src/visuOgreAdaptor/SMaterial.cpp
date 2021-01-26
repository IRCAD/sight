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

#include "visuOgreAdaptor/SMaterial.hpp"

#include "visuOgreAdaptor/SShaderParameter.hpp"
#include "visuOgreAdaptor/STexture.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/Convert.hpp>

#include <data/String.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Field.hpp>

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Material.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <services/macros.hpp>
#include <services/op/Add.hpp>
#include <services/op/Get.hpp>

#include <string>

namespace visuOgreAdaptor
{

const core::com::Slots::SlotKeyType SMaterial::s_UPDATE_FIELD_SLOT   = "updateField";
const core::com::Slots::SlotKeyType SMaterial::s_SWAP_TEXTURE_SLOT   = "swapTexture";
const core::com::Slots::SlotKeyType SMaterial::s_ADD_TEXTURE_SLOT    = "addTexture";
const core::com::Slots::SlotKeyType SMaterial::s_REMOVE_TEXTURE_SLOT = "removeTexture";

const std::string SMaterial::s_MATERIAL_INOUT = "material";

static const std::string s_MATERIAL_TEMPLATE_NAME_CONFIG = "materialTemplate";
static const std::string s_MATERIAL_NAME_CONFIG          = "materialName";
static const std::string s_TEXTURE_NAME_CONFIG           = "textureName";
static const std::string s_SHADING_MODE_CONFIG           = "shadingMode";

//------------------------------------------------------------------------------

SMaterial::SMaterial() noexcept
{
    newSlot(s_UPDATE_FIELD_SLOT, &SMaterial::updateField, this);
    newSlot(s_SWAP_TEXTURE_SLOT, &SMaterial::swapTexture, this);
    newSlot(s_ADD_TEXTURE_SLOT, &SMaterial::createTextureAdaptor, this);
    newSlot(s_REMOVE_TEXTURE_SLOT, &SMaterial::removeTextureAdaptor, this);

    m_representationDict["SURFACE"]   = data::Material::SURFACE;
    m_representationDict["POINT"]     = data::Material::POINT;
    m_representationDict["WIREFRAME"] = data::Material::WIREFRAME;
    m_representationDict["EDGE"]      = data::Material::EDGE;

}

//------------------------------------------------------------------------------

SMaterial::~SMaterial() noexcept
{
}

//------------------------------------------------------------------------------

void SMaterial::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_materialTemplateName = config.get(s_MATERIAL_TEMPLATE_NAME_CONFIG, m_materialTemplateName);
    m_materialName         = config.get(s_MATERIAL_NAME_CONFIG, this->getID());
    m_textureName          = config.get(s_TEXTURE_NAME_CONFIG, m_textureName);
    m_shadingMode          = config.get(s_SHADING_MODE_CONFIG, m_shadingMode);
    m_representationMode   = config.get<std::string>("representationMode", m_representationMode);

    auto it = m_representationDict.find(m_representationMode);
    if(it == m_representationDict.end())
    {
        SLM_ERROR("Value: " + m_representationMode + " is not valid for 'representationMode'."
                  " Accepted values are: SURFACE/POINT/WIREFRAME/EDGE."
                  "'representationMode' is reset to default value (SURFACE). ");
        m_representationMode = "SURFACE";
    }
}

//------------------------------------------------------------------------------

void SMaterial::starting()
{
    this->initialize();
    {
        const auto material = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

        if(!m_shadingMode.empty())
        {
            data::Material::ShadingType shadingMode = data::Material::PHONG;
            if(m_shadingMode == "ambient")
            {
                shadingMode = data::Material::AMBIENT;
            }
            else if(m_shadingMode == "flat")
            {
                shadingMode = data::Material::FLAT;
            }
            else if(m_shadingMode == "gouraud")
            {
                shadingMode = data::Material::GOURAUD;
            }

            // Force the shading mode of the material if it has been set in the configuration of the adaptor
            material->setShadingMode(shadingMode);
        }

        material->setRepresentationMode(m_representationDict[m_representationMode]);

        m_materialFw = std::make_unique< ::fwRenderOgre::Material>(m_materialName, m_materialTemplateName);

        data::String::sptr string = data::String::New();
        string->setValue(m_materialTemplateName);

        ::fwDataTools::helper::Field helper(material.get_shared());
        helper.setField("ogreMaterial", string);
        helper.notify();
    }

    this->createShaderParameterAdaptors();

    // A texture adaptor is configured in the XML scene, we can retrieve it
    if(!m_textureName.empty())
    {
        if(!m_texAdaptor)
        {
            this->setTextureName(m_textureName);
        }

        if(m_texAdaptor->getTextureName().empty())
        {
            m_texAdaptor->setRenderService(this->getRenderService());
            m_texAdaptor->setLayerID(m_layerID);
        }

        m_textureConnection.connect(m_texAdaptor, ::visuOgreAdaptor::STexture::s_TEXTURE_SWAPPED_SIG, this->getSptr(),
                                    ::visuOgreAdaptor::SMaterial::s_SWAP_TEXTURE_SLOT);

        if(m_texAdaptor->isStarted())
        {
            this->swapTexture();
        }
    }
    else
    {
        this->createTextureAdaptor();
    }

    const auto configTree = core::runtime::Convert::toPropertyTree(this->getConfiguration());

    if(configTree.find("config") != configTree.not_found())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

::services::IService::KeyConnectionsMap SMaterial::getAutoConnections() const
{
    services::IService::KeyConnectionsMap connections;
    connections.push(s_MATERIAL_INOUT, data::Material::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MATERIAL_INOUT, data::Material::s_ADDED_FIELDS_SIG, s_UPDATE_FIELD_SLOT);
    connections.push(s_MATERIAL_INOUT, data::Material::s_CHANGED_FIELDS_SIG, s_UPDATE_FIELD_SLOT);
    connections.push(s_MATERIAL_INOUT, data::Material::s_ADDED_TEXTURE_SIG, s_ADD_TEXTURE_SLOT);
    connections.push(s_MATERIAL_INOUT, data::Material::s_REMOVED_TEXTURE_SIG, s_REMOVE_TEXTURE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SMaterial::updating()
{
    const auto material = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

    if(m_r2vbObject)
    {
        m_materialFw->setPrimitiveType(m_r2vbObject->getInputPrimitiveType2());
    }

    // Set up representation mode
    m_materialFw->updatePolygonMode( material->getRepresentationMode() );
    m_materialFw->updateOptionsMode( material->getOptionsMode() );
    m_materialFw->updateShadingMode( material->getShadingMode(), this->getLayer()->getLightsNumber(),
                                     this->hasDiffuseTexture(), m_texAdaptor ? m_texAdaptor->getUseAlpha() : false);
    m_materialFw->updateRGBAMode(material.get_shared());
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::stopping()
{
    m_materialFw.reset();
    m_textureConnection.disconnect();
    this->unregisterServices();

    ::Ogre::MaterialManager::getSingleton().remove(m_materialName, ::fwRenderOgre::RESOURCE_GROUP);

    const auto material = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

    if(material->getField("shaderParameters"))
    {
        material->removeField("shaderParameters");
    }
}

//------------------------------------------------------------------------------

void SMaterial::createShaderParameterAdaptors()
{
    auto material = this->getMaterial();

    SLM_ASSERT( "Material '" + m_materialTemplateName + "'' not found", material );

    const auto constants = ::fwRenderOgre::helper::Shading::findMaterialConstants(*material);
    for(const auto& constant : constants)
    {
        const std::string& constantName = std::get<0>(constant);
        const auto& constantType        = std::get<1>(constant);
        const auto& constantValue       = std::get<3>(constant);

        auto obj = ::fwRenderOgre::helper::Shading::createObjectFromShaderParameter(constantType, constantValue);
        if(obj != nullptr)
        {
            const auto shaderType           = std::get<2>(constant);
            const std::string shaderTypeStr = shaderType == ::Ogre::GPT_VERTEX_PROGRAM ? "vertex" :
                                              shaderType == ::Ogre::GPT_FRAGMENT_PROGRAM ? "fragment" :
                                              "geometry";
            const core::tools::fwID::IDType id = this->getID() + "_" + shaderTypeStr + "-" + constantName;

            // Creates an Ogre adaptor and associates it with the Sight object
            auto srv =
                this->registerService< ::visuOgreAdaptor::SShaderParameter>( "::visuOgreAdaptor::SShaderParameter", id);
            srv->registerInOut(obj, "parameter", true);

            // Naming convention for shader parameters
            srv->setRenderService(this->getRenderService());

            services::IService::ConfigType config;
            config.add("config.<xmlattr>.layer", m_layerID);
            config.add("config.<xmlattr>.parameter", constantName);
            config.add("config.<xmlattr>.shaderType", shaderTypeStr);
            config.add("config.<xmlattr>.materialName", m_materialName);

            srv->setConfiguration(config);
            srv->configure();
            srv->start();

            // Add the object to the shaderParameter composite of the Material to keep the object alive
            const auto materialData = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

            data::Composite::sptr composite = materialData->setDefaultField("shaderParameters",
                                                                            data::Composite::New());
            (*composite)[constantName] = obj;
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::setTextureName(const std::string& _textureName)
{
    if(_textureName.empty())
    {
        m_texAdaptor = nullptr;
    }
    else
    {
        auto textureAdaptors = this->getRenderService()->getAdaptors< ::visuOgreAdaptor::STexture>();
        auto result          =
            std::find_if(textureAdaptors.begin(), textureAdaptors.end(),
                         [_textureName](const ::visuOgreAdaptor::STexture::sptr& srv)
            {
                return srv->getTextureName() == _textureName;
            });

        SLM_ASSERT("STexture adaptor managing texture '" + _textureName + "' is not found",
                   result != textureAdaptors.end());
        m_texAdaptor = *result;
    }

    m_textureName = _textureName;
}

//------------------------------------------------------------------------------

void SMaterial::updateField( data::Object::FieldsContainerType _fields)
{
    for(auto elt : _fields)
    {
        if (elt.first == "ogreMaterial")
        {
            this->unregisterServices("::visuOgreAdaptor::SShaderParameter");
            {
                const auto material = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

                data::String::csptr string = data::String::dynamicCast(elt.second);
                this->setMaterialTemplateName(string->getValue());

                m_materialFw->setTemplate(m_materialTemplateName);

                if(material->getField("shaderParameters"))
                {
                    material->removeField("shaderParameters");
                }
            }
            this->createShaderParameterAdaptors();
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::swapTexture()
{
    SLM_ASSERT("Missing texture adaptor", m_texAdaptor);

    ::Ogre::TexturePtr currentTexture = m_texAdaptor->getTexture();
    SLM_ASSERT("Texture not set in Texture adaptor", currentTexture);

    m_materialFw->setDiffuseTexture(currentTexture);

    // Update the shaders
    const auto material = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

    m_materialFw->updateShadingMode( material->getShadingMode(), this->getLayer()->getLightsNumber(),
                                     this->hasDiffuseTexture(), m_texAdaptor->getUseAlpha() );

    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::createTextureAdaptor()
{
    SLM_ASSERT("Texture adaptor already configured in XML", m_textureName.empty());

    const auto material = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

    // If the associated material has a texture, we have to create a texture adaptor to handle it
    if(material->getDiffuseTexture())
    {
        // Creates an Ogre adaptor and associates it with the Sight texture object
        auto texture = material->getDiffuseTexture();
        m_texAdaptor = this->registerService< ::visuOgreAdaptor::STexture >("::visuOgreAdaptor::STexture");
        m_texAdaptor->registerInput(texture, "image", true);

        m_texAdaptor->setID(this->getID() + "_" + m_texAdaptor->getID());
        m_texAdaptor->setRenderService(this->getRenderService());
        m_texAdaptor->setLayerID(m_layerID);

        const std::string materialName = material->getID();
        m_texAdaptor->setTextureName(materialName + "_Texture");

        m_textureConnection.connect(m_texAdaptor, ::visuOgreAdaptor::STexture::s_TEXTURE_SWAPPED_SIG, this->getSptr(),
                                    ::visuOgreAdaptor::SMaterial::s_SWAP_TEXTURE_SLOT);

        m_texAdaptor->start();
    }
}

//------------------------------------------------------------------------------

void SMaterial::removeTextureAdaptor()
{
    SLM_ASSERT("Missing texture adaptor", m_texAdaptor);
    SLM_ASSERT("Texture adaptor already configured in XML", m_textureName.empty());

    this->getRenderService()->makeCurrent();

    m_materialFw->setDiffuseTexture(::Ogre::TexturePtr());

    m_textureConnection.disconnect();
    this->unregisterServices("::visuOgreAdaptor::STexture");
    m_texAdaptor.reset();

    // Update the shaders
    const auto material = this->getLockedInOut< data::Material >(s_MATERIAL_INOUT);

    m_materialFw->updateShadingMode( material->getShadingMode(), this->getLayer()->getLightsNumber(),
                                     this->hasDiffuseTexture(), false );

    this->requestRender();
}

//-----------------------------------------------------------------------------

} // namespace visuOgreAdaptor.
