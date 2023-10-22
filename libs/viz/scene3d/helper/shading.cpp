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

#include "viz/scene3d/helper/shading.hpp"

#include "viz/scene3d/ogre.hpp"

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/matrix4.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>

#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>

#include <regex>

/**
 * Do not mark `SAMPLER1DSHADOW`, `SAMPLER2DSHADOW` as incorrect.
 * cspell:ignore SAMPLER1DSHADOW SAMPLER2DSHADOW
 */

namespace sight::viz::scene3d::helper
{

static const std::regex s_PEEL_REGEX(".*/peel.*");
static const std::regex s_WEIGHT_BLEND_REGEX(".*/weightBlend.*");
static const std::regex s_TRANSMITTANCE_BLEND_REGEX(".*/transmittanceBlend.*");
static const std::regex s_DEPTH_MAP_REGEX("(.*depth.*)|(.*backDepth.*)");
static const std::regex s_LIGHT_PARAM_REGEX("u_f[2-4]?(NumLights|Light(Ambient|Dir|Diffuse|Specular).*)");

static const std::string s_AMBIENT       = "Ambient";
static const std::string s_FLAT          = "Flat";
static const std::string s_PIXELLIGHTING = "PixelLit";

//-----------------------------------------------------------------------------

bool shading::isColorTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const std::regex regex_dual_peel_init("Dual.*/peelInit.*");

    const bool peel_tech        = std::regex_match(name, s_PEEL_REGEX);
    const bool weight_pass      = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool peel_init_pass   = std::regex_match(name, regex_dual_peel_init);
    const bool ray_entry_points = name == "FrontFacesMin";

    return name.empty() || (peel_tech && !peel_init_pass) || weight_pass || ray_entry_points;
}

//-----------------------------------------------------------------------------

bool shading::isPeelTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool peel_tech    = std::regex_match(name, s_PEEL_REGEX);

    return peel_tech;
}

//-----------------------------------------------------------------------------

bool shading::isGeometricTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();

    const bool peel_pass           = std::regex_match(name, s_PEEL_REGEX);
    const bool weight_blend        = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool transmittance_blend = std::regex_match(name, s_TRANSMITTANCE_BLEND_REGEX);

    return name.empty() || peel_pass || weight_blend || transmittance_blend;
}

//-----------------------------------------------------------------------------

bool shading::isDepthOnlyTechnique(const Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool depth        = std::regex_match(name, s_DEPTH_MAP_REGEX);

    return depth;
}

//-----------------------------------------------------------------------------
std::string shading::getPermutation(data::material::shading_t _mode, bool _diffuse_texture, bool _vertex_color)
{
    std::string suffix;

    switch(_mode)
    {
        case data::material::AMBIENT:
            suffix = s_AMBIENT;
            break;

        case data::material::FLAT:
            suffix = s_FLAT;
            break;

        case data::material::PHONG:
            suffix = s_PIXELLIGHTING;
            break;
    }

    if(_vertex_color)
    {
        suffix += "+VT";
    }

    if(_diffuse_texture)
    {
        suffix += "+DfsTex";
    }

    return suffix;
}

//-----------------------------------------------------------------------------

std::string shading::getR2VBGeometryProgramName(
    data::mesh::cell_type_t _primitive_type,
    bool _diffuse_texture,
    bool _vertex_color,
    bool _has_primitive_color
)
{
    std::string suffix;

    if(_primitive_type == data::mesh::cell_type_t::QUAD)
    {
        suffix = "Quad";
    }
    else if(_primitive_type == data::mesh::cell_type_t::TETRA)
    {
        suffix = "Tetra";
    }
    else
    {
        suffix = "Triangles";
    }

    if(_vertex_color)
    {
        suffix += "+VT";
    }

    if(_diffuse_texture)
    {
        suffix += "+DfsTex";
    }

    if(_has_primitive_color)
    {
        suffix += "+PPColor";
    }

    std::string name = "R2VB/" + suffix + "_GP";

    return name;
}

//-----------------------------------------------------------------------------

std::string shading::setPermutationInProgramName(const std::string& _name, const std::string& _permutation)
{
    std::string prg_name;

    // Clear the suffix in shader names (+VT+...)
    static const std::regex regexConcat("\\+.*(_[FV]P)");
    prg_name = std::regex_replace(_name, regexConcat, "$1");

    // Replace the shading technique
    static const std::regex regexshading("(" + s_AMBIENT + ")|(" + s_FLAT + ")|(" + s_PIXELLIGHTING + ")");
    prg_name = std::regex_replace(prg_name, regexshading, _permutation);

    return prg_name;
}

//-----------------------------------------------------------------------------

std::string shading::setTechniqueInProgramName(const std::string& _name, const std::string& _tech)
{
    std::string prg_name;

    // Replace the technique and the pass names
    static const std::regex regex(".*/");
    prg_name = std::regex_replace(_name, regex, _tech + "/");

    return prg_name;
}

//------------------------------------------------------------------------------

shading::shader_constants_t shading::findMaterialConstants(Ogre::Material& _material)
{
    shader_constants_t constants;

    // Only work on the first technique
    Ogre::Pass* pass = _material.getTechnique(0)->getPass(0);

    // If the material is programmable (ie contains shader programs) create associated ShaderParameter adaptor
    // with the given data::object ID
    if(pass->isProgrammable())
    {
        Ogre::GpuProgramParametersSharedPtr params;

        // Getting params for each program type
        if(pass->hasVertexProgram())
        {
            params = pass->getVertexProgramParameters();
            auto vp_constants = findShaderConstants(params, Ogre::GPT_VERTEX_PROGRAM);
            std::move(vp_constants.begin(), vp_constants.end(), std::inserter(constants, constants.begin()));
        }

        if(pass->hasFragmentProgram())
        {
            params = pass->getFragmentProgramParameters();
            auto fp_constants = findShaderConstants(params, Ogre::GPT_FRAGMENT_PROGRAM);
            std::move(fp_constants.begin(), fp_constants.end(), std::inserter(constants, constants.begin()));
        }

        if(pass->hasGeometryProgram())
        {
            params = pass->getGeometryProgramParameters();
            auto gp_constants = findShaderConstants(params, Ogre::GPT_GEOMETRY_PROGRAM);
            std::move(gp_constants.begin(), gp_constants.end(), std::inserter(constants, constants.begin()));
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

shading::shader_constants_t shading::findShaderConstants(
    Ogre::GpuProgramParametersSharedPtr _params,
    Ogre::GpuProgramType _shader_type,
    bool _enable_light_constants
)
{
    shader_constants_t parameters;

    Ogre::GpuNamedConstants constants_definition_map = _params->getConstantDefinitions();

    // Get only user constants
    for(const auto& cst_def : constants_definition_map.map)
    {
        if(!_enable_light_constants)
        {
            bool is_light_constant = std::regex_match(static_cast<std::string>(cst_def.first), s_LIGHT_PARAM_REGEX);

            if(is_light_constant)
            {
                continue;
            }
        }

        if(!Ogre::StringUtil::endsWith(
               cst_def.first,
               "[0]"
           ) && (_params->findAutoConstantEntry(cst_def.first) == nullptr))
        {
            constant_value_t constant_value {};
            bool found = false;
            if(cst_def.second.isDouble())
            {
                for(std::size_t i = 0 ; i < cst_def.second.elementSize ; ++i)
                {
                    constant_value.d[i] = _params->getDoubleConstantList()[cst_def.second.physicalIndex + i];
                }

                found = true;
            }
            else if(cst_def.second.isFloat())
            {
                for(std::size_t i = 0 ; i < cst_def.second.elementSize ; ++i)
                {
                    constant_value.f[i] = _params->getFloatConstantList()[cst_def.second.physicalIndex + i];
                }

                found = true;
            }
            else if(cst_def.second.isInt())
            {
                for(std::size_t i = 0 ; i < cst_def.second.elementSize ; ++i)
                {
                    constant_value.i[i] = _params->getIntConstantList()[cst_def.second.physicalIndex + i];
                }

                found = true;
            }

            if(found)
            {
                parameters.emplace_back(
                    cst_def.first,
                    cst_def.second.constType,
                    _shader_type,
                    constant_value

                );
            }
        }
    }

    return parameters;
}

//-----------------------------------------------------------------------------

data::object::sptr shading::createObjectFromShaderParameter(Ogre::GpuConstantType _type, constant_value_t _value)
{
    data::object::sptr object;

    switch(_type)
    {
        case Ogre::GpuConstantType::GCT_FLOAT1:
        {
            auto new_obj = std::make_shared<data::real>();
            new_obj->setValue(_value.f[0]);
            object = new_obj;
            break;
        }

        case Ogre::GpuConstantType::GCT_FLOAT2:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({2}, core::type::FLOAT);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<float>(0) = _value.f[0];
            array_object->at<float>(1) = _value.f[1];

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_FLOAT3:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({3}, core::type::FLOAT);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<float>(0) = _value.f[0];
            array_object->at<float>(1) = _value.f[1];
            array_object->at<float>(2) = _value.f[2];

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_FLOAT4:
        {
            auto new_obj = std::make_shared<data::color>();
            new_obj->setRGBA(_value.f[0], _value.f[1], _value.f[2], _value.f[3]);
            object = new_obj;
            break;
        }

        case Ogre::GpuConstantType::GCT_MATRIX_4X4:
            object = std::make_shared<data::matrix4>();
            break;

        case Ogre::GpuConstantType::GCT_INT1:
        {
            auto new_obj = std::make_shared<data::integer>();
            new_obj->setValue(_value.i[0]);
            object = new_obj;
            break;
        }

        case Ogre::GpuConstantType::GCT_INT2:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({2}, core::type::INT32);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value.i[0]);
            array_object->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value.i[1]);

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_INT3:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({3}, core::type::INT32);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value.i[0]);
            array_object->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value.i[1]);
            array_object->at<std::uint32_t>(2) = static_cast<std::uint32_t>(_value.i[2]);

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_INT4:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({4}, core::type::INT32);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<std::uint32_t>(0) = static_cast<std::uint32_t>(_value.i[0]);
            array_object->at<std::uint32_t>(1) = static_cast<std::uint32_t>(_value.i[1]);
            array_object->at<std::uint32_t>(2) = static_cast<std::uint32_t>(_value.i[2]);
            array_object->at<std::uint32_t>(3) = static_cast<std::uint32_t>(_value.i[3]);

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE1:
        {
            auto new_obj = std::make_shared<data::real>();
            new_obj->setValue(static_cast<float>(_value.d[0]));
            object = new_obj;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE2:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({2}, core::type::DOUBLE);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<double>(0) = _value.d[0];
            array_object->at<double>(1) = _value.d[1];

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE3:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({3}, core::type::DOUBLE);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<double>(0) = _value.d[0];
            array_object->at<double>(1) = _value.d[1];
            array_object->at<double>(2) = _value.d[2];

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_DOUBLE4:
        {
            data::array::sptr array_object = std::make_shared<data::array>();

            array_object->resize({4}, core::type::DOUBLE);

            const auto dump_lock = array_object->dump_lock();

            array_object->at<double>(0) = _value.d[0];
            array_object->at<double>(1) = _value.d[1];
            array_object->at<double>(2) = _value.d[2];
            array_object->at<double>(3) = _value.d[3];

            object = array_object;
            break;
        }

        case Ogre::GpuConstantType::GCT_MATRIX_DOUBLE_4X4:
            object = std::make_shared<data::matrix4>();
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

Ogre::GpuProgramPtr shading::createProgramFrom(
    const std::string& _name,
    const std::string& _source_file_name,
    const gpu_program_parameters_t& _parameters,
    Ogre::GpuProgramType _shader_type,
    const std::string& _base_name
)
{
    auto& mgr = Ogre::HighLevelGpuProgramManager::getSingleton();

    auto resource = mgr.getResourceByName(_name, RESOURCE_GROUP);
    if(resource)
    {
        return Ogre::dynamic_pointer_cast<Ogre::GpuProgram>(resource);
    }

    // Create shader object
    Ogre::HighLevelGpuProgramPtr new_program;
    new_program = mgr.createProgram(_name, RESOURCE_GROUP, "glsl", _shader_type);

    new_program->setSourceFile(_source_file_name);

    auto src_resource                 = mgr.getResourceByName(_base_name, RESOURCE_GROUP);
    auto src_program                  = Ogre::dynamic_pointer_cast<Ogre::GpuProgram>(src_resource);
    Ogre::String preprocessor_defines = src_program->getParameter("preprocessor_defines");

    for(const auto& params : _parameters)
    {
        if(params.first == "preprocessor_defines")
        {
            preprocessor_defines += preprocessor_defines.empty() ? params.second : "," + params.second;
        }
        else
        {
            new_program->setParameter(params.first, params.second);
        }
    }

    new_program->setParameter("preprocessor_defines", preprocessor_defines);

    // Copy parameters from the source program
    const Ogre::GpuProgramParametersSharedPtr& base_params = src_program->getDefaultParameters();
    const Ogre::GpuProgramParametersSharedPtr& params      = new_program->getDefaultParameters();
    params->copyMatchingNamedConstantsFrom(*base_params);

    new_program->load();
    return {new_program};
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
