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
#include "fwDataCamp/Reconstruction.hpp"

fwCampImplementDataMacro((fwData)(Reconstruction))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("is_visible", &::fwData::Reconstruction::m_bIsVisible)
    .property("organ_name", &::fwData::Reconstruction::m_sOrganName)
    .property("structure_type", &::fwData::Reconstruction::m_sStructureType)
    .property("material", &::fwData::Reconstruction::m_material)
    .property("image", &::fwData::Reconstruction::m_image)
    .property("mesh", &::fwData::Reconstruction::m_mesh)
    .property("volume", &::fwData::Reconstruction::m_computedMaskVolume)
    ;
}
