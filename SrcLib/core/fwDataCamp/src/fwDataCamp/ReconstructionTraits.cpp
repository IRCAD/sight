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
#include "fwDataCamp/ReconstructionTraits.hpp"

fwCampImplementDataMacro((fwData)(ReconstructionTraits))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("name", &::fwData::ReconstructionTraits::m_identifier)
    .property("mesh_node", &::fwData::ReconstructionTraits::m_meshOpNode)
    .property("mask_node", &::fwData::ReconstructionTraits::m_maskOpNode)
    .property("traits", &::fwData::ReconstructionTraits::m_structureTraits)
    ;
}
