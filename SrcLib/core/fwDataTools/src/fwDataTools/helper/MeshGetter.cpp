/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

MeshGetter::MeshGetter( ::fwData::Mesh::csptr mesh ) :
    m_mesh(mesh)
{
    typedef ::fwDataTools::helper::ArrayGetter ArrayGetter;
    SLM_ASSERT("Mesh ptr is null.", mesh);

    SLM_ASSERT("Mesh ptr is null.", m_mesh);
    m_helperPoints          = ArrayGetter::uptr( new ArrayGetter(m_mesh->getPointsArray()));
    m_helperCellTypes       = ArrayGetter::uptr( new ArrayGetter(m_mesh->getCellTypesArray()));
    m_helperCellData        = ArrayGetter::uptr( new ArrayGetter(m_mesh->getCellDataArray()));
    m_helperCellDataOffsets = ArrayGetter::uptr( new ArrayGetter(m_mesh->getCellDataOffsetsArray()));

    ::fwData::Array::sptr pointColors    = m_mesh->getPointColorsArray();
    ::fwData::Array::sptr cellColors     = m_mesh->getCellColorsArray();
    ::fwData::Array::sptr pointNormals   = m_mesh->getPointNormalsArray();
    ::fwData::Array::sptr cellNormals    = m_mesh->getCellNormalsArray();
    ::fwData::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    ::fwData::Array::sptr cellTexCoords  = m_mesh->getCellTexCoordsArray();

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

::fwData::Mesh::ConstPointsMultiArrayType MeshGetter::getPoints() const
{
    return ::fwData::Mesh::ConstPointsMultiArrayType(
        static_cast< const ::fwData::Mesh::PointsMultiArrayType::element* >(m_helperPoints->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][3]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstCellTypesMultiArrayType MeshGetter::getCellTypes() const
{
    return ::fwData::Mesh::ConstCellTypesMultiArrayType(
        static_cast< const ::fwData::Mesh::CellTypesMultiArrayType::element*> (m_helperCellTypes->getBuffer()),
        boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstCellDataMultiArrayType MeshGetter::getCellData() const
{
    return ::fwData::Mesh::ConstCellDataMultiArrayType(
        static_cast< const ::fwData::Mesh::CellDataMultiArrayType::element* >(m_helperCellData->getBuffer()),
        ::boost::extents[m_mesh->getCellDataSize()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstCellDataOffsetsMultiArrayType MeshGetter::getCellDataOffsets() const
{
    return ::fwData::Mesh::ConstCellDataOffsetsMultiArrayType(
        static_cast< const ::fwData::Mesh::CellDataOffsetsMultiArrayType::element*>(m_helperCellDataOffsets->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstPointColorsMultiArrayType MeshGetter::getPointColors() const
{
    ::fwData::Array::sptr pointColors = m_mesh->getPointColorsArray();
    return ::fwData::Mesh::ConstPointColorsMultiArrayType(
        static_cast< const ::fwData::Mesh::PointColorsMultiArrayType::element*>(m_helperPointColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointColors->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstCellColorsMultiArrayType MeshGetter::getCellColors() const
{
    ::fwData::Array::sptr cellColors = m_mesh->getCellColorsArray();
    return ::fwData::Mesh::ConstCellColorsMultiArrayType(
        static_cast< const ::fwData::Mesh::CellColorsMultiArrayType::element*>(m_helperCellColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellColors->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstPointNormalsMultiArrayType MeshGetter::getPointNormals() const
{
    ::fwData::Array::sptr pointNormals = m_mesh->getPointNormalsArray();
    return ::fwData::Mesh::ConstPointNormalsMultiArrayType(
        static_cast< const ::fwData::Mesh::PointNormalsMultiArrayType::element*>(m_helperPointNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointNormals->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstCellNormalsMultiArrayType MeshGetter::getCellNormals() const
{
    ::fwData::Array::sptr cellNormals = m_mesh->getCellNormalsArray();
    return ::fwData::Mesh::ConstCellNormalsMultiArrayType(
        static_cast< const ::fwData::Mesh::CellNormalsMultiArrayType::element*>(m_helperCellNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellNormals->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstPointTexCoordsMultiArrayType MeshGetter::getPointTexCoords() const
{
    ::fwData::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    return ::fwData::Mesh::ConstPointTexCoordsMultiArrayType(
        static_cast< const ::fwData::Mesh::PointTexCoordsMultiArrayType::element*>(m_helperPointTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointTexCoords->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::ConstCellTexCoordsMultiArrayType MeshGetter::getCellTexCoords() const
{
    ::fwData::Array::sptr cellTexCoords = m_mesh->getCellTexCoordsArray();
    return ::fwData::Mesh::ConstCellTexCoordsMultiArrayType(
        static_cast< const ::fwData::Mesh::CellTexCoordsMultiArrayType::element*>(m_helperCellTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellTexCoords->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::csptr MeshGetter::getMesh() const
{
    return m_mesh;
}

//------------------------------------------------------------------------------

bool MeshGetter::isClosed()
{
    bool isClosed = false;

    ::fwData::Mesh::Id cellDataSize = m_mesh->getCellDataSize();
    ::fwData::Mesh::Id nbOfCells    = m_mesh->getNumberOfCells();

    auto cellDataBegin        = m_helperCellData->begin< ::fwData::Mesh::CellValueType >();
    auto cellDataEnd          = cellDataBegin + cellDataSize;
    auto cellDataOffsetsBegin = m_helperCellDataOffsets->begin< ::fwData::Mesh::CellDataOffsetType >();
    auto cellDataOffsetsEnd   = cellDataOffsetsBegin + nbOfCells;
    auto cellTypesBegin       = m_helperCellTypes->begin< ::fwData::Mesh::CellTypes >();

    isClosed = ::fwMath::isBorderlessSurface(cellDataBegin,
                                             cellDataEnd, cellDataOffsetsBegin,
                                             cellDataOffsetsEnd, cellTypesBegin );
    return isClosed;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace fwDataTools
