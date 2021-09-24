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

#include "data/detail/reflection/Mesh.hpp"

#include <core/reflection/UserObject.hpp>

SIGHT_IMPLEMENT_DATA_REFLECTION((sight) (data) (Mesh))
{
    builder
    .tag("object_version", "4")
    .tag("lib_name", "sight::data")
    .base<sight::data::Object>()
    .property("nb_points", &sight::data::Mesh::m_nbPoints)
    .property("nb_cells", &sight::data::Mesh::m_nbCells)
    .property("cells_data_size", &sight::data::Mesh::m_cellsDataSize)
    .property("points", &sight::data::Mesh::m_points)
    .property("cell_types", &sight::data::Mesh::m_cellTypes)
    .property("cell_data", &sight::data::Mesh::m_cellData)
    .property("cell_data_offsets", &sight::data::Mesh::m_cellDataOffsets)
    .property("point_colors", &sight::data::Mesh::m_pointColors)
    .property("cell_colors", &sight::data::Mesh::m_cellColors)
    .property("point_normals", &sight::data::Mesh::m_pointNormals)
    .property("cell_normals", &sight::data::Mesh::m_cellNormals)
    .property("point_tex_coords", &sight::data::Mesh::m_pointTexCoords)
    .property("cell_tex_coords", &sight::data::Mesh::m_cellTexCoords)
    .property("array_map", &sight::data::Mesh::m_arrayMap)
    //.property("attributes", &sight::data::Mesh::m_attributes) // It uses a mapper to convert attributes property.
    ;
}
