/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "data/detail/reflection/Material.hpp"

#include <core/reflection/UserObject.hpp>

fwCampImplementDataMacro((sight)(data)(Material))
{
    builder
    .tag("object_version", "4")
    .tag("lib_name", "::sight::data")
    .base< ::sight::data::Object>()
    .property("shading_mode", &::sight::data::Material::m_shadingMode)
    .property("representation_mode", &::sight::data::Material::m_representationMode)
    .property("options_mode", &::sight::data::Material::m_optionsMode)
    .property("ambient", &::sight::data::Material::m_ambient)
    .property("diffuse", &::sight::data::Material::m_diffuse)
    .property("diffuse_texture", &::sight::data::Material::m_diffuseTexture)
    .property("diffuse_texture_filtering", &::sight::data::Material::m_diffuseTextureFiltering)
    .property("diffuse_texture_wrapping", &::sight::data::Material::m_diffuseTextureWrapping)
    ;
}

fwCampImplementEnumMacro((sight)(data)(Material)(OptionsType))
{
    builder
    .value("STANDARD", sight::data::Material::STANDARD)
    .value("NORMALS", sight::data::Material::NORMALS)
    .value("CELLS_NORMALS", sight::data::Material::CELLS_NORMALS)
    ;
}

fwCampImplementEnumMacro((sight)(data)(Material)(ShadingType))
{
    builder
    .value("AMBIENT", sight::data::Material::AMBIENT)
    .value("FLAT", sight::data::Material::FLAT)
    .value("GOURAUD", sight::data::Material::GOURAUD)
    .value("PHONG", sight::data::Material::PHONG)
    ;
}

fwCampImplementEnumMacro((sight)(data)(Material)(RepresentationType))
{
    builder
    .value("SURFACE", sight::data::Material::SURFACE)
    .value("POINT", sight::data::Material::POINT )
    .value("WIREFRAME", sight::data::Material::WIREFRAME)
    .value("EDGE", sight::data::Material::EDGE)
    ;
}

fwCampImplementEnumMacro((sight)(data)(Material)(FilteringType))
{
    builder
    .value("NEAREST", sight::data::Material::NEAREST)
    .value("LINEAR", sight::data::Material::LINEAR)
    ;
}

fwCampImplementEnumMacro((sight)(data)(Material)(WrappingType))
{
    builder
    .value("CLAMP", sight::data::Material::CLAMP)
    .value("REPEAT", sight::data::Material::REPEAT)
    ;
}
