/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SMaterial.hpp"

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SShaderParameter.hpp"
#include "visuOgreAdaptor/STexture.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Field.hpp>

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Material.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/Convert.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <boost/make_unique.hpp>

#include <string>

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SMaterial, ::fwData::Material );

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SMaterial::s_UPDATE_FIELD_SLOT   = "updateField";
const ::fwCom::Slots::SlotKeyType SMaterial::s_SWAP_TEXTURE_SLOT   = "swapTexture";
const ::fwCom::Slots::SlotKeyType SMaterial::s_ADD_TEXTURE_SLOT    = "addTexture";
const ::fwCom::Slots::SlotKeyType SMaterial::s_REMOVE_TEXTURE_SLOT = "removeTexture";

const std::string SMaterial::s_MATERIAL_INOUT = "material";

//------------------------------------------------------------------------------

SMaterial::SMaterial() noexcept :
    m_materialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_lightsNumber(1)
{
    newSlot(s_UPDATE_FIELD_SLOT, &SMaterial::updateField, this);
    newSlot(s_SWAP_TEXTURE_SLOT, &SMaterial::swapTexture, this);
    newSlot(s_ADD_TEXTURE_SLOT, &SMaterial::createTextureAdaptor, this);
    newSlot(s_REMOVE_TEXTURE_SLOT, &SMaterial::removeTextureAdaptor, this);
}

//------------------------------------------------------------------------------

SMaterial::~SMaterial() noexcept
{

}

//------------------------------------------------------------------------------

void SMaterial::createShaderParameterAdaptors()
{
    // We retrieve the parameters of the base material in a temporary material
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
            const fwTools::fwID::IDType id = this->getID() + "_" + shaderTypeStr + "-" + constantName;

            // Creates an Ogre adaptor and associates it with the f4s object
            auto srv =
                this->registerService< ::visuOgreAdaptor::SShaderParameter>( "::visuOgreAdaptor::SShaderParameter", id);
            srv->registerInOut(obj, "parameter", true);

            // Naming convention for shader parameters
            srv->setRenderService(this->getRenderService());

            ::fwServices::IService::ConfigType config;
            config.add("config.<xmlattr>.layer", m_layerID);
            config.add("config.<xmlattr>.parameter", constantName);
            config.add("config.<xmlattr>.shaderType", shaderTypeStr);
            config.add("config.<xmlattr>.materialName", m_materialName);

            srv->setConfiguration(config);
            srv->configure();
            srv->start();

            // Add the object to the shaderParameter composite of the Material to keep the object alive
            ::fwData::Material::sptr material   = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);
            ::fwData::Composite::sptr composite = material->setDefaultField("shaderParameters",
                                                                            ::fwData::Composite::New());
            (*composite)[constantName] = obj;
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::setTextureName(const std::string& textureName)
{
    if(textureName.empty())
    {
        m_texAdaptor = nullptr;
    }
    else
    {
        auto textureAdaptors = this->getRenderService()->getAdaptors< ::visuOgreAdaptor::STexture>();
        auto result          =
            std::find_if(textureAdaptors.begin(), textureAdaptors.end(),
                         [textureName](const ::visuOgreAdaptor::STexture::sptr& srv)
            {
                return srv->getTextureName() == textureName;
            });

        SLM_ASSERT("STexture adaptor managing texture '" + textureName + "' is not found",
                   result != textureAdaptors.end());
        m_texAdaptor = *result;
    }

    m_textureName = textureName;
}

//------------------------------------------------------------------------------

void SMaterial::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count("materialTemplate"))
    {
        m_materialTemplateName = config.get<std::string>("materialTemplate");
    }

    if(config.count("materialName"))
    {
        m_materialName = config.get<std::string>("materialName");
    }
    else
    {
        // Choose a default name if not provided
        m_materialName = this->getID();
    }

    if(config.count("textureName"))
    {
        m_textureName = config.get<std::string>("textureName");
    }

    if(config.count("shadingMode"))
    {
        m_shadingMode = config.get<std::string>("shadingMode");
    }
}

//------------------------------------------------------------------------------

void SMaterial::starting()
{
    this->initialize();

    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);

    if(!m_shadingMode.empty())
    {
        ::fwData::Material::ShadingType shadingMode = ::fwData::Material::PHONG;
        if(m_shadingMode == "ambient")
        {
            shadingMode = ::fwData::Material::AMBIENT;
        }
        else if(m_shadingMode == "flat")
        {
            shadingMode = ::fwData::Material::FLAT;
        }
        else if(m_shadingMode == "gouraud")
        {
            shadingMode = ::fwData::Material::GOURAUD;
        }

        // Force the shading mode of the material if it has been set in the configuration of the adaptor
        material->setShadingMode(shadingMode);
    }

    m_materialFw = ::boost::make_unique< ::fwRenderOgre::Material>(m_materialName, m_materialTemplateName);

    ::fwData::String::sptr string = ::fwData::String::New();
    string->setValue(m_materialTemplateName);

    ::fwDataTools::helper::Field helper(material);
    helper.setField("ogreMaterial", string);
    helper.notify();

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

    const auto configTree = ::fwRuntime::Convert::toPropertyTree(this->getConfiguration());

    if(configTree.find("config") != configTree.not_found())
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SMaterial::stopping()
{
    m_materialFw.reset();
    m_textureConnection.disconnect();
    this->unregisterServices();

    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);
    if(material->getField("shaderParameters"))
    {
        material->removeField("shaderParameters");
    }
}

//------------------------------------------------------------------------------

void SMaterial::updating()
{
    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);

    if(m_r2vbObject)
    {
        m_materialFw->setPrimitiveType(m_r2vbObject->getInputPrimitiveType());
    }

    // Set up representation mode
    m_materialFw->updatePolygonMode( material->getRepresentationMode() );
    m_materialFw->updateOptionsMode( material->getOptionsMode() );
    m_materialFw->updateShadingMode( material->getShadingMode(), this->getLayer()->getLightsNumber(),
                                     this->hasDiffuseTexture(), m_texAdaptor ? m_texAdaptor->getUseAlpha() : false);
    m_materialFw->updateRGBAMode( material );
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::updateField( ::fwData::Object::FieldsContainerType fields )
{
    for (auto elt : fields)
    {
        if (elt.first == "ogreMaterial")
        {
            ::fwData::Material::sptr material = this->getObject < ::fwData::Material >();
            ::fwData::String::csptr string    = ::fwData::String::dynamicCast(elt.second);
            this->setMaterialTemplateName(string->getValue());

            this->unregisterServices("::visuOgreAdaptor::SShaderParameter");
            if(material->getField("shaderParameters"))
            {
                material->removeField("shaderParameters");
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
    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);
    m_materialFw->updateShadingMode( material->getShadingMode(), this->getLayer()->getLightsNumber(),
                                     this->hasDiffuseTexture(), m_texAdaptor->getUseAlpha() );

    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::createTextureAdaptor()
{
    SLM_ASSERT("Texture adaptor already configured in XML", m_textureName.empty());

    ::fwData::Material::sptr f4sMaterial = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);

    // If the associated material has a texture, we have to create a texture adaptor to handle it
    if(f4sMaterial->getDiffuseTexture())
    {
        // Creates an Ogre adaptor and associates it with the f4s texture object
        auto texture = f4sMaterial->getDiffuseTexture();
        m_texAdaptor = this->registerService< ::visuOgreAdaptor::STexture >("::visuOgreAdaptor::STexture");
        m_texAdaptor->registerInput(texture, "image", true);

        m_texAdaptor->setID(this->getID() + "_" + m_texAdaptor->getID());
        m_texAdaptor->setRenderService(this->getRenderService());
        m_texAdaptor->setLayerID(m_layerID);

        const std::string materialName = f4sMaterial->getID();
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
    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);
    m_materialFw->updateShadingMode( material->getShadingMode(), this->getLayer()->getLightsNumber(),
                                     this->hasDiffuseTexture(), false );

    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMaterial::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_MATERIAL_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_MATERIAL_INOUT, ::fwData::Object::s_ADDED_FIELDS_SIG, s_UPDATE_FIELD_SLOT );
    connections.push( s_MATERIAL_INOUT, ::fwData::Object::s_CHANGED_FIELDS_SIG, s_UPDATE_FIELD_SLOT );
    connections.push( s_MATERIAL_INOUT, ::fwData::Material::s_ADDED_TEXTURE_SIG, s_ADD_TEXTURE_SLOT );
    connections.push( s_MATERIAL_INOUT, ::fwData::Material::s_REMOVED_TEXTURE_SIG, s_REMOVE_TEXTURE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor
