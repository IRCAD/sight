/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwDataTools/helper/MeshGetter.hpp"

#include <fwMath/MeshFunctions.hpp>

namespace fwDataTools
{

namespace helper
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000

MeshGetter::MeshGetter( data::Mesh::csptr mesh ) :
    m_mesh(mesh)
{
    FW_DEPRECATED_MSG("::fwDataTools::helper::MeshGetter is no longer supported, the methods have been moved to "
                      "data::Mesh", "22.0")

    typedef ::fwDataTools::helper::ArrayGetter ArrayGetter;
    SLM_ASSERT("Mesh ptr is null.", mesh);

    SLM_ASSERT("Mesh ptr is null.", m_mesh);
    m_helperPoints          = ArrayGetter::uptr( new ArrayGetter(m_mesh->getPointsArray()));
    m_helperCellTypes       = ArrayGetter::uptr( new ArrayGetter(m_mesh->getCellTypesArray()));
    m_helperCellData        = ArrayGetter::uptr( new ArrayGetter(m_mesh->getCellDataArray()));
    m_helperCellDataOffsets = ArrayGetter::uptr( new ArrayGetter(m_mesh->getCellDataOffsetsArray()));

    data::Array::sptr pointColors    = m_mesh->getPointColorsArray();
    data::Array::sptr cellColors     = m_mesh->getCellColorsArray();
    data::Array::sptr pointNormals   = m_mesh->getPointNormalsArray();
    data::Array::sptr cellNormals    = m_mesh->getCellNormalsArray();
    data::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    data::Array::sptr cellTexCoords  = m_mesh->getCellTexCoordsArray();

    if(pointColors)
    {
        m_helperPointColors = ArrayGetter::uptr( new ArrayGetter(pointColors));
    }
    if(cellColors)
    {
        m_helperCellColors = ArrayGetter::uptr( new ArrayGetter(cellColors));
    }
    if(pointNormals)
    {
        m_helperPointNormals = ArrayGetter::uptr( new ArrayGetter(pointNormals));
    }
    if(cellNormals)
    {
        m_helperCellNormals = ArrayGetter::uptr( new ArrayGetter(cellNormals));
    }
    if(pointTexCoords)
    {
        m_helperPointTexCoords = ArrayGetter::uptr( new ArrayGetter(pointTexCoords));
    }
    if(cellTexCoords)
    {
        m_helperCellTexCoords = ArrayGetter::uptr( new ArrayGetter(cellTexCoords));
    }
}

//-----------------------------------------------------------------------------

MeshGetter::~MeshGetter()
{
}

//------------------------------------------------------------------------------

data::Mesh::ConstPointsMultiArrayType MeshGetter::getPoints() const
{
    return data::Mesh::ConstPointsMultiArrayType(
        static_cast< const data::Mesh::PointsMultiArrayType::element* >(m_helperPoints->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][3]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstCellTypesMultiArrayType MeshGetter::getCellTypes() const
{
    return data::Mesh::ConstCellTypesMultiArrayType(
        static_cast< const data::Mesh::CellTypesMultiArrayType::element*> (m_helperCellTypes->getBuffer()),
        boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstCellDataMultiArrayType MeshGetter::getCellData() const
{
    return data::Mesh::ConstCellDataMultiArrayType(
        static_cast< const data::Mesh::CellDataMultiArrayType::element* >(m_helperCellData->getBuffer()),
        ::boost::extents[m_mesh->getCellDataSize()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstCellDataOffsetsMultiArrayType MeshGetter::getCellDataOffsets() const
{
    return data::Mesh::ConstCellDataOffsetsMultiArrayType(
        static_cast< const data::Mesh::CellDataOffsetsMultiArrayType::element*>(m_helperCellDataOffsets->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstPointColorsMultiArrayType MeshGetter::getPointColors() const
{
    data::Array::sptr pointColors = m_mesh->getPointColorsArray();
    return data::Mesh::ConstPointColorsMultiArrayType(
        static_cast< const data::Mesh::PointColorsMultiArrayType::element*>(m_helperPointColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointColors->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstCellColorsMultiArrayType MeshGetter::getCellColors() const
{
    data::Array::sptr cellColors = m_mesh->getCellColorsArray();
    return data::Mesh::ConstCellColorsMultiArrayType(
        static_cast< const data::Mesh::CellColorsMultiArrayType::element*>(m_helperCellColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellColors->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstPointNormalsMultiArrayType MeshGetter::getPointNormals() const
{
    data::Array::sptr pointNormals = m_mesh->getPointNormalsArray();
    return data::Mesh::ConstPointNormalsMultiArrayType(
        static_cast< const data::Mesh::PointNormalsMultiArrayType::element*>(m_helperPointNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointNormals->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstCellNormalsMultiArrayType MeshGetter::getCellNormals() const
{
    data::Array::sptr cellNormals = m_mesh->getCellNormalsArray();
    return data::Mesh::ConstCellNormalsMultiArrayType(
        static_cast< const data::Mesh::CellNormalsMultiArrayType::element*>(m_helperCellNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellNormals->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstPointTexCoordsMultiArrayType MeshGetter::getPointTexCoords() const
{
    data::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    return data::Mesh::ConstPointTexCoordsMultiArrayType(
        static_cast< const data::Mesh::PointTexCoordsMultiArrayType::element*>(m_helperPointTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointTexCoords->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::ConstCellTexCoordsMultiArrayType MeshGetter::getCellTexCoords() const
{
    data::Array::sptr cellTexCoords = m_mesh->getCellTexCoordsArray();
    return data::Mesh::ConstCellTexCoordsMultiArrayType(
        static_cast< const data::Mesh::CellTexCoordsMultiArrayType::element*>(m_helperCellTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellTexCoords->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::csptr MeshGetter::getMesh() const
{
    return m_mesh;
}

//------------------------------------------------------------------------------

bool MeshGetter::isClosed()
{
    bool isClosed = false;

    data::Mesh::Size cellDataSize = m_mesh->getCellDataSize();
    data::Mesh::Size nbOfCells    = m_mesh->getNumberOfCells();

    auto cellDataBegin        = m_helperCellData->begin< data::Mesh::CellId >();
    auto cellDataEnd          = cellDataBegin + cellDataSize;
    auto cellDataOffsetsBegin = m_helperCellDataOffsets->begin< data::Mesh::CellId >();
    auto cellDataOffsetsEnd   = cellDataOffsetsBegin + nbOfCells;
    auto cellTypesBegin       = m_helperCellTypes->begin< data::Mesh::CellTypes >();

    isClosed = ::fwMath::isBorderlessSurface(cellDataBegin,
                                             cellDataEnd, cellDataOffsetsBegin,
                                             cellDataOffsetsEnd, cellTypesBegin );
    return isClosed;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace fwDataTools
