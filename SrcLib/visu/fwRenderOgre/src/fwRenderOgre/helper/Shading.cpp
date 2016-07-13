/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/helper/Shading.hpp"

#include <regex>
#include <boost/regex.hpp>

#include <fwComEd/helper/Array.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/TransformationMatrix3D.hpp>

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
        default:
            SLM_ERROR("Unknown shading mode. ");
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

//-----------------------------------------------------------------------------

::fwData::Object::sptr Shading::createObjectFromShaderParameter(::Ogre::GpuConstantType type)
{
    ::fwData::Object::sptr object;

    switch(type)
    {
        case ::Ogre::GpuConstantType::GCT_FLOAT1:
            object = ::fwData::Float::New();
            break;
        case ::Ogre::GpuConstantType::GCT_FLOAT2:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            float vec[2] = { 0., 0. };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::FloatType>();
            arrayObject->resize( type, {1}, 2, true);

            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
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
            break;
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
            object = ::fwData::Integer::New();
            break;
        case ::Ogre::GpuConstantType::GCT_INT2:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            int vec[2] = {0, 0};

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 2, true);

            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_INT3:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            int* vec[3] = {0, 0, 0};

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 3, true);

            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_INT4:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            int* vec[4] = {0, 0, 0, 0};

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::Int32Type>();
            arrayObject->resize( type, {1}, 4, true);

            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE1:
            object = ::fwData::Float::New();
            break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE2:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            float vec[2] = { 0., 0. };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::FloatType>();
            arrayObject->resize( type, {1}, 2, true);

            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE3:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            float vec[3] = { 0., 0., 0. };

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::FloatType>();
            arrayObject->resize( type, {1}, 3, true);

            ::fwComEd::helper::Array arrayHelper(arrayObject);
            arrayHelper.setItem( {0}, vec);

            object = arrayObject;
        }
        break;
        case ::Ogre::GpuConstantType::GCT_DOUBLE4:
        {
            ::fwData::Array::sptr arrayObject = ::fwData::Array::New();
            float vec[4] = { 0., 0., 0., 0.};

            ::fwTools::Type type = ::fwTools::Type::create< ::fwTools::Type::FloatType>();
            arrayObject->resize( type, {1}, 4, true);

            ::fwComEd::helper::Array arrayHelper(arrayObject);
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
            OSLM_FATAL("Object type "+GpuConstantTypeNames[type-1]+" not supported yet");
    }
    return object;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
