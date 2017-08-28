/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SMaterial.hpp"

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SShaderParameter.hpp"
#include "visuOgreAdaptor/STexture.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Field.hpp>

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreVector3.h>

#include <string>

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SMaterial, ::fwData::Material );

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SMaterial::s_UPDATE_FIELD_SLOT   = "updateField";
const ::fwCom::Slots::SlotKeyType SMaterial::s_SWAP_TEXTURE_SLOT   = "swapTexture";
const ::fwCom::Slots::SlotKeyType SMaterial::s_ADD_TEXTURE_SLOT    = "addTexture";
const ::fwCom::Slots::SlotKeyType SMaterial::s_REMOVE_TEXTURE_SLOT = "removeTexture";

static const std::string s_MATERIAL_INOUT = "material";

//-----------------------------------------------------------------------------

const std::string SMaterial::DEFAULT_MATERIAL_TEMPLATE_NAME = "Default";

static const std::string s_EDGE_PASS    = "EdgePass";
static const std::string s_NORMALS_PASS = "NormalsPass";

//------------------------------------------------------------------------------

SMaterial::SMaterial() noexcept :
    m_materialTemplateName(DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_hasMeshNormal(true),
    m_hasVertexColor(false),
    m_hasPrimitiveColor(false),
    m_primitiveType(::fwData::Mesh::TRIANGLE),
    m_meshBoundingBox(::Ogre::Vector3::ZERO, ::Ogre::Vector3::ZERO),
    m_normalLengthFactor(0.1f),
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

void SMaterial::loadMaterialParameters()
{
    // We retrieve the parameters of the base material in a temporary material
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_materialTemplateName);

    SLM_ASSERT( "Material '" + m_materialTemplateName + "'' not found", !material.isNull() );

    // Then we copy these parameters in m_material.
    // We can now alter this new instance without changing the default material
    material.get()->copyDetailsTo(m_material);

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
            config.add("service.config.<xmlattr>.layer", m_layerID);
            config.add("service.config.<xmlattr>.parameter", constantName);
            config.add("service.config.<xmlattr>.shaderType", shaderTypeStr);
            config.add("service.config.<xmlattr>.materialName", m_materialName);

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

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

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

    if(config.count("normalLength"))
    {
        std::string stringLength = config.get<std::string>("normalLength");
        m_normalLengthFactor = static_cast< ::Ogre::Real >(std::stof(stringLength));
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

    m_material = ::Ogre::MaterialManager::getSingleton().create(
        m_materialName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ::fwData::String::sptr string = ::fwData::String::New();
    string->setValue(m_materialTemplateName);

    ::fwDataTools::helper::Field helper(material);
    helper.setField("ogreMaterial", string);
    helper.notify();

    this->loadMaterialParameters();

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

    this->updating();
}

//------------------------------------------------------------------------------

void SMaterial::stopping()
{
    m_material.setNull();
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

    // Set up representation mode
    this->updatePolygonMode( material->getRepresentationMode() );
    this->updateOptionsMode( material->getOptionsMode() );
    this->updateShadingMode( material->getShadingMode() );
    this->updateRGBAMode( material );
    this->requestRender();
}

//------------------------------------------------------------------------------

::Ogre::Real SMaterial::computeNormalLength()
{
    const ::Ogre::Vector3 meshBBoxSize = m_meshBoundingBox.getSize();
    const ::Ogre::Real averageSize     = ( meshBBoxSize.x + meshBBoxSize.y + meshBBoxSize.z ) /
                                         static_cast< ::Ogre::Real >(3.0);
    return averageSize * m_normalLengthFactor;
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
            this->loadMaterialParameters();
            this->updating();
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::swapTexture()
{
    SLM_ASSERT("Missing texture adaptor", m_texAdaptor);

    ::Ogre::TexturePtr currentTexture = m_texAdaptor->getTexture();
    SLM_ASSERT("Texture not set in Texture adaptor", !currentTexture.isNull());

    this->cleanTransparencyTechniques();

    ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();
    while( techIt.hasMoreElements())
    {
        ::Ogre::Technique* technique = techIt.getNext();
        SLM_ASSERT("Technique is not set", technique);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*technique))
        {
            ::Ogre::Pass* pass                     = technique->getPass(0);
            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("diffuseTexture");

            if(texUnitState)
            {
                texUnitState->setTexture(currentTexture);
            }
            else
            {
                SLM_ERROR("No 'diffuseTexture' texture unit state found in .material."
                          " STexture adaptor will not work properly.");
            }
        }
    }

    // Update the shaders
    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);
    this->updateShadingMode( material->getShadingMode() );

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

        std::string materialName = f4sMaterial->getID();
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

    ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();
    while( techIt.hasMoreElements())
    {
        ::Ogre::Technique* technique = techIt.getNext();
        SLM_ASSERT("Technique is not set", technique);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*technique))
        {
            ::Ogre::Pass* pass                     = technique->getPass(0);
            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("diffuseTexture");
            if(texUnitState)
            {
                texUnitState->setTextureName("");
            }
        }
    }

    m_textureConnection.disconnect();
    this->unregisterServices("::visuOgreAdaptor::STexture");
    m_texAdaptor.reset();

    // Update the shaders
    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);
    this->updateShadingMode( material->getShadingMode() );

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

//------------------------------------------------------------------------------

void SMaterial::updateOptionsMode(int optionsMode)
{
    this->cleanTransparencyTechniques();
    // First remove the normals pass if there is already one
    this->removePass(s_NORMALS_PASS);

    ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();
    const ::Ogre::Real normalLength = this->computeNormalLength();

    if(optionsMode != ::fwData::Material::STANDARD)
    {
        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* currentTechnique = techIt.getNext();

            // We need the first pass of the current technique in order to copy its rendering states in the normals pass
            ::Ogre::Pass* firstPass = currentTechnique->getPass(0);
            SLM_ASSERT("Pass is null", firstPass);

            const bool depthOnly = ::fwRenderOgre::helper::Shading::isDepthOnlyTechnique(*currentTechnique);

            if( ::fwRenderOgre::helper::Shading::isGeometricTechnique(*currentTechnique) || depthOnly )
            {
                // We copy the first pass, thus keeping all rendering states
                ::Ogre::Pass* normalsPass = currentTechnique->createPass();
                *normalsPass              = *firstPass;
                normalsPass->setName(s_NORMALS_PASS);

                // Vertex shader
                normalsPass->setVertexProgram("Default/Normal_VP");

                std::string gpName = depthOnly ? "DepthPeeling/depthMap/" : "";
                gpName += (optionsMode == ::fwData::Material::NORMALS) ?
                          "VerticesNormalsDisplay_GP" :
                          "CellsNormalsDisplay_GP";

                normalsPass->setGeometryProgram(gpName);

                if(!depthOnly)
                {
                    std::string fpName = normalsPass->getFragmentProgramName();
                    fpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(fpName, "Edge_Normal");
                    normalsPass->setFragmentProgram(fpName);
                }

                // Updates the normal length according to the bounding box's size
                normalsPass->getGeometryProgramParameters()->setNamedConstant("u_normalLength", normalLength);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::updatePolygonMode(int polygonMode)
{
    // This is necessary to load and compile the material, otherwise the following technique iterator
    // is null when we call this method on the first time (from starting() for instance)
    m_material->touch();

    this->cleanTransparencyTechniques();

    // First remove a previous normal pass if it exists
    this->removePass(s_EDGE_PASS);

    ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();

    if(polygonMode == ::fwData::Material::EDGE)
    {
        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            ::Ogre::Pass* firstPass = tech->getPass(0);
            SLM_ASSERT("No pass found", firstPass);

            firstPass->setPolygonMode(::Ogre::PM_SOLID);
            firstPass->setPointSpritesEnabled(false);

            ::Ogre::Pass* edgePass = tech->getPass(s_EDGE_PASS);
            if(!edgePass)
            {
                // We copy the first pass, thus keeping all rendering states
                edgePass  = tech->createPass();
                *edgePass = *firstPass;
                edgePass->setName(s_EDGE_PASS);

                // Then we switch the vertex shader...
                edgePass->setVertexProgram("Default/Edge_VP");

                // ... and the fragment shader
                std::string fpName = edgePass->getFragmentProgramName();
                fpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(fpName, "Edge_Normal");
                edgePass->setFragmentProgram(fpName);

                edgePass->setPolygonMode(::Ogre::PM_WIREFRAME);
            }
        }
    }
    else
    {
        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            ::Ogre::Technique::PassIterator passIt = tech->getPassIterator();

            while ( passIt.hasMoreElements() )
            {
                ::Ogre::Pass* ogrePass = passIt.getNext();

                switch( polygonMode )
                {
                    case ::fwData::Material::SURFACE:
                        ogrePass->setPolygonMode(::Ogre::PM_SOLID );
                        ogrePass->setPointSpritesEnabled(false);
                        break;

                    case ::fwData::Material::WIREFRAME:
                        ogrePass->setPolygonMode(::Ogre::PM_WIREFRAME);
                        break;

                    case ::fwData::Material::POINT:
                        ogrePass->setPolygonMode(::Ogre::PM_POINTS);
                        ogrePass->setPointSpritesEnabled(false);
                        ogrePass->setPointSize(1.f);
                        break;

                    default:
                        if( polygonMode != ::fwData::Material::EDGE )
                        {
                            SLM_ASSERT("Unhandled material representation mode : " << polygonMode, false );
                        }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::updateShadingMode( int shadingMode  )
{
    ::fwData::Material::ShadingType mode = static_cast< ::fwData::Material::ShadingType >(shadingMode);

    bool updateLightsNumber(false);

    if(mode != ::fwData::Material::AMBIENT)
    {
        int currentLightsNumber = this->getLayer()->getLightsNumber();

        // We need to update the number of lights supported in the material if it has changed
        if(m_lightsNumber != currentLightsNumber && currentLightsNumber > 0)
        {
            m_lightsNumber     = currentLightsNumber;
            updateLightsNumber = true;
        }
    }

    ::Ogre::String permutation;
    permutation = ::fwRenderOgre::helper::Shading::getPermutation(mode, this->hasDiffuseTexture(),
                                                                  m_hasVertexColor);
    ::Ogre::String r2vbGSName;
    r2vbGSName = ::fwRenderOgre::helper::Shading::getR2VBGeometryProgramName(m_primitiveType,
                                                                             this->hasDiffuseTexture(),
                                                                             m_hasVertexColor,
                                                                             m_hasPrimitiveColor);

    this->cleanTransparencyTechniques();

    // Iterate through each technique found in the material and switch the shading mode
    ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();
    while( techIt.hasMoreElements())
    {
        ::Ogre::Technique* tech = techIt.getNext();
        SLM_ASSERT("Technique is not set", tech);

        ::Ogre::Technique::PassIterator passIt = tech->getPassIterator();

        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* ogrePass = passIt.getNext();

            // Discard edge pass
            if (ogrePass->getName() == s_EDGE_PASS || ogrePass->getName() == s_NORMALS_PASS )
            {
                continue;
            }

            if(m_primitiveType != ::fwData::Mesh::TRIANGLE || m_hasPrimitiveColor)
            {
                // We need a geometry shader (primitive generation and per-primitive color)
                // and thus we rely on the render to vertex buffer pipeline

                ogrePass->setVertexProgram("R2VB/" + permutation + "_VP");
                ogrePass->setGeometryProgram(r2vbGSName);
                ogrePass->setFragmentProgram("");

                if(m_hasPrimitiveColor)
                {
                    const std::string texUnitName = "PerPrimitiveColor";
                    ::Ogre::TextureUnitState* texUnitState = ogrePass->getTextureUnitState(texUnitName);

                    const auto result = ::Ogre::TextureManager::getSingleton().createOrRetrieve(
                        m_perPrimitiveColorTextureName,
                        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);

                    SLM_ASSERT("Texture should have been created before in SMesh", !result.second);

                    ::Ogre::TexturePtr tex = result.first.dynamicCast< ::Ogre::Texture>();

                    if(texUnitState == nullptr)
                    {
                        OSLM_DEBUG("create unit state: " << m_perPrimitiveColorTextureName);

                        ::Ogre::TextureUnitState* texUnitState = ogrePass->createTextureUnitState();
                        texUnitState->setName(texUnitName);
                        texUnitState->setTexture(tex);
                        texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
                        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

                        const auto unitStateCount = ogrePass->getNumTextureUnitStates();

                        // Unit state is set to 10 in the material file, but the real index is set here
                        // Ogre packs texture unit indices so we can't use spare indices
                        ogrePass->getGeometryProgramParameters()->setNamedConstant("u_colorPrimitiveTexture",
                                                                                   unitStateCount - 1);
                    }

                    // Set size outside the scope of texture creation because the size could vary
                    ::Ogre::Vector2 size(static_cast<float>(tex->getWidth()),
                                         static_cast<float>(tex->getHeight() - 1));
                    ogrePass->getGeometryProgramParameters()->setNamedConstant("u_colorPrimitiveTextureSize", size);
                }
            }
            else
            {
                if(m_materialTemplateName == DEFAULT_MATERIAL_TEMPLATE_NAME)
                {
                    // "Regular" pipeline

                    std::string vpName = ogrePass->getVertexProgramName();
                    vpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(vpName, permutation);
                    ogrePass->setVertexProgram(vpName);

                    std::string fpName = ogrePass->getFragmentProgramName();
                    fpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(fpName, permutation);
                    ogrePass->setFragmentProgram(fpName);

                    const bool colorPass = ::fwRenderOgre::helper::Shading::isColorTechnique(*tech);

                    if(m_texAdaptor)
                    {
                        // Updates the u_hasTextureAlpha flag uniform according to the configuration
                        // of the texture adaptor
                        if(this->hasDiffuseTexture() && colorPass)
                        {
                            int useTextureAlpha = static_cast<int>(m_texAdaptor->getUseAlpha());
                            ogrePass->getFragmentProgramParameters()->setNamedConstant("u_useTextureAlpha",
                                                                                       useTextureAlpha);
                        }
                    }

                    if(updateLightsNumber)
                    {
                        ::Ogre::GpuProgramParametersSharedPtr vp = ogrePass->getVertexProgramParameters();

                        if(vp->_findNamedConstantDefinition("u_numLights"))
                        {
                            vp->setNamedConstant("u_numLights", m_lightsNumber);
                        }

                        if(!ogrePass->getFragmentProgramName().empty())
                        {
                            ::Ogre::GpuProgramParametersSharedPtr fp = ogrePass->getFragmentProgramParameters();

                            if(fp->_findNamedConstantDefinition("u_numLights"))
                            {
                                fp->setNamedConstant("u_numLights", m_lightsNumber);
                            }
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::updateRGBAMode(fwData::Material::sptr fw_material)
{
    //Set up Material colors
    ::fwData::Color::sptr f4sAmbient = fw_material->ambient();
    ::fwData::Color::sptr f4sDiffuse = fw_material->diffuse();

    ::Ogre::ColourValue ambient(f4sAmbient->red(), f4sAmbient->green(), f4sAmbient->blue(), f4sAmbient->alpha());
    m_material->setAmbient(ambient);

    ::Ogre::ColourValue diffuse(f4sDiffuse->red(), f4sDiffuse->green(), f4sDiffuse->blue(), f4sDiffuse->alpha());
    m_material->setDiffuse(diffuse);

    ::Ogre::ColourValue specular(.2f, .2f, .2f, 1.f);
    m_material->setSpecular( specular );
    m_material->setShininess( 25 );
}

//-----------------------------------------------------------------------------

void SMaterial::removePass(const std::string& _name)
{
    SLM_ASSERT("Material is not set", !m_material.isNull());

    ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();

    while( techIt.hasMoreElements())
    {
        ::Ogre::Technique* technique = techIt.getNext();
        SLM_ASSERT("Technique is not set", technique);

        ::Ogre::Technique::PassIterator passIt = technique->getPassIterator();
        std::vector< ::Ogre::Pass* > removePassVector;

        // Collect the passes to remove
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* ogrePass = passIt.getNext();
            if(ogrePass->getName() == _name)
            {
                removePassVector.push_back(ogrePass);
                continue;
            }
        }

        // Perform the removal
        for(auto edgePass : removePassVector)
        {
            technique->removePass(edgePass->getIndex());
        }
    }
}

//-----------------------------------------------------------------------------

void SMaterial::cleanTransparencyTechniques()
{
    SLM_ASSERT("Material is not set", !m_material.isNull());

    ::Ogre::Material::TechniqueIterator techIt = m_material->getTechniqueIterator();

    std::vector< unsigned short > removeTechniqueVector;

    unsigned short index = 0;
    while( techIt.hasMoreElements())
    {
        ::Ogre::Technique* technique = techIt.getNext();
        SLM_ASSERT("Technique is not set", technique);

        auto scheme = technique->getSchemeName();
        if( ::Ogre::StringUtil::startsWith( scheme, "CelShadingDepthPeeling", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "DepthPeeling", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "DualDepthPeeling", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "HybridTransparency", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "WeightedBlended", false) )
        {
            removeTechniqueVector.push_back(index);
        }
        ++index;
    }

    // Remove in inverse order otherwise the index we stored becomes invalid ;-)
    for(auto it = removeTechniqueVector.rbegin(); it != removeTechniqueVector.rend(); ++it )
    {
        m_material->removeTechnique(*it);
    }
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor
