/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Material.hpp"

fwCampImplementDataMacro((fwData)(Material))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("shading_mode",&::fwData::Material::m_shadingMode)
        .property("representation_mode",&::fwData::Material::m_representationMode)
        .property("options_mode", &::fwData::Material::m_optionsMode)
        .property("ambient",&::fwData::Material::m_ambient)
        .property("diffuse",  &::fwData::Material::m_diffuse)
        ;

}

fwCampImplementEnumMacro((fwData)(Material)(OPTIONS_MODE))
{
    builder
        .value("SURFACE",  ::fwData::Material::MODE_SURFACE)
        .value("POINT",  ::fwData::Material::MODE_POINT )
        .value("WIREFRAME",  ::fwData::Material::MODE_WIREFRAME)
        .value("EDGE",  ::fwData::Material::MODE_EDGE)
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
        .value("STANDARD", ::fwData::Material::MODE_STANDARD)
        .value("NORMALS", ::fwData::Material::MODE_NORMALS)
        ;
}
