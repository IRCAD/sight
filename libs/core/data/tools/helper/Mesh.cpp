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

#include "data/tools/helper/Mesh.hpp"

namespace sight::data::tools
{

namespace helper
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000

Mesh::Mesh( data::Mesh::sptr mesh ) :
    m_mesh(mesh)
{
    FW_DEPRECATED_MSG("::data::tools::helper::Mesh is no longer supported, the methods have been moved to "
                      "::sight::data::Mesh", "22.0")
    SLM_ASSERT("Mesh ptr is null.", mesh);
    this->updateLock();
}

//-----------------------------------------------------------------------------

Mesh::~Mesh()
{
}

//-----------------------------------------------------------------------------

void Mesh::updateLock()
{
    SLM_ASSERT("Mesh ptr is null.", m_mesh);
    m_mesh->getPointsArray()->setNumberOfComponents(3);
    m_helperPoints          = data::tools::helper::Array::New(m_mesh->getPointsArray());
    m_helperCellTypes       = data::tools::helper::Array::New(m_mesh->getCellTypesArray());
    m_helperCellData        = data::tools::helper::Array::New(m_mesh->getCellDataArray());
    m_helperCellDataOffsets = data::tools::helper::Array::New(m_mesh->getCellDataOffsetsArray());

    data::Array::sptr pointColors    = m_mesh->getPointColorsArray();
    data::Array::sptr cellColors     = m_mesh->getCellColorsArray();
    data::Array::sptr pointNormals   = m_mesh->getPointNormalsArray();
    data::Array::sptr cellNormals    = m_mesh->getCellNormalsArray();
    data::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    data::Array::sptr cellTexCoords  = m_mesh->getCellTexCoordsArray();

    if(pointColors)
    {
        m_helperPointColors = data::tools::helper::Array::New(pointColors);
    }
    if(cellColors)
    {
        m_helperCellColors = data::tools::helper::Array::New(cellColors);
    }
    if(pointNormals)
    {
        m_helperPointNormals = data::tools::helper::Array::New(pointNormals);
    }
    if(cellNormals)
    {
        m_helperCellNormals = data::tools::helper::Array::New(cellNormals);
    }
    if(pointTexCoords)
    {
        m_helperPointTexCoords = data::tools::helper::Array::New(pointTexCoords);
    }
    if(cellTexCoords)
    {
        m_helperCellTexCoords = data::tools::helper::Array::New(cellTexCoords);
    }
}

//-----------------------------------------------------------------------------

data::Mesh::PointId Mesh::insertNextPoint(const data::Mesh::PointValueType p[3])
{
    data::Mesh::Size nbPoints = m_mesh->getNumberOfPoints();
    data::Array::sptr points  = m_mesh->getPointsArray();
    size_t allocatedPts       = points->empty() ? 0 : points->getSize().at(0);
    if( allocatedPts <= nbPoints )
    {
        points->resize({allocatedPts + POINT_REALLOC_STEP}, true);
    }
    m_helperPoints->setItem({nbPoints}, p);
    m_mesh->setNumberOfPoints(nbPoints+1);
    return nbPoints;
}

//------------------------------------------------------------------------------

data::Mesh::PointId Mesh::insertNextPoint(data::Mesh::PointValueType x,
                                          data::Mesh::PointValueType y,
                                          data::Mesh::PointValueType z)
{
    const data::Mesh::PointValueType p[3] = {x, y, z};
    return this->insertNextPoint(p);
}

//------------------------------------------------------------------------------

void Mesh::setPoint(data::Mesh::PointId id, const data::Mesh::PointValueType p[3])
{
    m_helperPoints->setItem( {id}, p);
}

//------------------------------------------------------------------------------

void Mesh::setPoint(data::Mesh::PointId id,
                    data::Mesh::PointValueType x,
                    data::Mesh::PointValueType y,
                    data::Mesh::PointValueType z )
{
    const data::Mesh::PointValueType p[3] = {x, y, z};
    this->setPoint(id, p);
}

//------------------------------------------------------------------------------

void Mesh::setPointColor(data::Mesh::PointId id, const data::Mesh::ColorValueType c[4])
{
    m_helperPointColors->setItem({ id }, c);
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(data::Mesh::CellId id, const data::Mesh::ColorValueType c[4])
{
    m_helperCellColors->setItem({ id }, c);
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(data::Mesh::PointId id, const data::Mesh::NormalValueType n[3])
{
    m_helperPointNormals->setItem({ id }, n);
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(data::Mesh::CellId id, const data::Mesh::NormalValueType n[3])
{
    m_helperCellNormals->setItem({ id }, n);
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoord(data::Mesh::PointId id, const data::Mesh::TexCoordValueType t[2])
{
    m_helperPointTexCoords->setItem({ id }, t);
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoord(data::Mesh::CellId id, const data::Mesh::TexCoordValueType t[2])
{
    m_helperCellTexCoords->setItem({ id }, t);
}

//------------------------------------------------------------------------------

data::Mesh::CellId Mesh::insertNextCell(data::Mesh::CellTypesEnum type,
                                        const data::Mesh::CellId* cell,
                                        size_t nb)
{
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'NO_CELL'",
               type != data::Mesh::NO_CELL || nb == 0);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POINT'",
               type != data::Mesh::POINT || nb == 1);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'EDGE'",
               type != data::Mesh::EDGE || nb == 2);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'TRIANGLE'",
               type != data::Mesh::TRIANGLE || nb == 3);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'QUAD'",
               type != data::Mesh::QUAD || nb == 4);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'TETRA'",
               type != data::Mesh::TETRA || nb == 4);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POLY'",
               type != data::Mesh::POLY || nb > 4);

    data::Mesh::Size cellsDataSize    = m_mesh->getCellDataSize();
    data::Mesh::Size nbCells          = m_mesh->getNumberOfCells();
    data::Array::sptr cellTypes       = m_mesh->getCellTypesArray();
    data::Array::sptr cellDataOffsets = m_mesh->getCellDataOffsetsArray();
    data::Array::sptr cellData        = m_mesh->getCellDataArray();

    size_t allocatedCellTypes       = cellTypes->empty() ? 0 : cellTypes->getSize().at(0);
    size_t allocatedCellDataOffsets = cellDataOffsets->empty() ? 0 : cellDataOffsets->getSize().at(0);

    if( allocatedCellTypes <= nbCells )
    {
        cellTypes->resize({allocatedCellTypes + CELL_REALLOC_STEP}, true);
    }
    if( allocatedCellDataOffsets <= nbCells )
    {
        cellDataOffsets->resize({allocatedCellDataOffsets + CELL_REALLOC_STEP}, true);
    }

    size_t allocatedCellData = cellData->empty() ? 0 : cellData->getSize().at(0);

    if( allocatedCellData <= cellsDataSize + nb )
    {
        cellData->resize({allocatedCellData + CELLDATA_REALLOC_STEP}, true);
    }

    const data::Mesh::CellTypes t[1] = {static_cast< data::Mesh::CellTypes >(type)};
    m_helperCellTypes->setItem({nbCells}, t);

    data::Mesh::CellId* buf = reinterpret_cast< data::Mesh::CellId* >(
        m_helperCellData->getBufferPtr({cellsDataSize}, 0, sizeof(data::Mesh::CellId))
        );
    std::copy(cell, cell+nb, buf);

    const data::Mesh::CellId id[1] = {cellsDataSize};
    m_helperCellDataOffsets->setItem({nbCells}, id);

    cellsDataSize += static_cast<data::Mesh::Size>(nb);
    m_mesh->setCellDataSize(cellsDataSize);
    m_mesh->setNumberOfCells(nbCells + 1);
    return nbCells;
}

//------------------------------------------------------------------------------

data::Mesh::CellId Mesh::insertNextCell(data::Mesh::PointId p)
{
    data::Mesh::PointId point[1] = {p};
    return this->insertNextCell(data::Mesh::POINT, point, 1);
}

//------------------------------------------------------------------------------

data::Mesh::CellId Mesh::insertNextCell(data::Mesh::PointId p1,
                                        data::Mesh::PointId p2)
{
    data::Mesh::PointId p[2] = {p1, p2};
    return this->insertNextCell(data::Mesh::EDGE, p, 2);
}

//------------------------------------------------------------------------------

data::Mesh::CellId Mesh::insertNextCell(data::Mesh::PointId p1,
                                        data::Mesh::PointId p2,
                                        data::Mesh::PointId p3)
{
    data::Mesh::PointId p[3] = {p1, p2, p3};
    return this->insertNextCell(data::Mesh::TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

data::Mesh::CellId Mesh::insertNextCell(data::Mesh::PointId p1,
                                        data::Mesh::PointId p2,
                                        data::Mesh::PointId p3,
                                        data::Mesh::PointId p4,
                                        data::Mesh::CellTypesEnum type)
{
    data::Mesh::PointId p[4] = {p1, p2, p3, p4};

    return this->insertNextCell(type, p, 4);
}

//------------------------------------------------------------------------------

data::Mesh::PointsMultiArrayType Mesh::getPoints() const
{
    return data::Mesh::PointsMultiArrayType(
        static_cast< data::Mesh::PointsMultiArrayType::element* >(m_helperPoints->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][3]
        );
}

//------------------------------------------------------------------------------

data::Mesh::CellTypesMultiArrayType Mesh::getCellTypes() const
{
    return data::Mesh::CellTypesMultiArrayType(
        static_cast< data::Mesh::CellTypesMultiArrayType::element*> (m_helperCellTypes->getBuffer()),
        boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::CellDataMultiArrayType Mesh::getCellData() const
{
    return data::Mesh::CellDataMultiArrayType(
        static_cast< data::Mesh::CellDataMultiArrayType::element* >(m_helperCellData->getBuffer()),
        ::boost::extents[m_mesh->getCellDataSize()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::CellDataOffsetsMultiArrayType Mesh::getCellDataOffsets() const
{
    return data::Mesh::CellDataOffsetsMultiArrayType(
        static_cast< data::Mesh::CellDataOffsetsMultiArrayType::element*>(m_helperCellDataOffsets->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

data::Mesh::PointColorsMultiArrayType Mesh::getPointColors() const
{
    data::Array::sptr pointColors = m_mesh->getPointColorsArray();
    return data::Mesh::PointColorsMultiArrayType(
        static_cast< data::Mesh::PointColorsMultiArrayType::element*>(m_helperPointColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][static_cast<long>(pointColors->getNumberOfComponents())]
        );
}

//------------------------------------------------------------------------------

data::Mesh::CellColorsMultiArrayType Mesh::getCellColors() const
{
    data::Array::sptr cellColors = m_mesh->getCellColorsArray();
    return data::Mesh::CellColorsMultiArrayType(
        static_cast< data::Mesh::CellColorsMultiArrayType::element*>(m_helperCellColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][static_cast<long>(cellColors->getNumberOfComponents())]
        );
}

//------------------------------------------------------------------------------

data::Mesh::PointNormalsMultiArrayType Mesh::getPointNormals() const
{
    data::Array::sptr pointNormals = m_mesh->getPointNormalsArray();
    return data::Mesh::PointNormalsMultiArrayType(
        static_cast< data::Mesh::PointNormalsMultiArrayType::element*>(m_helperPointNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][static_cast<long>(pointNormals->getNumberOfComponents())]
        );
}

//------------------------------------------------------------------------------

data::Mesh::CellNormalsMultiArrayType Mesh::getCellNormals() const
{
    data::Array::sptr cellNormals = m_mesh->getCellNormalsArray();
    return data::Mesh::CellNormalsMultiArrayType(
        static_cast< data::Mesh::CellNormalsMultiArrayType::element*>(m_helperCellNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][static_cast<long>(cellNormals->getNumberOfComponents())]
        );
}

//------------------------------------------------------------------------------

data::Mesh::PointTexCoordsMultiArrayType Mesh::getPointTexCoords() const
{
    data::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    return data::Mesh::PointTexCoordsMultiArrayType(
        static_cast< data::Mesh::PointTexCoordsMultiArrayType::element*>(m_helperPointTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][static_cast<long>(pointTexCoords->getNumberOfComponents())]
        );
}

//------------------------------------------------------------------------------

data::Mesh::CellTexCoordsMultiArrayType Mesh::getCellTexCoords() const
{
    data::Array::sptr cellTexCoords = m_mesh->getCellTexCoordsArray();
    return data::Mesh::CellTexCoordsMultiArrayType(
        static_cast< data::Mesh::CellTexCoordsMultiArrayType::element*>(m_helperCellTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][static_cast<long>(cellTexCoords->getNumberOfComponents())]
        );
}

//------------------------------------------------------------------------------

data::Mesh::csptr Mesh::getMesh() const
{
    return m_mesh;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace sight::data::tools
