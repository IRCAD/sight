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
#include "fwDataCamp/Mesh.hpp"

fwCampImplementDataMacro((fwData)(Mesh))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("nb_points", &::fwData::Mesh::m_nbPoints)
    .property("nb_cells", &::fwData::Mesh::m_nbCells)
    .property("cells_data_size", &::fwData::Mesh::m_cellsDataSize)
    .property("points", &::fwData::Mesh::m_points)
    .property("cell_types", &::fwData::Mesh::m_cellTypes)
    .property("cell_data", &::fwData::Mesh::m_cellData)
    .property("cell_data_offsets", &::fwData::Mesh::m_cellDataOffsets)
    .property("point_colors", &::fwData::Mesh::m_pointColors)
    .property("cell_colors", &::fwData::Mesh::m_cellColors)
    .property("point_normals", &::fwData::Mesh::m_pointNormals)
    .property("cell_normals", &::fwData::Mesh::m_cellNormals)
    .property("point_tex_coords", &::fwData::Mesh::m_pointTexCoords)
    .property("cell_tex_coords", &::fwData::Mesh::m_cellTexCoords)
    .property("array_map", &::fwData::Mesh::m_arrayMap)
    ;
}
