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

#include "viz/scene3d/helper/Shading.hpp"

#include "viz/scene3d/ogre.hpp"

#include <data/Boolean.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>
#include <data/Matrix4.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>

#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>

#include <regex>

/**
 * Do not mark `SAMPLER1DSHADOW`, `SAMPLER2DSHADOW` as incorrect.
 * cspell:ignore SAMPLER1DSHADOW SAMPLER2DSHADOW
 */

namespace sight::viz::scene3d::helper
{

static const std::string s_EDGE_PASS    = "EdgePass";
static const std::string s_NORMALS_PASS = "NormalsPass";

static const std::regex s_PEEL_REGEX(".*/peel.*");
static const std::regex s_WEIGHT_BLEND_REGEX(".*/weightBlend.*");
static const std::regex s_TRANSMITTANCE_BLEND_REGEX(".*/transmittanceBlend.*");
static const std::regex s_DEPTH_MAP_REGEX("(.*depth.*)|(.*backDepth.*)");
static const std::regex s_LIGHT_PARAM_REGEX("u_f[2-4]?(NumLights|Light(Ambient|Dir|Diffuse|Specular).*)");

static const std::string s_AMBIENT       = "Ambient";
static const std::string s_FLAT          = "Flat";
static const std::string s_PIXELLIGHTING = "PixelLit";

//-----------------------------------------------------------------------------

bool Shading::isColorTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const std::regex regexDualPeelInit("Dual.*/peelInit.*");

    const bool peelTech     = std::regex_match(name, s_PEEL_REGEX);
    const bool weightPass   = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool peelInitPass = std::regex_match(name, regexDualPeelInit);

    return name.empty() || (peelTech && !peelInitPass) || weightPass;
}

//-----------------------------------------------------------------------------

bool Shading::isPeelTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool peelTech     = std::regex_match(name, s_PEEL_REGEX);

    return peelTech;
}

//-----------------------------------------------------------------------------

bool Shading::isGeometricTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();

    const bool peelPass           = std::regex_match(name, s_PEEL_REGEX);
    const bool weightBlend        = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool transmittanceBlend = std::regex_match(name, s_TRANSMITTANCE_BLEND_REGEX);

    return name.empty() || peelPass || weightBlend || transmittanceBlend;
}

//-----------------------------------------------------------------------------

bool Shading::isDepthOnlyTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool depth        = std::regex_match(name, s_DEPTH_MAP_REGEX);

    return depth;
}

//-----------------------------------------------------------------------------
std::string Shading::getPermutation(data::Material::ShadingType _mode, bool _diffuseTexture, bool _vertexColor)
{
    std::string suffix;

    switch(_mode)
    {
        case data::Material::AMBIENT:
            suffix = s_AMBIENT;
            break;

        case data::Material::FLAT:
            suffix = s_FLAT;
            break;

        case data::Material::PHONG:
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

std::string Shading::getR2VBGeometryProgramName(
    data::Mesh::CellType _primitiveType,
    bool _diffuseTexture,
    bool _vertexColor,
    bool _hasPrimitiveColor
)
{
    std::string suffix;

    if(_primitiveType == data::Mesh::CellType::QUAD)
    {
        suffix = "Quad";
    }
    else if(_primitiveType == data::Mesh::CellType::TETRA)
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

    std::string name = "R2VB/" + suffix + "_GP";

    return name;
}

//-----------------------------------------------------------------------------

std::string Shading::setPermutationInProgramName(const std::string& _name, const std::string& _permutation)
{
    std::string prgName;

    // Clear the suffix in shader names (+VT+...)
    static const std::regex regexConcat("\\+.*(_[FV]P)");
    prgName = std::regex_replace(_name, regexConcat, "$1");

    // Replace the shading technique
    static const std::regex regexShading(std::string("(").append(s_AMBIENT).append(")|(").append(s_FLAT).append(")|(").
                                         append(s_PIXELLIGHTING).append(")"));
    prgName = std::regex_replace(prgName, regexShading, _permutation);

    return prgName;
}

//-----------------------------------------------------------------------------

std::string Shading::setTechniqueInProgramName(const std::string& _name, const std::string& _tech)
{
    std::string prgName;

    // Replace the technique and the pass names
    static const std::regex regex(".*/");
    prgName = std::regex_replace(_name, regex, _tech + "/");

    return prgName;
}

//------------------------------------------------------------------------------

Shading::ShaderConstantsType Shading::findMaterialConstants(Ogre::Material& _material)
{
    ShaderConstantsType constants;

    // Only work on the first technique
    Ogre::Pass* pass = _material.getTechnique(0)->getPass(0);

    // If the material is programmable (ie contains shader programs) create associated ShaderParameter adaptor
    // with the given data::Object ID
    if(pass->isProgrammable())
    {
        Ogre::GpuProgramParametersSharedPtr params;

        // Getting params for each program type
        if(pass->hasVertexProgram())
        {
            params = pass->getVertexProgramParameters();
            auto vpConstants = findShaderConstants(params, Ogre::GPT_VERTEX_PROGRAM);
            std::move(vpConstants.begin(), vpConstants.end(), std::inserter(constants, constants.begin()));
        }

        if(pass->hasFragmentProgram())
        {
            params = pass->getFragmentProgramParameters();
            auto fpConstants = findShaderConstants(params, Ogre::GPT_FRAGMENT_PROGRAM);
            std::move(fpConstants.begin(), fpConstants.end(), std::inserter(constants, constants.begin()));
        }

        if(pass->hasGeometryProgram())
        {
            params = pass->getGeometryProgramParameters();
            auto gpConstants = findShaderConstants(params, Ogre::GPT_GEOMETRY_PROGRAM);
            std::move(gpConstants.begin(), gpConstants.end(), std::inserter(constants, constants.begin()));
        }

        if(pass->hasTessellationHullProgram())
        {
            SIGHT_WARN("Tessellation Hull Program in Material not supported yet");
        }

        if(pass->hasTessellationDomainProgram())
        {
            SIGHT_WARN("Tessellation Domain Program in Material not supported yet");
        }
    }

    return constants;
}

//------------------------------------------------------------------------------

Shading::ShaderConstantsType Shading::findShaderConstants(
    Ogre::GpuProgramParametersSharedPtr _params,
    Ogre::GpuProgramType _shaderType,
    bool _enableLightConstants
)
{
    ShaderConstantsType parameters;

    Ogre::GpuNamedConstants constantsDefinitionMap = _params->getConstantDefinitions();

    // Get only user constants
    for(const auto& cstDef : constantsDefinitionMap.map)
    {
        if(!_enableLightConstants)
        {
            bool isLightConstant = std::regex_match(static_cast<std::string>(cstDef.first), s_LIGHT_PARAM_REGEX);

            if(isLightConstant)
            {
                continue;
            }
        }

        if(!Ogre::StringUtil::endsWith(
               cstDef.first,
               "[0]"
           ) && (_params->findAutoConstantEntry(cstDef.first) == nullptr))
        {
            ConstantValueType constantValue {};
            bool found = false;
            if(cstDef.second.isDouble())
            {
                for(std::size_t i = 0 ; i < cstDef.second.elementSize ; ++i)
                {
                    constantValue.d[i] = _params->getDoubleConstantList()[cstDef.second.physicalIndex + i];
                }

                found = true;
            }
            else if(cstDef.second.isFloat())
            {
                for(std::size_t i = 0 ; i < cstDef.second.elementSize ; ++i)
                {
                    constantValue.f[i] = _params->getFloatConstantList()[cstDef.second.physicalIndex + i];
                }

                found = true;
            }
            else if(cstDef.second.isInt())
            {
                for(std::size_t i = 0 ; i < cstDef.second.elementSize ; ++i)
                {
                    constantValue.i[i] = _params->getIntConstantList()[cstDef.second.physicalIndex + i];
                }

                found = true;
            }

            if(found)
            {
                parameters.push_back(
                    std::make_tuple(
                        cstDef.first,
                        cstDef.second.constType,
                        _shaderType,
                        constantValue
                    )
                );
            }
        }
    }

    return parameters;
}

//-----------------------------------------------------------------------------

data::Object::sptr Shading::createObjectFromShaderParameter(Ogre::GpuConstantType _type, ConstantValueType _value)
{
    data::Object::sptr object;

    switch(_type)
    {
        case Ogre::GpuConstantType::GCT_FLOAT1:
        {
            auto newObj = data::Float::New();
            newObj->setValue(_value.f[0]);
            object = newObj;
            break;
        }

        case Ogre::GpuConstantType::GCT_FLOAT2:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({2}, core::Type::FLOAT);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<float>(0) = _value.f[0];
            arrayObject->at<float>(1) = _value.f[1];

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_FLOAT3:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({3}, core::Type::FLOAT);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<float>(0) = _value.f[0];
            arrayObject->at<float>(1) = _value.f[1];
            arrayObject->at<float>(2) = _value.f[2];

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_FLOAT4:
        {
            auto newObj = data::Color::New();
            newObj->setRGBA(_value.f[0], _value.f[1], _value.f[2], _value.f[3]);
            object = newObj;
            break;
        }

        case Ogre::GpuConstantType::GCT_MATRIX_4X4:
            object = data::Matrix4::New();
            break;

        case Ogre::GpuConstantType::GCT_INT1:
        {
            auto newObj = data::Integer::New();
            newObj->setValue(_value.i[0]);
            object = newObj;
            break;
        }

        case Ogre::GpuConstantType::GCT_INT2:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({2}, core::Type::INT32);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value.i[0]);
            arrayObject->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value.i[1]);

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_INT3:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({3}, core::Type::INT32);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value.i[0]);
            arrayObject->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value.i[1]);
            arrayObject->at<std::uint32_t>(2) = static_cast<std::uint32_t>(_value.i[2]);

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_INT4:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({4}, core::Type::INT32);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value.i[0]);
            arrayObject->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value.i[1]);
            arrayObject->at<std::uint32_t>(2) = static_cast<std::uint32_t>(_value.i[2]);
            arrayObject->at<std::uint32_t>(3) = static_cast<std::uint32_t>(_value.i[3]);

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE1:
        {
            auto newObj = data::Float::New();
            newObj->setValue(static_cast<float>(_value.d[0]));
            object = newObj;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE2:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({2}, core::Type::DOUBLE);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<double>(0) = _value.d[0];
            arrayObject->at<double>(1) = _value.d[1];

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE3:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({3}, core::Type::DOUBLE);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<double>(0) = _value.d[0];
            arrayObject->at<double>(1) = _value.d[1];
            arrayObject->at<double>(2) = _value.d[2];

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE4:
        {
            data::Array::sptr arrayObject = data::Array::New();

            arrayObject->resize({4}, core::Type::DOUBLE);

            const auto dumpLock = arrayObject->dump_lock();

            arrayObject->at<double>(0) = _value.d[0];
            arrayObject->at<double>(1) = _value.d[1];
            arrayObject->at<double>(2) = _value.d[2];
            arrayObject->at<double>(3) = _value.d[3];

            object = arrayObject;
            break;
        }

        case Ogre::GpuConstantType::GCT_MATRIX_DOUBLE_4X4:
            object = data::Matrix4::New();
            break;

        default:
            [[maybe_unused]] static const std::array GpuConstantTypeNames
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
            SIGHT_WARN("Object type " + std::string(GpuConstantTypeNames[_type - 1]) + " not supported yet");
    }

    return object;
}

// ----------------------------------------------------------------------------

Ogre::GpuProgramPtr Shading::createProgramFrom(
    const std::string& _name,
    const std::string& _sourceFileName,
    const GpuProgramParametersType& _parameters,
    Ogre::GpuProgramType _shaderType,
    const std::string& _baseName
)
{
    auto& mgr = Ogre::HighLevelGpuProgramManager::getSingleton();

    auto resource = mgr.getResourceByName(_name, RESOURCE_GROUP);
    if(resource)
    {
        return Ogre::dynamic_pointer_cast<Ogre::GpuProgram>(resource);
    }

    // Create shader object
    Ogre::HighLevelGpuProgramPtr newProgram;
    newProgram = mgr.createProgram(_name, RESOURCE_GROUP, "glsl", _shaderType);

    newProgram->setSourceFile(_sourceFileName);

    auto srcResource                 = mgr.getResourceByName(_baseName, RESOURCE_GROUP);
    auto srcProgram                  = Ogre::dynamic_pointer_cast<Ogre::GpuProgram>(srcResource);
    Ogre::String preprocessorDefines = srcProgram->getParameter("preprocessor_defines");

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

    // Copy parameters from the source program
    const Ogre::GpuProgramParametersSharedPtr& baseParams = srcProgram->getDefaultParameters();
    const Ogre::GpuProgramParametersSharedPtr& params     = newProgram->getDefaultParameters();
    params->copyMatchingNamedConstantsFrom(*baseParams);

    newProgram->load();
    return {newProgram};
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
