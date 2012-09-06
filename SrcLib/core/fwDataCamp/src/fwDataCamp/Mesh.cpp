/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Mesh.hpp"

fwCampImplementDataMacro((fwData)(Mesh))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwData")
        .base< ::fwData::Object>()
        .property("nb_points"        , &::fwData::Mesh::m_nbPoints)
        .property("nb_cells"         , &::fwData::Mesh::m_nbCells)
        .property("cells_data_size"   , &::fwData::Mesh::m_cellsDataSize)
        .property("points"          , &::fwData::Mesh::m_points)
        .property("cell_types"       , &::fwData::Mesh::m_cellTypes)
        .property("cell_data"       , &::fwData::Mesh::m_cellData)
        .property("cell_data_offsets" , &::fwData::Mesh::m_cellDataOffsets)
        .property("point_colors"     , &::fwData::Mesh::m_pointColors)
        .property("cell_colors"      , &::fwData::Mesh::m_cellColors)
        .property("point_normals"    , &::fwData::Mesh::m_pointNormals)
        .property("cell_normals"     , &::fwData::Mesh::m_cellNormals)
        ;
}
