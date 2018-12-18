/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwRenderOgre/helper/Shading.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <boost/regex.hpp>

#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>

#include <regex>

namespace fwRenderOgre
{

namespace helper
{

static const std::string s_EDGE_PASS    = "EdgePass";
static const std::string s_NORMALS_PASS = "NormalsPass";

static const std::regex s_PEEL_REGEX(".*/peel.*");
static const std::regex s_WEIGHT_BLEND_REGEX(".*/weightBlend.*");
static const std::regex s_TRANSMITTANCE_BLEND_REGEX(".*/transmittanceBlend.*");
static const std::regex s_DEPTH_MAP_REGEX("(.*depth.*)|(.*backDepth.*)");
static const std::regex s_LIGHT_PARAM_REGEX("u_(numLights|light(Ambient|Dir|Diffuse|Specular).*)");

static const std::string s_AMBIENT       = "Ambient";
static const std::string s_FLAT          = "Flat";
static const std::string s_GOURAUD       = "Gouraud";
static const std::string s_PIXELLIGHTING = "PixelLit";

//-----------------------------------------------------------------------------

bool Shading::isColorTechnique(const ::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const std::regex regexDualPeelInit("Dual.*/peelInit.*");

    const bool peelTech     = std::regex_match(name, s_PEEL_REGEX);
    const bool weightPass   = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool peelInitPass = std::regex_match(name, regexDualPeelInit);

    return name == "" || (peelTech && !peelInitPass) || weightPass;
}

//-----------------------------------------------------------------------------

bool Shading::isPeelTechnique(const ::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool peelTech     = std::regex_match(name, s_PEEL_REGEX);

    return peelTech;
}

//-----------------------------------------------------------------------------

bool Shading::isGeometricTechnique(const ::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();

    const bool peelPass           = std::regex_match(name, s_PEEL_REGEX);
    const bool weightBlend        = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool transmittanceBlend = std::regex_match(name, s_TRANSMITTANCE_BLEND_REGEX);

    return name == "" || peelPass || weightBlend || transmittanceBlend;
}

//-----------------------------------------------------------------------------

bool Shading::isDepthOnlyTechnique(const ::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool depth        = std::regex_match(name, s_DEPTH_MAP_REGEX);

    return depth;
}

//-----------------------------------------------------------------------------
std::string Shading::getPermutation(::fwData::Material::ShadingType _mode, bool _diffuseTexture, bool _vertexColor)
{
    std::string suffix;

    switch(_mode)
    {
        case ::fwData::Material::AMBIENT:
            suffix = s_AMBIENT;
            break;
        case ::fwData::Material::FLAT:
            suffix = s_FLAT;
            break;
        case ::fwData::Material::GOURAUD:
            suffix = s_GOURAUD;
            break;
        case ::fwData::Material::PHONG:
            suffix = s_PIXELLIGHTING;
            break;
    }

    if(_vertexColor)
    {
        suffix += "+VT";
    }
    if(_diffuseTexture)
    {
        suffix += "+DfsTex";
    }

    return suffix;
}

//-----------------------------------------------------------------------------

std::string Shading::getR2VBGeometryProgramName(::fwData::Mesh::CellTypesEnum _primitiveType, bool _diffuseTexture,
                                                bool _vertexColor, bool _hasPrimitiveColor)
{
    std::string suffix;

    if(_primitiveType == ::fwData::Mesh::QUAD)
    {
        suffix = "Quad";
    }
    else if(_primitiveType == ::fwData::Mesh::TETRA)
    {
        suffix = "Tetra";
    }
    else
    {
        suffix = "Triangles";
    }

    if(_vertexColor)
    {
        suffix += "+VT";
    }
    if(_diffuseTexture)
    {
        suffix += "+DfsTex";
    }
    if(_hasPrimitiveColor)
    {
        suffix += "+PPColor";
    }

    const std::string name = "R2VB/" + suffix + "_GP";

    return name;
}

//-----------------------------------------------------------------------------

std::string Shading::setPermutationInProgramName(const std::string& _name, const std::string& _permutation)
{
    std::string prgName;

    // Clear the suffix in shader names (+VT+...)
    static const ::boost::regex regexConcat("\\N{plus-sign}.*(_[FV]P)", ::boost::regex::extended);
    prgName = ::boost::regex_replace(_name, regexConcat, "$1");

    // Replace the shading technique
    static const ::boost::regex regexShading("("+s_AMBIENT+")|("+s_FLAT+")|("+s_GOURAUD+")|("+s_PIXELLIGHTING+")");
    prgName = ::boost::regex_replace(prgName, regexShading, _permutation);

    return prgName;
}

//-----------------------------------------------------------------------------

std::string Shading::setTechniqueInProgramName(const std::string& _name, const std::string& _tech)
{
    std::string prgName;

    // Replace the technique and the pass names
    static const ::boost::regex regex(".*/");
    prgName = ::boost::regex_replace(_name, regex, _tech + "/");

    return prgName;
}

//------------------------------------------------------------------------------

Shading::ShaderConstantsType Shading::findMaterialConstants(::Ogre::Material& _material)
{
    ShaderConstantsType constants;

    // Only work on the first technique
    ::Ogre::Pass* pass = _material.getTechnique(0)->getPass(0);

    // If the material is programmable (ie contains shader programs) create associated ShaderParameter adaptor
    // with the given ::fwData::Object ID
    if (pass->isProgrammable())
    {
        ::Ogre::GpuProgramParametersSharedPtr params;

        // Getting params for each program type
        if (pass->hasVertexProgram())
        {
            params = pass->getVertexProgramParameters();
            auto vpConstants = findShaderConstants(params, ::Ogre::GPT_VERTEX_PROGRAM);
            std::move(vpConstants.begin(), vpConstants.end(), std::inserter(constants, constants.begin()));
        }
        if (pass->hasFragmentProgram())
        {
            params = pass->getFragmentProgramParameters();
            auto fpConstants = findShaderConstants(params, ::Ogre::GPT_FRAGMENT_PROGRAM);
            std::move(fpConstants.begin(), fpConstants.end(), std::inserter(constants, constants.begin()));
        }
        if (pass->hasGeometryProgram())
        {
            params = pass->getGeometryProgramParameters();
            auto gpConstants = findShaderConstants(params, ::Ogre::GPT_GEOMETRY_PROGRAM);
            std::move(gpConstants.begin(), gpConstants.end(), std::inserter(constants, constants.begin()));
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

    return constants;
}

//------------------------------------------------------------------------------

Shading::ShaderConstantsType Shading::findShaderConstants(::Ogre::GpuProgramParametersSharedPtr _params,
                                                          ::Ogre::GpuProgramType _shaderType,
                                                          bool _enableLightConstants)
{
    ShaderConstantsType parameters;

    ::Ogre::GpuNamedConstants constantsDefinitionMap = _params->getConstantDefinitions();

    // Get only user constants
    for (const auto& cstDef : constantsDefinitionMap.map)
    {
        if(!_enableLightConstants)
        {
            bool isLightConstant = std::regex_match(static_cast<std::string>(cstDef.first), s_LIGHT_PARAM_REGEX);

            if(isLightConstant)
            {
                continue;
            }
        }

        if (!::Ogre::StringUtil::endsWith(cstDef.first, "[0]") && !_params->findAutoConstantEntry(cstDef.first))
        {
            ConstantValueType constantValue;
            bool found = false;
            if(cstDef.second.isDouble())
            {
                for(size_t i = 0; i < cstDef.second.elementSize; ++i)
                {
                    constantValue.d[i] = _params->getDoubleConstantList()[cstDef.second.physicalIndex + i];
                }
                found = true;
            }
            else if(cstDef.second.isFloat())
            {
                for(size_t i = 0; i < cstDef.second.elementSize; ++i)
                {
                    constantValue.f[i] = _params->getFloatConstantList()[cstDef.second.physicalIndex + i];
                }
                found = true;
            }
            else if(cstDef.second.isInt())
            {
                for(size_t i = 0; i < cstDef.second.elementSize; ++i)
                {
                    constantValue.i[i] = _params->getIntConstantList()[cstDef.second.physicalIndex + i];
                }
                found = true;
            }

            if(found)
            {
                parameters.push_back(std::make_tuple(cstDef.first, cstDef.second.constType, _shaderType,
                                                     constantValue));
            }
        }
    }

    return parameters;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType _type, ConstantValueType _value)
{
    ::fwData::Object::sptr object;

    switch(_type)
    {
        case ::Ogre::GpuConstantType::GCT_FLOAT1:
        {
            auto newObj = ::fwData::Float::New();
            newObj->setValue(_value.f[0]);
            object = newObj;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_FLOAT2:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            float vec[2] = { _value.f[0], _value.f[1] };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::FloatType>();
            arrayObject->resize( type, {1}, 2, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_FLOAT3:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            float vec[3] = { _value.f[0], _value.f[1], _value.f[2] };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::FloatType>();
            arrayObject->resize( type, {1}, 3, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_FLOAT4:
        {
            auto newObj = ::fwData::Color::New();
            newObj->setRGBA(_value.f[0], _value.f[1], _value.f[2], _value.f[3]);
            object = newObj;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_MATRIX_4X4:
            object = ::fwData::TransformationMatrix3D::New();
            break;
        case ::Ogre::GpuConstantType::GCT_INT1:
        {
            auto newObj = ::fwData::Integer::New();
            newObj->setValue(_value.i[0]);
            object = newObj;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_INT2:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            int vec[2] = {_value.i[0], _value.i[1]};

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 2, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_INT3:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            int vec[3] = {_value.i[0], _value.i[1], _value.i[2]};

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 3, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_INT4:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            int vec[4] = {_value.i[0], _value.i[1], _value.i[2], _value.i[3]};

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 4, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE1:
        {
            auto newObj = ::fwData::Float::New();
            newObj->setValue( static_cast<float>(_value.d[0]) );
            object = newObj;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE2:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            double vec[2] = { _value.d[0], _value.d[1] };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::DoubleType>();
            arrayObject->resize( type, {1}, 2, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE3:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            double vec[3] = { _value.d[0], _value.d[1], _value.d[2] };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::DoubleType>();
            arrayObject->resize( type, {1}, 3, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE4:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            double vec[4] = { _value.d[0], _value.d[1], _value.d[2], _value.d[3] };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::DoubleType>();
            arrayObject->resize( type, {1}, 4, true);

            ::fwDataTools::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
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
            OSLM_WARN("Object type "+GpuConstantTypeNames[_type-1]+" not supported yet");

            (void)GpuConstantTypeNames; // Only there to avoid the 'unused' warning.
    }
    return object;
}

// ----------------------------------------------------------------------------

::Ogre::GpuProgramPtr Shading::createProgramFrom(const std::string& _name,
                                                 const std::string& _sourceFileName,
                                                 const GpuProgramParametersType& _parameters,
                                                 ::Ogre::GpuProgramType _shaderType,
                                                 const std::string& _baseName)
{
    auto& mgr = ::Ogre::HighLevelGpuProgramManager::getSingleton();

    auto resource = mgr.getResourceByName(_name, "Materials");
    if(resource)
    {
        return ::Ogre::dynamic_pointer_cast< ::Ogre::GpuProgram>(resource);
    }

    // Create shader object
    ::Ogre::HighLevelGpuProgramPtr newProgram;
    newProgram = mgr.createProgram(_name, "Materials", "glsl", _shaderType);

    newProgram->setSourceFile(_sourceFileName);

    auto srcResource = mgr.getResourceByName(_baseName, ::Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    auto srcProgram  = ::Ogre::dynamic_pointer_cast< ::Ogre::GpuProgram>(srcResource);
    ::Ogre::String preprocessorDefines = srcProgram->getParameter("preprocessor_defines");

    for(const auto& params : _parameters)
    {
        if(params.first == "preprocessor_defines")
        {
            preprocessorDefines += preprocessorDefines.empty() ? params.second : "," + params.second;
        }
        else
        {
            newProgram->setParameter(params.first, params.second);
        }
    }
    newProgram->setParameter("preprocessor_defines", preprocessorDefines);

    // Grab previous attached shaders and add them to the new program
    ::Ogre::String attachedShaders = srcProgram->getParameter("attach");
    newProgram->setParameter("attach", attachedShaders);

    // Copy parameters from the source program
    const ::Ogre::GpuProgramParametersSharedPtr& baseParams = srcProgram->getDefaultParameters();
    const ::Ogre::GpuProgramParametersSharedPtr& params     = newProgram->getDefaultParameters();
    params->copyMatchingNamedConstantsFrom(*baseParams);

    newProgram->load();
    return ::Ogre::GpuProgramPtr(newProgram);
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
