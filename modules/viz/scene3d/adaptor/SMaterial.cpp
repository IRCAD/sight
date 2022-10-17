/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"

#include "modules/viz/scene3d/adaptor/SShaderParameter.hpp"
#include "modules/viz/scene3d/adaptor/STexture.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/helper/Field.hpp>
#include <data/Matrix4.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/op/Get.hpp>

#include <viz/scene3d/helper/Shading.hpp>
#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/Material.hpp>
#include <viz/scene3d/Utils.hpp>

#include <string>

namespace sight::module::viz::scene3d::adaptor
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

SMaterial::~SMaterial() noexcept =
    default;

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
        SIGHT_ERROR(
            "Value: " + m_representationMode + " is not valid for 'representationMode'."
                                               " Accepted values are: SURFACE/POINT/WIREFRAME/EDGE."
                                               "'representationMode' is reset to default value (SURFACE). "
        );
        m_representationMode = "SURFACE";
    }
}

//------------------------------------------------------------------------------

void SMaterial::starting()
{
    this->initialize();
    {
        const auto material = m_materialData.lock();

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

            // Force the shading mode of the material if it has been set in the configuration of the adaptor
            material->setShadingMode(shadingMode);
        }

        material->setRepresentationMode(m_representationDict[m_representationMode]);

        m_materialFw = std::make_unique<sight::viz::scene3d::Material>(m_materialName, m_materialTemplateName);

        data::String::sptr string = data::String::New();
        string->setValue(m_materialTemplateName);

        data::helper::Field helper(material.get_shared());
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

        m_textureConnection.connect(
            m_texAdaptor,
            module::viz::scene3d::adaptor::STexture::s_TEXTURE_SWAPPED_SIG,
            this->getSptr(),
            module::viz::scene3d::adaptor::SMaterial::s_SWAP_TEXTURE_SLOT
        );

        if(m_texAdaptor->isStarted())
        {
            this->swapTexture();
        }
    }
    else
    {
        this->createTextureAdaptor();
    }

    const auto configTree = this->getConfiguration();

    if(configTree.find("config") != configTree.not_found())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMaterial::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
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
    const auto material = m_materialData.lock();

    if(m_r2vbObject != nullptr)
    {
        m_materialFw->setPrimitiveType(m_r2vbObject->getInputPrimitiveType());
    }

    // Set up representation mode
    m_materialFw->updatePolygonMode(material->getRepresentationMode());
    m_materialFw->updateOptionsMode(material->getOptionsMode());
    m_materialFw->updateShadingMode(
        material->getShadingMode(),
        this->getLayer()->getLightsNumber(),
        this->hasDiffuseTexture(),
        m_texAdaptor ? m_texAdaptor->getUseAlpha() : false
    );
    m_materialFw->updateRGBAMode(material.get_shared());
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::stopping()
{
    m_materialFw.reset();
    m_textureConnection.disconnect();
    this->unregisterServices();

    const auto material = m_materialData.lock();

    if(material->getField("shaderParameters"))
    {
        material->removeField("shaderParameters");
    }
}

//------------------------------------------------------------------------------

void SMaterial::createShaderParameterAdaptors()
{
    auto material = this->getMaterial();

    SIGHT_ASSERT("Material '" + m_materialTemplateName + "'' not found", material);

    const auto constants = sight::viz::scene3d::helper::Shading::findMaterialConstants(*material);
    for(const auto& constant : constants)
    {
        const std::string& constantName = std::get<0>(constant);
        const auto& constantType        = std::get<1>(constant);
        const auto& constantValue       = std::get<3>(constant);

        auto obj = sight::viz::scene3d::helper::Shading::createObjectFromShaderParameter(constantType, constantValue);
        if(obj != nullptr)
        {
            const auto shaderType           = std::get<2>(constant);
            const std::string shaderTypeStr = shaderType == Ogre::GPT_VERTEX_PROGRAM ? "vertex"
                                                                                     : shaderType
                                              == Ogre::GPT_FRAGMENT_PROGRAM ? "fragment"
                                                                            :
                                              "geometry";
            const core::tools::fwID::IDType id =
                std::string(this->getID()) + "_" + shaderTypeStr + "-" + constantName;

            // Creates an Ogre adaptor and associates it with the Sight object
            auto srv =
                this->registerService<sight::module::viz::scene3d::adaptor::SShaderParameter>(
                    "sight::module::viz::scene3d::adaptor::SShaderParameter",
                    id
                );
            srv->setInOut(obj, "parameter", true);

            // Naming convention for shader parameters
            srv->setRenderService(this->getRenderService());

            service::IService::ConfigType config;
            config.add("config.<xmlattr>.layer", m_layerID);
            config.add("config.<xmlattr>.parameter", constantName);
            config.add("config.<xmlattr>.shaderType", shaderTypeStr);
            config.add("config.<xmlattr>.materialName", m_materialName);

            srv->setConfiguration(config);
            srv->configure();
            srv->start();

            // Add the object to the shaderParameter composite of the Material to keep the object alive
            const auto materialData = m_materialData.lock();

            data::Composite::sptr composite = materialData->setDefaultField(
                "shaderParameters",
                data::Composite::New()
            );
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
        auto textureAdaptors =
            this->getRenderService()->getAdaptors<sight::module::viz::scene3d::adaptor::STexture>();
        auto result =
            std::find_if(
                textureAdaptors.begin(),
                textureAdaptors.end(),
                [_textureName](const module::viz::scene3d::adaptor::STexture::sptr& srv)
            {
                return srv->getTextureName() == _textureName;
            });

        SIGHT_ASSERT(
            "STexture adaptor managing texture '" + _textureName + "' is not found",
            result != textureAdaptors.end()
        );
        m_texAdaptor = *result;
    }

    m_textureName = _textureName;
}

//------------------------------------------------------------------------------

void SMaterial::updateField(data::Object::FieldsContainerType _fields)
{
    for(const auto& elt : _fields)
    {
        if(elt.first == "ogreMaterial")
        {
            this->unregisterServices("sight::module::viz::scene3d::adaptor::SShaderParameter");
            {
                const auto material = m_materialData.lock();

                data::String::csptr string = data::String::dynamicCast(elt.second);
                if(string->value() == m_materialTemplateName)
                {
                    // Avoid useless update if this is the same template material
                    continue;
                }

                this->setMaterialTemplateName(string->getValue());

                m_materialFw->setTemplate(m_materialTemplateName);

                if(material->getField("shaderParameters"))
                {
                    material->removeField("shaderParameters");
                }
            }
            this->createShaderParameterAdaptors();
            this->updating();

            // When resetting the material template, all techniques and passes will be destroyed,
            // so we need to reset the texture unit states
            Ogre::TexturePtr currentTexture = m_texAdaptor->getTexture();
            if(currentTexture)
            {
                m_materialFw->setDiffuseTexture(currentTexture);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::swapTexture()
{
    SIGHT_ASSERT("Missing texture adaptor", m_texAdaptor);

    Ogre::TexturePtr currentTexture = m_texAdaptor->getTexture();
    SIGHT_ASSERT("Texture not set in Texture adaptor", currentTexture);

    m_materialFw->setDiffuseTexture(currentTexture);

    // Update the shaders
    const auto material = m_materialData.lock();

    m_materialFw->updateShadingMode(
        material->getShadingMode(),
        this->getLayer()->getLightsNumber(),
        this->hasDiffuseTexture(),
        m_texAdaptor->getUseAlpha()
    );

    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::createTextureAdaptor()
{
    SIGHT_ASSERT("Texture adaptor already configured in XML", m_textureName.empty());

    const auto material = m_materialData.lock();

    // If the associated material has a texture, we have to create a texture adaptor to handle it
    if(material->getDiffuseTexture())
    {
        // Creates an Ogre adaptor and associates it with the Sight texture object
        auto texture = material->getDiffuseTexture();
        m_texAdaptor = this->registerService<module::viz::scene3d::adaptor::STexture>(
            "sight::module::viz::scene3d::adaptor::STexture"
        );
        m_texAdaptor->setInput(texture, "image", true);

        m_texAdaptor->setID(this->getID() + "_" + m_texAdaptor->getID());
        m_texAdaptor->setRenderService(this->getRenderService());
        m_texAdaptor->setLayerID(m_layerID);

        const std::string materialName = material->getID();
        m_texAdaptor->setTextureName(materialName + "_Texture");

        m_textureConnection.connect(
            m_texAdaptor,
            module::viz::scene3d::adaptor::STexture::s_TEXTURE_SWAPPED_SIG,
            this->getSptr(),
            module::viz::scene3d::adaptor::SMaterial::s_SWAP_TEXTURE_SLOT
        );

        m_texAdaptor->start();
    }
}

//------------------------------------------------------------------------------

void SMaterial::removeTextureAdaptor()
{
    SIGHT_ASSERT("Missing texture adaptor", m_texAdaptor);
    SIGHT_ASSERT("Texture adaptor already configured in XML", m_textureName.empty());

    this->getRenderService()->makeCurrent();

    m_materialFw->setDiffuseTexture(Ogre::TexturePtr());

    m_textureConnection.disconnect();
    this->unregisterServices("sight::module::viz::scene3d::adaptor::STexture");
    m_texAdaptor.reset();

    // Update the shaders
    const auto material = m_materialData.lock();

    m_materialFw->updateShadingMode(
        material->getShadingMode(),
        this->getLayer()->getLightsNumber(),
        this->hasDiffuseTexture(),
        false
    );

    this->requestRender();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
