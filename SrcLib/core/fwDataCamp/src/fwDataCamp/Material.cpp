/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/Material.hpp"

fwCampImplementDataMacro((fwData)(Material))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("shading_mode",&::fwData::Material::m_shadingMode)
    .property("representation_mode",&::fwData::Material::m_representationMode)
    .property("options_mode", &::fwData::Material::m_optionsMode)
    .property("ambient",&::fwData::Material::m_ambient)
    .property("diffuse", &::fwData::Material::m_diffuse)
    .property("diffuse_texture", &::fwData::Material::m_diffuseTexture)
    .property("diffuse_texture_filtering", &::fwData::Material::m_diffuseTextureFiltering)
    .property("diffuse_texture_wrapping", &::fwData::Material::m_diffuseTextureWrapping)
    ;
}

fwCampImplementEnumMacro((fwData)(Material)(OPTIONS_MODE))
{
    builder
    .value("STANDARD", ::fwData::Material::MODE_STANDARD)
    .value("NORMALS", ::fwData::Material::MODE_NORMALS)
    ;
}

fwCampImplementEnumMacro((fwData)(Material)(SHADING_MODE))
{
    builder
    .value("FLAT", ::fwData::Material::MODE_FLAT)
    .value("GOURAUD", ::fwData::Material::MODE_GOURAUD)
    .value("PHONG", ::fwData::Material::MODE_PHONG)
    ;
}

fwCampImplementEnumMacro((fwData)(Material)(REPRESENTATION_MODE))
{
    builder
    .value("SURFACE",  ::fwData::Material::MODE_SURFACE)
    .value("POINT",  ::fwData::Material::MODE_POINT )
    .value("WIREFRAME",  ::fwData::Material::MODE_WIREFRAME)
    .value("EDGE",  ::fwData::Material::MODE_EDGE)
    ;
}

fwCampImplementEnumMacro((fwData)(Material)(FilteringType))
{
    builder
    .value("NEAREST", ::fwData::Material::NEAREST)
    .value("LINEAR", ::fwData::Material::LINEAR)
    ;
}

fwCampImplementEnumMacro((fwData)(Material)(WrappingType))
{
    builder
    .value("CLAMP", ::fwData::Material::CLAMP)
    .value("REPEAT", ::fwData::Material::REPEAT)
    ;
}
