/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
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

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/fwID.hpp>

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SShaderParameter.hpp"
#include "visuOgreAdaptor/STexture.hpp"

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

const std::string SMaterial::DEFAULT_MATERIAL_TEMPLATE_NAME = "Default";

static const std::string s_EDGE_PASS    = "EdgePass";
static const std::string s_NORMALS_PASS = "NormalsPass";

//------------------------------------------------------------------------------

SMaterial::SMaterial() throw() :
    m_materialName(""),
    m_materialTemplateName(DEFAULT_MATERIAL_TEMPLATE_NAME),
    m_hasMeshNormal(true),
    m_hasVertexColor(false),
    m_hasPrimitiveColor(false),
    m_primitiveType(::fwData::Mesh::TRIANGLE),
    m_meshBoundingBox(::Ogre::Vector3::ZERO, ::Ogre::Vector3::ZERO),
    m_normalLengthFactor(0.1f)
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

    OSLM_ASSERT( "Material '" << m_materialTemplateName << "'' not found", !material.isNull() );

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

int SMaterial::getStartPriority()
{
    return -10;
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
    else
    {
        // Choose a default name if not provided
        m_materialName = this->getID();
    }

    if(m_configuration->hasAttribute("textureAdaptor"))
    {
        m_texAdaptorUID = m_configuration->getAttributeValue("textureAdaptor");
    }

    if(m_configuration->hasAttribute("shadingMode"))
    {
        m_shadingMode = m_configuration->getAttributeValue("shadingMode");
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
        ::fwData::Material::sptr material = this->getObject < ::fwData::Material >();
        material->setShadingMode(shadingMode);
    }

    m_material = ::Ogre::MaterialManager::getSingleton().create(
        m_materialName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ::fwData::String::sptr string = ::fwData::String::New();
    string->setValue(m_materialTemplateName);

    ::fwData::Material::sptr material = this->getObject < ::fwData::Material >();
    ::fwComEd::helper::Field helper(material);
    helper.setField("ogreMaterial", string);
    helper.notify();

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

        if(m_texAdaptor->isStarted())
        {
            this->swapTexture();
        }
    }
    else
    {
        this->createTextureAdaptor();
    }

    this->doUpdate();
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
    this->requestRender();
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
            this->doUpdate();
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
    // is null when we call this method on the first time (from doStart() for instance)
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

                    if(m_texAdaptor)
                    {
                        const bool colorPass = ::fwRenderOgre::helper::Shading::isColorTechnique(*tech);

                        // Updates the u_hasTextureAlpha flag uniform according to the configuration
                        // of the texture adaptor
                        if(this->hasDiffuseTexture() && colorPass)
                        {
                            int useTextureAlpha = static_cast<int>(m_texAdaptor->getUseAlpha());
                            ogrePass->getFragmentProgramParameters()->setNamedConstant("u_useTextureAlpha",
                                                                                       useTextureAlpha);
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

    m_textureConnection->disconnect();
    this->unregisterServices("::visuOgreAdaptor::STexture");
    m_texAdaptor.reset();

    // Update the shaders
    ::fwData::Material::sptr material = this->getObject< ::fwData::Material >();
    this->updateShadingMode( material->getShadingMode() );

    this->requestRender();
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
