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

#include "modules/viz/scene3d/adaptor/material.hpp"

#include "modules/viz/scene3d/adaptor/shader_parameter.hpp"
#include "modules/viz/scene3d/adaptor/texture.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/composite.hpp>
#include <data/helper/Field.hpp>
#include <data/matrix4.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/helper/Shading.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/Utils.hpp>

#include <string>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::slots::key_t material::UPDATE_FIELD_SLOT   = "updateField";
const core::com::slots::key_t material::SWAP_TEXTURE_SLOT   = "swapTexture";
const core::com::slots::key_t material::ADD_TEXTURE_SLOT    = "addTexture";
const core::com::slots::key_t material::REMOVE_TEXTURE_SLOT = "removeTexture";

const std::string material::s_MATERIAL_INOUT = "material";

//------------------------------------------------------------------------------

material::material() noexcept
{
    new_slot(UPDATE_FIELD_SLOT, &material::updateField, this);
    new_slot(SWAP_TEXTURE_SLOT, &material::swapTexture, this);
    new_slot(ADD_TEXTURE_SLOT, &material::createTextureAdaptor, this);
    new_slot(REMOVE_TEXTURE_SLOT, &material::removeTextureAdaptor, this);

    m_representationDict["SURFACE"]   = data::material::SURFACE;
    m_representationDict["POINT"]     = data::material::POINT;
    m_representationDict["WIREFRAME"] = data::material::WIREFRAME;
    m_representationDict["EDGE"]      = data::material::EDGE;
}

//------------------------------------------------------------------------------

void material::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_MATERIAL_TEMPLATE_NAME_CONFIG = s_CONFIG + "materialTemplate";
    static const std::string s_MATERIAL_NAME_CONFIG          = s_CONFIG + "materialName";
    static const std::string s_TEXTURE_NAME_CONFIG           = s_CONFIG + "textureName";
    static const std::string s_SHADING_MODE_CONFIG           = s_CONFIG + "shadingMode";
    static const std::string s_REPRESENTATION_MODE_CONFIG    = s_CONFIG + "representationMode";

    m_materialTemplateName = config.get(s_MATERIAL_TEMPLATE_NAME_CONFIG, m_materialTemplateName);
    m_materialName         = config.get(s_MATERIAL_NAME_CONFIG, this->get_id());
    m_textureName          = config.get(s_TEXTURE_NAME_CONFIG, m_textureName);
    m_shadingMode          = config.get(s_SHADING_MODE_CONFIG, m_shadingMode);
    m_representationMode   = config.get(s_REPRESENTATION_MODE_CONFIG, m_representationMode);

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

void material::configure(
    const std::string& _id,
    const std::string& _name,
    sight::viz::scene3d::render::sptr _service,
    const std::string& _layer,
    const std::string& _shadingMode,
    const std::string& _template
)
{
    this->set_id(_id);
    this->setMaterialName(_name);
    this->setRenderService(_service);
    this->setLayerID(_layer);
    this->setShadingMode(_shadingMode);
    this->setMaterialTemplateName(_template);
}

//------------------------------------------------------------------------------

void material::starting()
{
    this->initialize();
    {
        const auto material = m_materialData.lock();

        if(!m_shadingMode.empty())
        {
            data::material::ShadingType shadingMode = data::material::PHONG;
            if(m_shadingMode == "ambient")
            {
                shadingMode = data::material::AMBIENT;
            }
            else if(m_shadingMode == "flat")
            {
                shadingMode = data::material::FLAT;
            }

            // Force the shading mode of the material if it has been set in the configuration of the adaptor
            material->setShadingMode(shadingMode);
        }

        material->setRepresentationMode(m_representationDict[m_representationMode]);

        m_materialFw = std::make_unique<sight::viz::scene3d::material>(m_materialName, m_materialTemplateName);

        data::string::sptr string = std::make_shared<data::string>();
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
            module::viz::scene3d::adaptor::texture::TEXTURE_SWAPPED_SIG,
            this->get_sptr(),
            module::viz::scene3d::adaptor::material::SWAP_TEXTURE_SLOT
        );

        if(m_texAdaptor->started())
        {
            this->swapTexture();
        }
    }
    else
    {
        this->createTextureAdaptor();
    }

    const auto configTree = this->get_config();

    if(configTree.find("config") != configTree.not_found())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

service::connections_t material::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_MATERIAL_INOUT, data::material::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_MATERIAL_INOUT, data::material::ADDED_FIELDS_SIG, UPDATE_FIELD_SLOT);
    connections.push(s_MATERIAL_INOUT, data::material::CHANGED_FIELDS_SIG, UPDATE_FIELD_SLOT);
    connections.push(s_MATERIAL_INOUT, data::material::ADDED_TEXTURE_SIG, ADD_TEXTURE_SLOT);
    connections.push(s_MATERIAL_INOUT, data::material::REMOVED_TEXTURE_SIG, REMOVE_TEXTURE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void material::updating()
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

void material::stopping()
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

void material::createShaderParameterAdaptors()
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
            const core::tools::id::type id =
                std::string(this->get_id()) + "_" + shaderTypeStr + "-" + constantName;

            // Creates an Ogre adaptor and associates it with the Sight object
            auto srv =
                this->registerService<sight::module::viz::scene3d::adaptor::shader_parameter>(
                    "sight::module::viz::scene3d::adaptor::shader_parameter",
                    id
                );
            srv->set_inout(obj, "parameter", true);

            // Naming convention for shader parameters
            srv->setRenderService(this->getRenderService());

            service::config_t config;
            config.add("config.<xmlattr>.parameter", constantName);
            config.add("config.<xmlattr>.shaderType", shaderTypeStr);
            config.add("config.<xmlattr>.materialName", m_materialName);

            srv->setLayerID(m_layerID);
            srv->set_config(config);
            srv->configure();
            srv->start();

            // Add the object to the shaderParameter composite of the Material to keep the object alive
            const auto materialData = m_materialData.lock();

            data::composite::sptr composite = materialData->setDefaultField(
                "shaderParameters",
                std::make_shared<data::composite>()
            );
            (*composite)[constantName] = obj;
        }
    }
}

//------------------------------------------------------------------------------

void material::setTextureName(const std::string& _textureName)
{
    if(_textureName.empty())
    {
        m_texAdaptor = nullptr;
    }
    else
    {
        auto textureAdaptors =
            this->getRenderService()->getAdaptors<sight::module::viz::scene3d::adaptor::texture>();
        auto result =
            std::find_if(
                textureAdaptors.begin(),
                textureAdaptors.end(),
                [_textureName](const module::viz::scene3d::adaptor::texture::sptr& srv)
            {
                return srv->getTextureName() == _textureName;
            });

        SIGHT_ASSERT(
            "texture adaptor managing texture '" + _textureName + "' is not found",
            result != textureAdaptors.end()
        );
        m_texAdaptor = *result;
    }

    m_textureName = _textureName;
}

//------------------------------------------------------------------------------

void material::updateField(data::object::FieldsContainerType _fields)
{
    for(const auto& elt : _fields)
    {
        if(elt.first == "ogreMaterial")
        {
            this->unregisterServices("sight::module::viz::scene3d::adaptor::shader_parameter");
            {
                const auto material = m_materialData.lock();

                data::string::csptr string = std::dynamic_pointer_cast<data::string>(elt.second);
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

void material::swapTexture()
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

void material::createTextureAdaptor()
{
    SIGHT_ASSERT("Texture adaptor already configured in XML", m_textureName.empty());

    const auto material = m_materialData.lock();

    // If the associated material has a texture, we have to create a texture adaptor to handle it
    if(material->getDiffuseTexture())
    {
        // Creates an Ogre adaptor and associates it with the Sight texture object
        auto texture = material->getDiffuseTexture();
        m_texAdaptor = this->registerService<module::viz::scene3d::adaptor::texture>(
            "sight::module::viz::scene3d::adaptor::texture"
        );
        m_texAdaptor->set_input(texture, "image", true);

        m_texAdaptor->set_id(this->get_id() + "_" + m_texAdaptor->get_id());
        m_texAdaptor->setRenderService(this->getRenderService());
        m_texAdaptor->setLayerID(m_layerID);

        const std::string materialName = material->get_id();
        m_texAdaptor->setTextureName(materialName + "_Texture");

        m_textureConnection.connect(
            m_texAdaptor,
            module::viz::scene3d::adaptor::texture::TEXTURE_SWAPPED_SIG,
            this->get_sptr(),
            module::viz::scene3d::adaptor::material::SWAP_TEXTURE_SLOT
        );

        m_texAdaptor->start();
    }
}

//------------------------------------------------------------------------------

void material::removeTextureAdaptor()
{
    SIGHT_ASSERT("Missing texture adaptor", m_texAdaptor);
    SIGHT_ASSERT("Texture adaptor already configured in XML", m_textureName.empty());

    this->getRenderService()->makeCurrent();

    m_materialFw->setDiffuseTexture(Ogre::TexturePtr());

    m_textureConnection.disconnect();
    this->unregisterServices("sight::module::viz::scene3d::adaptor::texture");
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
