/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/Material.hpp"

fwCampImplementDataMacro((fwData)(Material))
{
    builder
    .tag("object_version", "4")
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

fwCampImplementEnumMacro((fwData)(Material)(OptionsType))
{
    builder
    .value("STANDARD", ::fwData::Material::STANDARD)
    .value("NORMALS", ::fwData::Material::NORMALS)
    .value("CELLS_NORMALS", ::fwData::Material::CELLS_NORMALS)
    ;
}

fwCampImplementEnumMacro((fwData)(Material)(ShadingType))
{
    builder
    .value("AMBIENT", ::fwData::Material::AMBIENT)
    .value("FLAT", ::fwData::Material::FLAT)
    .value("GOURAUD", ::fwData::Material::GOURAUD)
    .value("PHONG", ::fwData::Material::PHONG)
    ;
}

fwCampImplementEnumMacro((fwData)(Material)(RepresentationType))
{
    builder
    .value("SURFACE",  ::fwData::Material::SURFACE)
    .value("POINT",  ::fwData::Material::POINT )
    .value("WIREFRAME",  ::fwData::Material::WIREFRAME)
    .value("EDGE",  ::fwData::Material::EDGE)
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
