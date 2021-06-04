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

#include "data/detail/reflection/StructureTraits.hpp"

#include <core/reflection/UserObject.hpp>

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (StructureTraits))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "::sight::data")
    .base< ::sight::data::Object>()
    .property("property_type", &::sight::data::StructureTraits::m_propertyType)
    .property("type", &::sight::data::StructureTraits::m_type)
    .property("categories", &::sight::data::StructureTraits::m_categories)
    .property("color", &::sight::data::StructureTraits::m_color)
    .property("class", &::sight::data::StructureTraits::m_class)
    .property("native_exp", &::sight::data::StructureTraits::m_nativeExp)
    .property("native_geomertic_exp", &::sight::data::StructureTraits::m_nativeGeometricExp)
    .property("attachment_type", &::sight::data::StructureTraits::m_attachmentType)
    .property("anatomic_region", &::sight::data::StructureTraits::m_anatomicRegion)
    .property("property_category", &::sight::data::StructureTraits::m_propertyCategory)
    ;
}

SIGHT_IMPLEMENT_ENUM_REFLECTION((sight) (data) (StructureTraits) (Category))
{
    builder
    .value("BODY", sight::data::StructureTraits::BODY)
    .value("HEAD", sight::data::StructureTraits::HEAD)
    .value("NECK", sight::data::StructureTraits::NECK)
    .value("THORAX", sight::data::StructureTraits::THORAX)
    .value("ABDOMEN", sight::data::StructureTraits::ABDOMEN)
    .value("PELVIS", sight::data::StructureTraits::PELVIS)
    .value("ARM", sight::data::StructureTraits::ARM)
    .value("LEG", sight::data::StructureTraits::LEG)
    .value("LIVER_SEGMENTS", sight::data::StructureTraits::LIVER_SEGMENTS)
    .value("OTHER", sight::data::StructureTraits::OTHER)
    ;
}

SIGHT_IMPLEMENT_ENUM_REFLECTION((sight) (data) (StructureTraits) (StructureClass))
{
    builder
    .value("TOOL", sight::data::StructureTraits::TOOL)
    .value("ENVIRONMENT", sight::data::StructureTraits::ENVIRONMENT)
    .value("VESSEL", sight::data::StructureTraits::VESSEL)
    .value("LESION", sight::data::StructureTraits::LESION)
    .value("ORGAN", sight::data::StructureTraits::ORGAN)
    .value("FUNCTIONAL", sight::data::StructureTraits::FUNCTIONAL)
    .value("NO_CONSTRAINT", sight::data::StructureTraits::NO_CONSTRAINT)
    ;
}
