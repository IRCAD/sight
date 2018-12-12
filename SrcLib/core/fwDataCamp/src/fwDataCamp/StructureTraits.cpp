/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/StructureTraits.hpp"

fwCampImplementDataMacro((fwData)(StructureTraits))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("property_type", &::fwData::StructureTraits::m_propertyType)
    .property("type", &::fwData::StructureTraits::m_type)
    .property("categories", &::fwData::StructureTraits::m_categories)
    .property("color", &::fwData::StructureTraits::m_color)
    .property("class", &::fwData::StructureTraits::m_class)
    .property("native_exp", &::fwData::StructureTraits::m_nativeExp)
    .property("native_geomertic_exp", &::fwData::StructureTraits::m_nativeGeometricExp)
    .property("attachment_type", &::fwData::StructureTraits::m_attachmentType)
    .property("anatomic_region", &::fwData::StructureTraits::m_anatomicRegion)
    .property("property_category", &::fwData::StructureTraits::m_propertyCategory)
    ;
}

fwCampImplementEnumMacro((fwData)(StructureTraits)(Category))
{
    builder
    .value("BODY",  ::fwData::StructureTraits::BODY)
    .value("HEAD",  ::fwData::StructureTraits::HEAD )
    .value("NECK",  ::fwData::StructureTraits::NECK)
    .value("THORAX",  ::fwData::StructureTraits::THORAX)
    .value("ABDOMEN",  ::fwData::StructureTraits::ABDOMEN)
    .value("PELVIS",  ::fwData::StructureTraits::PELVIS)
    .value("ARM",  ::fwData::StructureTraits::ARM)
    .value("LEG",  ::fwData::StructureTraits::LEG)
    .value("LIVER_SEGMENTS",  ::fwData::StructureTraits::LIVER_SEGMENTS)
    .value("OTHER",  ::fwData::StructureTraits::OTHER)
    ;
}

fwCampImplementEnumMacro((fwData)(StructureTraits)(StructureClass))
{
    builder
    .value("TOOL",  ::fwData::StructureTraits::TOOL)
    .value("ENVIRONMENT",  ::fwData::StructureTraits::ENVIRONMENT )
    .value("VESSEL",  ::fwData::StructureTraits::VESSEL)
    .value("LESION",  ::fwData::StructureTraits::LESION)
    .value("ORGAN",  ::fwData::StructureTraits::ORGAN)
    .value("FUNCTIONAL",  ::fwData::StructureTraits::FUNCTIONAL)
    .value("NO_CONSTRAINT",  ::fwData::StructureTraits::NO_CONSTRAINT)
    ;
}
