/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/helper/Shading.hpp"

#include <regex>
#include <boost/regex.hpp>

namespace fwRenderOgre
{

namespace helper
{

static const std::string s_EDGE_PASS    = "EdgePass";
static const std::string s_NORMALS_PASS = "NormalsPass";

static const std::regex s_PEEL_REGEX(".*_peel.*");
static const std::regex s_WEIGHT_BLEND_REGEX(".*_weight_blend.*");
static const std::regex s_TRANSMITTANCE_BLEND_REGEX(".*_transmittance_blend.*");
static const std::regex s_DEPTH_MAP_REGEX("(.*depth_map.*)|(.*back_depth.*)");

static const std::string s_AMBIENT       = "Ambient";
static const std::string s_FLAT          = "Flat";
static const std::string s_GOURAUD       = "Gouraud";
static const std::string s_PIXELLIGHTING = "PixelLit";

//-----------------------------------------------------------------------------

bool Shading::isColorTechnique(::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const std::regex regexDualPeelInit("Dual.*_peel_init.*");

    const bool peelTech     = std::regex_match(name, s_PEEL_REGEX);
    const bool weightPass   = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool peelInitPass = std::regex_match(name, regexDualPeelInit);

    return name == "" || (peelTech && !peelInitPass) || weightPass;
}

//-----------------------------------------------------------------------------

bool Shading::isPeelTechnique(::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool peelTech     = std::regex_match(name, s_PEEL_REGEX);

    return peelTech;
}

//-----------------------------------------------------------------------------

bool Shading::isGeometricTechnique(::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();

    const bool peelPass           = std::regex_match(name, s_PEEL_REGEX);
    const bool weightBlend        = std::regex_match(name, s_WEIGHT_BLEND_REGEX);
    const bool transmittanceBlend = std::regex_match(name, s_TRANSMITTANCE_BLEND_REGEX);

    return name == "" || peelPass || weightBlend || transmittanceBlend;
}

//-----------------------------------------------------------------------------

bool Shading::isDepthOnlyTechnique(::Ogre::Technique& _tech)
{
    const std::string& name = _tech.getName();
    const bool depth        = std::regex_match(name, s_DEPTH_MAP_REGEX);

    return depth;
}

//-----------------------------------------------------------------------------
std::string Shading::getProgramSuffix(::fwData::Material::ShadingType _mode, bool _diffuseTexture, bool _vertexColor)
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

    const std::string name = "RenderScene_" + suffix + "_GP_glsl";

    return name;
}

//-----------------------------------------------------------------------------

std::string Shading::replaceProgramSuffix(const std::string& _prgName, const std::string _suffix)
{
    std::string prgName;

    // Clear the suffix in shader names (+VT+...)
    static const ::boost::regex regexConcat("\\N{plus-sign}.*(_[FV]P_glsl)", ::boost::regex::extended);
    prgName = ::boost::regex_replace(_prgName, regexConcat, "$1");

    // Replace the shading technique
    static const ::boost::regex regexShading("("+s_AMBIENT+")|("+s_FLAT+")|("+s_GOURAUD+")|("+s_PIXELLIGHTING+")");
    prgName = ::boost::regex_replace(prgName, regexShading, _suffix);

    return prgName;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
