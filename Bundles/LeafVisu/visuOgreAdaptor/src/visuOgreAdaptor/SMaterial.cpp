/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwComEd/helper/Array.hpp>
#include <fwComEd/helper/Field.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwTools/fwID.hpp>

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SShaderParameter.hpp"
#include "visuOgreAdaptor/STexture.hpp"

#include <boost/regex.hpp>
#include <string>

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTextureManager.h>

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SMaterial, ::fwData::Material );

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SMaterial::s_UPDATE_FIELD_SLOT   = "updateField";
const ::fwCom::Slots::SlotKeyType SMaterial::s_SWAP_TEXTURE_SLOT   = "swapTexture";
const ::fwCom::Slots::SlotKeyType SMaterial::s_ADD_TEXTURE_SLOT    = "addTexture";
const ::fwCom::Slots::SlotKeyType SMaterial::s_REMOVE_TEXTURE_SLOT = "removeTexture";

//-----------------------------------------------------------------------------

const std::string SMaterial::DEFAULT_MATERIAL_TEMPLATE_NAME = "1 - Default";

static const std::string s_EDGE_PASS    = "EdgePass";
static const std::string s_NORMALS_PASS = "NormalsPass";

static const std::regex s_PEEL_REGEX(".*_peel.*");
static const std::regex s_WEIGHT_BLEND_REGEX(".*_weight_blend.*");
static const std::regex s_TRANSMITTANCE_BLEND_REGEX(".*_transmittance_blend.*");
static const std::regex s_DEPTH_MAP_REGEX("(.*depth_map.*)|(.*back_depth.*)");

static const std::string s_NONE          = "None";
static const std::string s_FLAT          = "Flat";
static const std::string s_GOURAUD       = "Gouraud";
static const std::string s_PIXELLIGHTING = "PixelLit";

//-----------------------------------------------------------------------------

static bool isColorTechnique(const std::string _name, bool& _peelTech)
{
    const std::regex regexPeel(".*_peel.*");
    const std::regex regexWeight(".*_weight_blend.*");
    const std::regex regexDualPeelInit("Dual.*_peel_init.*");

    _peelTech = std::regex_match(_name, regexPeel);
    const bool weightPass   = std::regex_match(_name, regexWeight);
    const bool peelInitPass = std::regex_match(_name, regexDualPeelInit);

    return _name == "" || (_peelTech && !peelInitPass) || weightPass;
}

//------------------------------------------------------------------------------

SMaterial::SMaterial() throw() :
    m_materialName(""),
    m_materialTemplateName(DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_hasMeshNormal(true),
    m_hasVertexColor(false),
    m_hasPrimitiveColor(false),
    m_meshBoundingBox(::Ogre::Vector3::ZERO, ::Ogre::Vector3::ZERO),
    m_normalLengthFactor(0.1f),
    m_hasQuad(false),
    m_hasTetra(false),
    m_shadingMode(::fwData::Material::SHADING_MODE::MODE_PHONG)
{
    m_textureConnection = ::fwServices::helper::SigSlotConnection::New();

    newSlot(s_UPDATE_FIELD_SLOT, &SMaterial::updateField, this);
    newSlot(s_SWAP_TEXTURE_SLOT, &SMaterial::swapTexture, this);
    newSlot(s_ADD_TEXTURE_SLOT, &SMaterial::createTextureAdaptor, this);
    newSlot(s_REMOVE_TEXTURE_SLOT, &SMaterial::removeTextureAdaptor, this);
}

//------------------------------------------------------------------------------

SMaterial::~SMaterial() throw()
{

}

//------------------------------------------------------------------------------

void SMaterial::loadMaterialParameters()
{
    // We retrieve the parameters of the base material in a temporary material
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_materialTemplateName);

    // Then we copy these parameters in m_material.
    // We can now alter this new instance without changing the default material
    material.get()->copyDetailsTo(m_material);

    ::Ogre::Pass* pass = material->getTechnique(0)->getPass(0);

    // If the material is programmable (ie contains shader programs) create associated ShaderParameter adaptor
    // with the given ::fwData::Object ID
    if (pass->isProgrammable())
    {
        ::Ogre::GpuProgramParametersSharedPtr params;
        // At this time, we have whether a set of ShaderParameter or a Texture adaptor
        this->unregisterServices("::visuOgreAdaptor::SShaderParameter");

        // Getting params for each program type
        if (pass->hasVertexProgram())
        {
            params = pass->getVertexProgramParameters();
            this->loadShaderParameters(params, "vp");
        }
        if (pass->hasFragmentProgram())
        {
            params = pass->getFragmentProgramParameters();
            this->loadShaderParameters(params, "fp");
        }
        if (pass->hasGeometryProgram())
        {
            params = pass->getGeometryProgramParameters();
            this->loadShaderParameters(params, "gp");
        }
        if (pass->hasTessellationHullProgram())
        {
            OSLM_WARN("Tessellation Hull Program in Material not supported yet");
        }
        if (pass->hasTessellationDomainProgram())
        {
            OSLM_WARN("Tessellation Domain Program in Material not supported yet");
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::loadShaderParameters(::Ogre::GpuProgramParametersSharedPtr params, std::string shaderType)
{
    // We first need to check if our constant is related to Ogre or FW4SPL

    // Getting Ogre auto constants
    ::Ogre::GpuProgramParameters::AutoConstantIterator autoConstantsDefinitionIt = params->getAutoConstantIterator();
    // Getting whole constants
    ::Ogre::GpuNamedConstants constantsDefinitionMap = params->getConstantDefinitions();

    // Copy constants map
    ::Ogre::GpuNamedConstants constantsDefinitionOnly = constantsDefinitionMap;

    // Getting only user constants
    for (auto autoCstDef : autoConstantsDefinitionIt)
    {
        ::Ogre::GpuProgramParameters::AutoConstantEntry entry = autoCstDef;
        for (auto cstDef : constantsDefinitionMap.map)
        {
            ::Ogre::GpuConstantDefinition def = cstDef.second;
            // If the physical index is the same, it is an Ogre auto constant
            if (entry.physicalIndex == def.physicalIndex)
            {
                // Then remove it from the copied map
                constantsDefinitionOnly.map.erase(cstDef.first);
            }
        }
    }

    // Create a new ::fwData::Object for each Ogre user constant
    for (auto keyVal : constantsDefinitionOnly.map)
    {
        // We also need to check if the paramName doesn't ends with [0] (when using arrays).
        // Ogre defines no-array variable twince : var and var[0]
        // warning with GCC compiler: unknown escape sequence: '\]'
        ::Ogre::String paramName = keyVal.first;
        const std::regex regexNo0Tab(".*\[0\]", std::regex_constants::basic);
        if(!std::regex_match(paramName, regexNo0Tab))
        {

            // Trying to get FW4SPL object corresponding to paramName
            ::fwData::Object::sptr obj;
            std::string objName = this->getObject()->getID() + "_" + shaderType + "_" + paramName;

            // Check if object exist, else create it with the corresponding type
            obj = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject(objName));
            if (obj == nullptr)
            {
                ::Ogre::GpuConstantDefinition cstDef = keyVal.second;
                obj                                  =
                    this->createObjectFromShaderParameter(cstDef.constType, paramName);
            }
            obj->setName(paramName);

            // Add the object to the shaderParameter composite of the Material
            ::fwData::Material::sptr material   = this->getObject< ::fwData::Material >();
            ::fwData::Composite::sptr composite = material->setDefaultField(
                "shaderParameters", ::fwData::Composite::New());
            (*composite)[paramName] = obj;

            // Create associated ShaderParameter adaptor
            ::fwRenderOgre::IAdaptor::sptr shaderParameterService;

            this->setServiceOnShaderParameter(shaderParameterService, obj, paramName, shaderType);
        }
    }
}

//------------------------------------------------------------------------------

::fwData::Object::sptr SMaterial::createObjectFromShaderParameter(::Ogre::GpuConstantType type, std::string paramName)
{
    ::fwData::Object::sptr object;

    switch(type)
    {
        case ::Ogre::GpuConstantType::GCT_FLOAT1:
            object = ::fwData::Float::New();
            break;
        case ::Ogre::GpuConstantType::GCT_FLOAT2:
        {
            object = ::fwData::Array::New();
            float vec2[2];
            vec2[0]                           = 0.;
            vec2[1]                           = 0.;
            ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(object);
            arrayObject->setType(::fwTools::Type::create< ::fwTools::Type::FloatType>());
            arrayObject->setNumberOfComponents(2);
            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setBuffer(vec2, false, arrayObject->getType(), arrayObject->getSize(), 2);
        }
        break;
        case ::Ogre::GpuConstantType::GCT_FLOAT3:
            object = fwData::Point::New();
            break;
        case ::Ogre::GpuConstantType::GCT_FLOAT4:
            object = fwData::Color::New();
            break;
        case ::Ogre::GpuConstantType::GCT_SAMPLER1D:
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_SAMPLER2D:
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_SAMPLER3D:
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_SAMPLERCUBE:
            object = ::fwData::Integer::New();
        case ::Ogre::GpuConstantType::GCT_SAMPLERRECT:
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_SAMPLER1DSHADOW:
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_SAMPLER2DSHADOW:
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_SAMPLER2DARRAY:
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_MATRIX_4X4:
            object = ::fwData::TransformationMatrix3D::New();
            break;
        case ::Ogre::GpuConstantType::GCT_INT1:
            // TOFIX : Ogre didn't manage glsl boolean type. For now, a f4w boolean can be loaded naming variable
            // boolean as "int Boolean" in the glsl script
            if(paramName=="Boolean")
            {
                object = fwData::Boolean::New();
            }
            else
            {
                object = ::fwData::Integer::New();
            }
            break;
        case ::Ogre::GpuConstantType::GCT_INT2:
        {
            object = ::fwData::Array::New();
            int* vec2[2];
            vec2[0]                           = 0;
            vec2[1]                           = 0;
            ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(object);
            arrayObject->setType(::fwTools::Type::create< ::fwTools::Type::Int32Type>());
            arrayObject->setNumberOfComponents(2);
            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setBuffer(vec2, false, arrayObject->getType(), arrayObject->getSize(), 2);
        }
        break;
        case ::Ogre::GpuConstantType::GCT_INT3:
        {
            object = ::fwData::Array::New();
            int* vec3[3];
            vec3[0]                           = 0;
            vec3[1]                           = 0;
            vec3[2]                           = 0;
            ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(object);
            arrayObject->setType(::fwTools::Type::create< ::fwTools::Type::Int32Type>());
            arrayObject->setNumberOfComponents(3);
            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setBuffer(vec3, false, arrayObject->getType(), arrayObject->getSize(), 3);
        }
        break;
        case ::Ogre::GpuConstantType::GCT_INT4:
        {
            object = ::fwData::Array::New();
            int* vec4[4];
            vec4[0]                           = 0;
            vec4[1]                           = 0;
            vec4[2]                           = 0;
            vec4[3]                           = 0;
            ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(object);
            arrayObject->setType(::fwTools::Type::create< ::fwTools::Type::Int32Type>());
            arrayObject->setNumberOfComponents(4);
            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setBuffer(vec4, false, arrayObject->getType(), arrayObject->getSize(), 4);
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE1:
            object = ::fwData::Float::New();
            break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE2:
        {
            object = ::fwData::Array::New();
            float vec2[2];
            vec2[0]                           = 0.;
            vec2[1]                           = 0.;
            ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(object);
            arrayObject->setType(::fwTools::Type::create< ::fwTools::Type::FloatType>());
            arrayObject->setNumberOfComponents(2);
            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setBuffer(vec2, false, arrayObject->getType(), arrayObject->getSize(), 2);
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE3:
        {
            object = ::fwData::Array::New();
            float vec3[3];
            vec3[0]                           = 0.;
            vec3[1]                           = 0.;
            vec3[2]                           = 0.;
            ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(object);
            arrayObject->setType(::fwTools::Type::create< ::fwTools::Type::FloatType>());
            arrayObject->setNumberOfComponents(3);
            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setBuffer(vec3, false, arrayObject->getType(), arrayObject->getSize(), 3);
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE4:
        {
            object = ::fwData::Array::New();
            float vec4[4];
            vec4[0]                           = 0.;
            vec4[1]                           = 0.;
            vec4[2]                           = 0.;
            vec4[3]                           = 0.;
            ::fwData::Array::sptr arrayObject = ::fwData::Array::dynamicCast(object);
            arrayObject->setType(::fwTools::Type::create< ::fwTools::Type::FloatType>());
            arrayObject->setNumberOfComponents(4);
            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setBuffer(vec4, false, arrayObject->getType(), arrayObject->getSize(), 4);
        }
        break;
        case ::Ogre::GpuConstantType::GCT_MATRIX_DOUBLE_4X4:
            object = ::fwData::TransformationMatrix3D::New();
            break;
        default:
            std::string GpuConstantTypeNames[] =
            {
                "GCT_FLOAT1",
                "GCT_FLOAT2",
                "GCT_FLOAT3",
                "GCT_FLOAT4",
                "GCT_SAMPLER1D",
                "GCT_SAMPLER2D",
                "GCT_SAMPLER3D",
                "GCT_SAMPLERCUBE",
                "GCT_SAMPLERRECT",
                "GCT_SAMPLER1DSHADOW",
                "GCT_SAMPLER2DSHADOW",
                "GCT_SAMPLER2DARRAY",
                "GCT_MATRIX_2X2",
                "GCT_MATRIX_2X3",
                "GCT_MATRIX_2X4",
                "GCT_MATRIX_3X2",
                "GCT_MATRIX_3X3",
                "GCT_MATRIX_3X4",
                "GCT_MATRIX_4X2",
                "GCT_MATRIX_4X3",
                "GCT_MATRIX_4X4",
                "GCT_INT1",
                "GCT_INT2",
                "GCT_INT3",
                "GCT_INT4",
                "GCT_SUBROUTINE",
                "GCT_DOUBLE1",
                "GCT_DOUBLE2",
                "GCT_DOUBLE3",
                "GCT_DOUBLE4",
                "GCT_MATRIX_DOUBLE_2X2",
                "GCT_MATRIX_DOUBLE_2X3",
                "GCT_MATRIX_DOUBLE_2X4",
                "GCT_MATRIX_DOUBLE_3X2",
                "GCT_MATRIX_DOUBLE_3X3",
                "GCT_MATRIX_DOUBLE_3X4",
                "GCT_MATRIX_DOUBLE_4X2",
                "GCT_MATRIX_DOUBLE_4X3",
                "GCT_MATRIX_DOUBLE_4X4",
                "GCT_UNKNOWN"
            };
            OSLM_FATAL("Object type "+GpuConstantTypeNames[type-1]+" not supported yet");
    }
    return object;
}

//------------------------------------------------------------------------------

void SMaterial::setServiceOnShaderParameter(::fwRenderOgre::IAdaptor::sptr& srv,
                                            std::shared_ptr< ::fwData::Object > object, std::string paramName,
                                            std::string shaderType)
{
    if(!srv)
    {
        // Creates an Ogre adaptor and associates it with the f4s object
        srv = ::fwServices::add< ::fwRenderOgre::IAdaptor >(object, "::visuOgreAdaptor::SShaderParameter");
        SLM_ASSERT("Unable to instanciate shader service", srv);
        ::visuOgreAdaptor::SShaderParameter::sptr shaderParamService = ::visuOgreAdaptor::SShaderParameter::dynamicCast(
            srv);

        // Naming convention for shader parameters
        shaderParamService->setID(this->getID() +"_"+ shaderParamService->getID() + "-" + shaderType + "-" + paramName);
        // FIXME m_layerID always ""
        shaderParamService->setLayerID(m_layerID);
        // Same render service as its parent
        shaderParamService->setRenderService(this->getRenderService());
        shaderParamService->setMaterialName(m_materialName);
        shaderParamService->setParamName(paramName);
        shaderParamService->setShaderType(shaderType);

        shaderParamService->start();
        shaderParamService->update();

        // Add created subservice to current service
        this->registerService(shaderParamService);
    }
    else if(srv->getObject() != object)
    {
        srv->swap(object);
    }
}

//------------------------------------------------------------------------------

void SMaterial::setTextureAdaptor(const std::string& textureAdaptorUID)
{
    if(textureAdaptorUID.empty())
    {
        m_texAdaptor = nullptr;
    }
    else
    {
        m_texAdaptorUID = textureAdaptorUID;

        auto textureService = ::fwServices::get(m_texAdaptorUID);
        m_texAdaptor = ::visuOgreAdaptor::STexture::dynamicCast(textureService);
    }
}


//------------------------------------------------------------------------------

void SMaterial::doConfigure() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("Not a \"config\" configuration", m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("materialTemplate"))
    {
        m_materialTemplateName = m_configuration->getAttributeValue("materialTemplate");
    }

    if(m_configuration->hasAttribute("materialName"))
    {
        m_materialName = m_configuration->getAttributeValue("materialName");
    }

    if(m_configuration->hasAttribute("textureAdaptor"))
    {
        m_texAdaptorUID = m_configuration->getAttributeValue("textureAdaptor");
    }

    if(m_configuration->hasAttribute("shadingMode"))
    {
        std::string shadingMode = m_configuration->getAttributeValue("shadingMode");

        if(shadingMode != "ambient")
        {
            m_shadingMode = ::fwData::Material::SHADING_MODE::MODE_AMBIENT;
        }
        else if(shadingMode == "flat")
        {
            m_shadingMode = ::fwData::Material::SHADING_MODE::MODE_FLAT;
        }
        else if(shadingMode == "gouraud")
        {
            m_shadingMode = ::fwData::Material::SHADING_MODE::MODE_GOURAUD;
        }
        else
        {
            m_shadingMode = ::fwData::Material::SHADING_MODE::MODE_PHONG;
        }
    }

    if(m_configuration->hasAttribute("normalLength"))
    {
        std::string stringLength = m_configuration->getAttributeValue("normalLength");
        m_normalLengthFactor = static_cast< ::Ogre::Real >(std::stof(stringLength));
    }
}

//------------------------------------------------------------------------------

void SMaterial::doStart() throw(fwTools::Failed)
{
    ::fwData::Material::sptr material = this->getObject < ::fwData::Material >();

    material->setShadingMode(m_shadingMode);

    m_material = ::Ogre::MaterialManager::getSingleton().create(
        m_materialName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    this->loadMaterialParameters();

    // A texture adaptor is configured in the XML scene, we can retrieve it
    if(!m_texAdaptorUID.empty())
    {
        if(!m_texAdaptor)
        {
            this->setTextureAdaptor(m_texAdaptorUID);
        }

        if(m_texAdaptor->getTextureName().empty())
        {
            m_texAdaptor->setRenderService(this->getRenderService());
            m_texAdaptor->setLayerID(m_layerID);
        }

        m_textureConnection->connect(m_texAdaptor, ::visuOgreAdaptor::STexture::s_TEXTURE_SWAPPED_SIG, this->getSptr(),
                                     ::visuOgreAdaptor::SMaterial::s_SWAP_TEXTURE_SLOT);

        this->swapTexture();
    }
    else
    {
        this->createTextureAdaptor();
    }

    this->updateSchemeSupport();
}

//------------------------------------------------------------------------------

void SMaterial::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE("SWAPPING Material");
    this->unregisterServices("::visuOgreAdaptor::SShaderParameter");
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SMaterial::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Material::sptr material = this->getObject < ::fwData::Material >();

    // Set up representation mode
    this->updatePolygonMode( material->getRepresentationMode() );
    this->updateOptionsMode( material->getOptionsMode() );
    this->updateShadingMode( material->getShadingMode() );
    this->updateRGBAMode( material );
    this->updateSchemeSupport();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::updateSchemeSupport()
{
    m_schemesSupported.clear();
    ::Ogre::Material::TechniqueIterator tech_iter = m_material->getTechniqueIterator();
    while( tech_iter.hasMoreElements())
    {
        ::Ogre::String techSchemeName = tech_iter.getNext()->getSchemeName();
        m_schemesSupported.push_back(techSchemeName);
    }
    ::fwRenderOgre::Layer::sptr currentLayer = this->getRenderService()->getLayer(m_layerID);
    if(currentLayer->isDefaultCompositorEnabled())
    {
        currentLayer->getDefaultCompositor()->updateTechniquesSupported(m_materialName, m_schemesSupported);
        currentLayer->getDefaultCompositor()->update();
    }
}

//------------------------------------------------------------------------------

::Ogre::Real SMaterial::computeNormalLength()
{
    ::Ogre::Vector3 meshBBoxSize = m_meshBoundingBox.getSize();
    ::Ogre::Real averageSize     = ( meshBBoxSize.x + meshBBoxSize.y + meshBBoxSize.z ) /
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
            this->loadMaterialParameters();

            this->requestRender();
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::swapTexture()
{
    SLM_ASSERT("Missing texture adaptor", m_texAdaptor);

    this->getRenderService()->makeCurrent();

    ::Ogre::TexturePtr currentTexture = m_texAdaptor->getTexture();
    SLM_ASSERT("Texture not set in Texture adaptor", !currentTexture.isNull());

    bool bEmptyTexture = (currentTexture->getSrcWidth() == 0 && currentTexture->getSrcHeight() == 0);

    ::Ogre::Material::TechniqueIterator tech_iter = m_material->getTechniqueIterator();
    while( tech_iter.hasMoreElements())
    {
        ::Ogre::Technique* technique = tech_iter.getNext();

        bool peelTech = false;
        if(isColorTechnique(technique->getName(), peelTech ))
        {
            ::Ogre::Pass* pass                     = technique->getPass(0);
            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("diffuseTexture");

            if(bEmptyTexture)
            {
                if(texUnitState)
                {
                    // If the texture is empty, we remove the texture unit state
                    auto texUnitStateIndex = pass->getTextureUnitStateIndex(texUnitState);
                    pass->removeTextureUnitState(texUnitStateIndex);
                    continue;
                }
            }

            if(!texUnitState)
            {
                texUnitState = pass->createTextureUnitState();
                texUnitState->setName("diffuseTexture");
                texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
                texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_WRAP);
            }
            texUnitState->setTexture(currentTexture);
        }
    }

    // Update the shaders
    ::fwData::Material::sptr material = this->getObject< ::fwData::Material >();
    this->updateShadingMode( material->getShadingMode() );

    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::doStop() throw(fwTools::Failed)
{
    m_textureConnection->disconnect();
    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SMaterial::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Object::s_ADDED_FIELDS_SIG, s_UPDATE_FIELD_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Object::s_CHANGED_FIELDS_SIG, s_UPDATE_FIELD_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Material::s_ADDED_TEXTURE_SIG, s_ADD_TEXTURE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Material::s_REMOVED_TEXTURE_SIG, s_REMOVE_TEXTURE_SLOT ) );
    return connections;
}

//------------------------------------------------------------------------------

void SMaterial::updateOptionsMode(int optionsMode)
{
    ::Ogre::Material::TechniqueIterator tech_iter = m_material->getSupportedTechniqueIterator();
    ::Ogre::Real normalLength                     = this->computeNormalLength();

    while( tech_iter.hasMoreElements())
    {
        ::Ogre::Technique* currentTechnique       = tech_iter.getNext();
        ::Ogre::Technique::PassIterator pass_iter = currentTechnique->getPassIterator();

        std::vector< ::Ogre::Pass* > normalPassVector;

        while ( pass_iter.hasMoreElements() )
        {
            ::Ogre::Pass* ogrePass = pass_iter.getNext();
            if(ogrePass->getName() == s_NORMALS_PASS)
            {
                normalPassVector.push_back(ogrePass);
            }
        }

        // We have to remove the normal passes
        for(auto normalPass : normalPassVector)
        {
            currentTechnique->removePass(normalPass->getIndex());
        }

        // If we are in standard mode (no normals), we don't have to create a normals pass
        if( optionsMode == ::fwData::Material::OPTIONS_MODE::MODE_STANDARD)
        {
            continue;
        }

        // We need the first pass of the current technique in order to copy its rendering states in the normals pass
        ::Ogre::Pass* firstPass = currentTechnique->getPass(0);

        const bool peelPass           = std::regex_match(currentTechnique->getName(), s_PEEL_REGEX);
        const bool weightBlend        = std::regex_match(currentTechnique->getName(), s_WEIGHT_BLEND_REGEX);
        const bool transmittanceBlend = std::regex_match(currentTechnique->getName(), s_TRANSMITTANCE_BLEND_REGEX);
        const bool depthMap           = std::regex_match(currentTechnique->getName(), s_DEPTH_MAP_REGEX);

        if( (currentTechnique->getName() == "" || peelPass || weightBlend || transmittanceBlend || depthMap) )
        {
            // We copy the first pass, thus keeping all rendering states
            ::Ogre::Pass* normalsPass = currentTechnique->createPass();
            *normalsPass              = *firstPass;
            normalsPass->setName(s_NORMALS_PASS);

            // Vertex shader
            normalsPass->setVertexProgram("Default_Normal_VP_glsl");

            // Geometry shader (for point or cell normals)
            if(optionsMode == ::fwData::Material::OPTIONS_MODE::MODE_NORMALS)
            {
                if(depthMap)
                {
                    normalsPass->setGeometryProgram("DepthPeeling_depth_map_VerticesNormalsDisplay_GP");
                }
                else
                {
                    normalsPass->setGeometryProgram("VerticesNormalsDisplay_GP");
                }
            }
            else
            {
                if(depthMap)
                {
                    normalsPass->setGeometryProgram("DepthPeeling_depth_map_CellsNormalsDisplay_GP");
                }
                else
                {
                    normalsPass->setGeometryProgram("CellsNormalsDisplay_GP");
                }
            }

            if(!depthMap)
            {
                auto fpName = normalsPass->getFragmentProgramName();

                // Clear the suffix (+VT+...)
                const ::boost::regex regexConcat("\\N{plus-sign}.*(_[FV]P_glsl)", ::boost::regex::extended);
                fpName = ::boost::regex_replace(fpName, regexConcat, "$1");

                // Fragment shader
                // We use a specific version of this shader when we use an OIT technique
                // (mainly to force the diffuse color because we don't manage to set uniforms with compositors usage)
                const ::boost::regex regexShading("("+s_FLAT+")|("+s_GOURAUD+")|("+s_PIXELLIGHTING+")");
                fpName = ::boost::regex_replace(fpName, regexShading, "Edge_Normal");

                const ::boost::regex regexTech("(peel_init)|(transmittance_blend)");
                fpName = ::boost::regex_replace(fpName, regexTech, "$&_Edge_Normal");

                normalsPass->setFragmentProgram(fpName);
            }

            // Updates the normal length according to the bounding box's size
            normalsPass->getGeometryProgramParameters()->setNamedConstant("u_normalLength", normalLength);
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::updatePolygonMode(int polygonMode)
{
    // This is necessary to load and compile the material, otherwise the following technique iterator
    // is null when we call this method on the first time (from doStart() for instance)
    m_material->touch();

    ::Ogre::Material::TechniqueIterator tech_iter = m_material->getSupportedTechniqueIterator();

    if(polygonMode == ::fwData::Material::MODE_EDGE)
    {
        while( tech_iter.hasMoreElements())
        {
            ::Ogre::Technique* tech = tech_iter.getNext();

            const bool peelPass           = std::regex_match(tech->getName(), s_PEEL_REGEX);
            const bool weightBlend        = std::regex_match(tech->getName(), s_WEIGHT_BLEND_REGEX);
            const bool transmittanceBlend = std::regex_match(tech->getName(), s_TRANSMITTANCE_BLEND_REGEX);

            if(tech->getName() == "" || peelPass || weightBlend || transmittanceBlend)
            {
                ::Ogre::Technique::PassIterator pass_iter = tech->getPassIterator();
                ::Ogre::Pass* firstPass                   = pass_iter.getNext();

                firstPass->setPolygonMode(::Ogre::PM_SOLID);

                ::Ogre::Pass* edgePass = tech->getPass(s_EDGE_PASS);
                if(!edgePass)
                {
                    // We copy the first pass, thus keeping all rendering states
                    edgePass  = tech->createPass();
                    *edgePass = *firstPass;
                    edgePass->setName(s_EDGE_PASS);

                    // We then switch the vertex shader...
                    edgePass->setVertexProgram("Default_Edge_VP_glsl");

                    // ... and the fragment shader
                    auto fpName = edgePass->getFragmentProgramName();

                    // Clear the suffix in shader names  (+VT+...)
                    const ::boost::regex regexConcat("\\N{plus-sign}.*(_[FV]P_glsl)", ::boost::regex::extended);
                    fpName = ::boost::regex_replace(fpName, regexConcat, "$1");

                    // For the latter, we use a specific version when we use an OIT technique
                    // (mainly to force the diffuse color because we don't manage to set uniforms with compositors usage)
                    const ::boost::regex regexShading("("+s_FLAT+")|("+s_GOURAUD+")|("+s_PIXELLIGHTING+")");
                    fpName = ::boost::regex_replace(fpName, regexShading, "Edge_Normal" );

                    const ::boost::regex regexTech("(peel_init)|(transmittance_blend)");
                    fpName = ::boost::regex_replace(fpName, regexTech, "$&_Edge_Normal" );

                    edgePass->setFragmentProgram(fpName);

                    edgePass->setPolygonMode(::Ogre::PM_WIREFRAME);
                }
            }
        }
    }
    else
    {
        while( tech_iter.hasMoreElements())
        {
            ::Ogre::Technique* tech                   = tech_iter.getNext();
            ::Ogre::Technique::PassIterator pass_iter = tech->getPassIterator();
            std::vector< ::Ogre::Pass* > edgePassVector;

            while ( pass_iter.hasMoreElements() )
            {
                ::Ogre::Pass* ogrePass = pass_iter.getNext();
                if(ogrePass->getName() == s_EDGE_PASS)
                {
                    edgePassVector.push_back(ogrePass);
                    continue;
                }

                switch( polygonMode )
                {

                    case ::fwData::Material::MODE_SURFACE:
                        ogrePass->setPolygonMode(::Ogre::PM_SOLID );
                        ogrePass->setPointSpritesEnabled(false);
                        break;

                    case ::fwData::Material::MODE_WIREFRAME:
                        ogrePass->setPolygonMode(::Ogre::PM_WIREFRAME);
                        break;

                    case ::fwData::Material::MODE_POINT:
                        ogrePass->setPolygonMode(::Ogre::PM_POINTS);
                        ogrePass->setPointSpritesEnabled(false);
                        ogrePass->setPointSize(1.f);
                        break;

                    default:
                        if( polygonMode != ::fwData::Material::MODE_EDGE )
                        {
                            SLM_ASSERT("Unhandled material representation mode : "
                                       << polygonMode, false );
                        }
                }
            }

            // We have to remove the edge passes
            for(auto edgePass : edgePassVector)
            {
                tech->removePass(edgePass->getIndex());
            }
        }
    }
}

//------------------------------------------------------------------------------

void SMaterial::updateShadingMode( int shadingMode  )
{
    if(m_materialTemplateName == DEFAULT_MATERIAL_TEMPLATE_NAME)
    {
        // Choose the shading mode string
        ::Ogre::String shadingProgramSuffix;

        switch(shadingMode)
        {
            case ::fwData::Material::SHADING_MODE::MODE_AMBIENT:
                shadingProgramSuffix = s_NONE;
                break;
            case ::fwData::Material::SHADING_MODE::MODE_FLAT:
                shadingProgramSuffix = s_FLAT;
                break;
            case ::fwData::Material::SHADING_MODE::MODE_GOURAUD:
                shadingProgramSuffix = s_GOURAUD;
                break;
            case ::fwData::Material::SHADING_MODE::MODE_PHONG:
                shadingProgramSuffix = s_PIXELLIGHTING;
                break;
            default:
                SLM_ERROR("Unknown shading mode. ");
        }

        ::Ogre::String shadingCfgs;
        if(m_hasVertexColor)
        {
            shadingCfgs += "+VT";
        }
        if(this->hasDiffuseTexture())
        {
            shadingCfgs += "+DfsTex";
        }

        std::string shadingProgramSuffixGS;
        if(m_hasQuad)
        {
            shadingProgramSuffixGS = "Quad";
        }
        else if(m_hasTetra)
        {
            shadingProgramSuffixGS = "Tetra";
        }
        else
        {
            shadingProgramSuffixGS = "Triangles";
        }

        shadingProgramSuffix   += shadingCfgs;
        shadingProgramSuffixGS += shadingCfgs;

        if(m_hasPrimitiveColor)
        {
            shadingProgramSuffixGS += "+PPColor";
        }

        // Iterate through each technique found in the material and switch the shading mode
        ::Ogre::Material::TechniqueIterator tech_iter = m_material->getTechniqueIterator();
        while( tech_iter.hasMoreElements())
        {
            ::Ogre::Technique* tech                   = tech_iter.getNext();
            ::Ogre::Technique::PassIterator pass_iter = tech->getPassIterator();

            ::Ogre::Pass* ogrePass;
            while ( pass_iter.hasMoreElements() )
            {
                ogrePass = pass_iter.getNext();

                // Discard edge pass
                if (ogrePass->getName() == s_EDGE_PASS || ogrePass->getName() == s_NORMALS_PASS )
                {
                    continue;
                }

                std::string ogreVertexName   = ogrePass->getVertexProgramName();
                std::string ogreFragmentName = ogrePass->getFragmentProgramName();

                // Check if we need a geometry shader (primitive generation and per-primitive color)
                if(m_hasQuad || m_hasTetra || m_hasPrimitiveColor)
                {
                    ogrePass->setVertexProgram("RenderScene_R2VB_" + shadingProgramSuffix + "_VP_glsl");
                    ogrePass->setGeometryProgram("RenderScene_" + shadingProgramSuffixGS + "_GP_glsl");
                    ogrePass->setFragmentProgram("");
                }
                else
                {
                    // Clear the suffix in shader names (+VT+...)
                    const ::boost::regex regexConcat("\\N{plus-sign}.*(_[FV]P_glsl)", ::boost::regex::extended);
                    ogreVertexName   = ::boost::regex_replace(ogreVertexName, regexConcat, "$1");
                    ogreFragmentName = ::boost::regex_replace(ogreFragmentName, regexConcat, "$1");

                    // Replace the shading technique
                    const ::boost::regex regexShading("("+s_NONE+")|("+s_FLAT+")|("+s_GOURAUD+")|("+s_PIXELLIGHTING+
                                                      ")");
                    ogreVertexName   = ::boost::regex_replace(ogreVertexName, regexShading, shadingProgramSuffix);
                    ogreFragmentName = ::boost::regex_replace(ogreFragmentName, regexShading, shadingProgramSuffix);

                    ogrePass->setVertexProgram(ogreVertexName);
                    ogrePass->setFragmentProgram(ogreFragmentName);
                }

                if(m_texAdaptor)
                {
                    bool peelPass;
                    bool colorPass = isColorTechnique(static_cast<std::string>(tech->getName()), peelPass);

                    // Updates the u_hasTextureAlpha flag uniform according to the configuration of the texture adaptor
                    if(this->hasDiffuseTexture() && shadingProgramSuffix == s_NONE  && colorPass)
                    {
                        int useTextureAlpha = static_cast<int>(m_texAdaptor->getUseAlpha());
                        ogrePass->getFragmentProgramParameters()->setNamedConstant("u_useTextureAlpha",
                                                                                   useTextureAlpha);
                    }
                }

                if(m_hasPrimitiveColor)
                {
                    const std::string texUnitName = "PerPrimitiveColor";
                    ::Ogre::TextureUnitState* texUnitState = ogrePass->getTextureUnitState(texUnitName);

                    const auto result = ::Ogre::TextureManager::getSingleton().createOrRetrieve(
                        m_perPrimitiveColorTextureName,
                        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);

                    SLM_ASSERT("Texture should have been created before in SMesh !", !result.second);

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
                        // Ogre packs texture unit indices so we can't use spare indices :'(
                        ogrePass->getGeometryProgramParameters()->setNamedConstant("u_colorPrimitiveTexture",
                                                                                   unitStateCount - 1);
                    }

                    // Set outside the scope of texture creation because the size could vary
                    ::Ogre::Vector2 size(static_cast<float>(tex->getWidth()), static_cast<float>(tex->getHeight()));
                    ogrePass->getGeometryProgramParameters()->setNamedConstant("u_colorPrimitiveTextureSize", size);
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

//------------------------------------------------------------------------------

void SMaterial::createTextureAdaptor()
{
    SLM_ASSERT("Texture adaptor already configured in XML", m_texAdaptorUID.empty());

    ::fwData::Material::sptr f4sMaterial = this->getObject< ::fwData::Material >();

    // If the associated material has a texture, we have to create a texture adaptor to handle it
    if(f4sMaterial->getDiffuseTexture())
    {
        // Creates an Ogre adaptor and associates it with the f4s texture object
        auto textureService = ::fwServices::add< ::fwRenderOgre::IAdaptor >(f4sMaterial->getDiffuseTexture(),
                                                                            "::visuOgreAdaptor::STexture");

        // If the adaptor has been well instantiated, we can cast it into a texture adaptor
        SLM_ASSERT("textureService not instantiated", textureService);
        m_texAdaptor = ::visuOgreAdaptor::STexture::dynamicCast(textureService);

        m_texAdaptor->setID(this->getID() + "_" + m_texAdaptor->getID());
        m_texAdaptor->setRenderService(this->getRenderService());
        m_texAdaptor->setLayerID(m_layerID);

        std::string materialName = this->getObject()->getID();
        m_texAdaptor->setTextureName(materialName + "_Texture");

        this->registerService(m_texAdaptor);

        m_textureConnection->connect(m_texAdaptor, ::visuOgreAdaptor::STexture::s_TEXTURE_SWAPPED_SIG, this->getSptr(),
                                     ::visuOgreAdaptor::SMaterial::s_SWAP_TEXTURE_SLOT);

        m_texAdaptor->start();
    }
}

//------------------------------------------------------------------------------

void SMaterial::removeTextureAdaptor()
{
    SLM_ASSERT("Missing texture adaptor", m_texAdaptor);
    SLM_ASSERT("Texture adaptor already configured in XML", m_texAdaptorUID.empty());

    this->getRenderService()->makeCurrent();

    ::Ogre::Material::TechniqueIterator tech_iter = m_material->getTechniqueIterator();
    while( tech_iter.hasMoreElements())
    {
        ::Ogre::Technique* technique = tech_iter.getNext();

        bool peelTech = false;

        if(isColorTechnique(technique->getName(), peelTech))
        {
            ::Ogre::Pass* pass                     = technique->getPass(0);
            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("diffuseTexture");
            if(texUnitState)
            {
                auto texUnitStateIndex = pass->getTextureUnitStateIndex(texUnitState);
                pass->removeTextureUnitState(texUnitStateIndex);
            }
        }
    }

    this->requestRender();

    m_textureConnection->disconnect();
    this->unregisterServices("::visuOgreAdaptor::STexture");
    m_texAdaptor.reset();

    // Update the shaders
    ::fwData::Material::sptr material = this->getObject< ::fwData::Material >();
    this->setShadingMode( material->getShadingMode() );

}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor
