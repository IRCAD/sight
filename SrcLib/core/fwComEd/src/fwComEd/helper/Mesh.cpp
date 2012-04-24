/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>

#include "fwComEd/helper/Mesh.hpp"

using namespace boost::assign;

namespace fwComEd
{

namespace helper
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000

Mesh::Mesh( ::fwData::Mesh::sptr mesh ) : m_mesh (mesh)
{
    SLM_ASSERT("Mesh ptr is null.", mesh);
    m_helperPoints          = ::fwComEd::helper::Array::New(mesh->getPointsArray());
    m_helperCellTypes       = ::fwComEd::helper::Array::New(mesh->getCellTypesArray());
    m_helperCellData        = ::fwComEd::helper::Array::New(mesh->getCellDataArray());
    m_helperCellDataOffsets = ::fwComEd::helper::Array::New(mesh->getCellDataOffsetsArray());

    ::fwData::Array::sptr pointColors  =  mesh->getPointColorsArray();
    ::fwData::Array::sptr cellColors   =  mesh->getCellColorsArray();
    ::fwData::Array::sptr pointNormals =  mesh->getPointNormalsArray();
    ::fwData::Array::sptr cellNormals  =  mesh->getCellNormalsArray();

    if(pointColors)
    {
        m_helperPointColors = ::fwComEd::helper::Array::New(pointColors);
    }
    if(cellColors)
    {
        m_helperCellColors = ::fwComEd::helper::Array::New(cellColors);
    }
    if(pointNormals)
    {
        m_helperPointNormals = ::fwComEd::helper::Array::New(pointNormals);
    }
    if(cellNormals)
    {
        m_helperCellNormals = ::fwComEd::helper::Array::New(cellNormals);
    }
}

//-----------------------------------------------------------------------------

Mesh::~Mesh()
{}

//-----------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::insertNextPoint(const ::fwData::Mesh::PointValueType p[3]) throw(::fwData::Exception)
{
    ::fwData::Mesh::Id nbPoints = m_mesh->getNumberOfPoints();
    ::fwData::Array::sptr points = m_mesh->getPointsArray();
    size_t allocatedPts = points->empty() ? 0 : points->getSize().at(0);
    if( allocatedPts <= nbPoints )
    {
        points->resize(list_of(allocatedPts + POINT_REALLOC_STEP), true);
    }
    m_helperPoints->setItem(list_of(nbPoints), p);
    nbPoints++;
    m_mesh->setNumberOfPoints(nbPoints);
    return nbPoints;
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::insertNextPoint(::fwData::Mesh::PointValueType x,
                                         ::fwData::Mesh::PointValueType y,
                                         ::fwData::Mesh::PointValueType z) throw(::fwData::Exception)
{
    const ::fwData::Mesh::PointValueType p[3] = {x,y,z};
    return this->insertNextPoint(p);
}

//------------------------------------------------------------------------------

void Mesh::setPoint(::fwData::Mesh::Id id, const ::fwData::Mesh::PointValueType p[3])
{
    m_helperPoints->setItem(list_of(id), p);
}

//------------------------------------------------------------------------------

void Mesh::setPoint(::fwData::Mesh::Id id,
                    ::fwData::Mesh::PointValueType x,
                    ::fwData::Mesh::PointValueType y,
                    ::fwData::Mesh::PointValueType z )
{
    const ::fwData::Mesh::PointValueType p[3] = {x,y,z};
    this->setPoint(id, p);
}

//------------------------------------------------------------------------------

void Mesh::setPointColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4])
{
    m_helperPointColors->setItem(list_of(id), c);
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4])
{
    m_helperCellColors->setItem(list_of(id), c);
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3])
{
    m_helperPointNormals->setItem(list_of(id), n);
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3])
{
    m_helperCellNormals->setItem(list_of(id), n);
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::insertNextCell(::fwData::Mesh::CellTypesEnum type,
                                        const ::fwData::Mesh::CellValueType *cell,
                                        size_t nb) throw(::fwData::Exception)
{
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'NO_CELL'",
            type != ::fwData::Mesh::NO_CELL || nb == 0);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POINT'",
            type != ::fwData::Mesh::POINT || nb == 1);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'EDGE'",
            type != ::fwData::Mesh::EDGE || nb == 2);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'TRIANGLE'",
            type != ::fwData::Mesh::TRIANGLE || nb == 3);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'QUAD'",
            type != ::fwData::Mesh::QUAD || nb == 4);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POLY'",
            type != ::fwData::Mesh::POLY || nb > 4);

    ::fwData::Mesh::Id cellsDataSize = m_mesh->getCellDataSize();
    ::fwData::Mesh::Id nbCells = m_mesh->getNumberOfCells();
    ::fwData::Array::sptr cellTypes = m_mesh->getCellTypesArray();
    ::fwData::Array::sptr cellDataOffsets = m_mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellData = m_mesh->getCellDataArray();

    size_t allocatedCellTypes       = cellTypes->empty() ? 0 : cellTypes->getSize().at(0);
    size_t allocatedCellDataOffsets = cellDataOffsets->empty() ? 0 : cellDataOffsets->getSize().at(0);

    if( allocatedCellTypes <= nbCells )
    {
        cellTypes->resize(list_of(allocatedCellTypes + CELL_REALLOC_STEP), true);
    }
    if( allocatedCellDataOffsets <= nbCells )
    {
        cellDataOffsets->resize(list_of(allocatedCellDataOffsets + CELL_REALLOC_STEP), true);
    }


    size_t allocatedCellData = cellData->empty() ? 0 : cellData->getSize().at(0);

    if( allocatedCellData <= cellsDataSize + nb )
    {
        cellData->resize(list_of(allocatedCellData + CELLDATA_REALLOC_STEP), true);
    }


    const ::fwData::Mesh::CellTypes t[1] = {static_cast< ::fwData::Mesh::CellTypes >(type)};
    m_helperCellTypes->setItem(list_of(nbCells), t);

    ::fwData::Mesh::CellValueType *buf = reinterpret_cast< ::fwData::Mesh::CellValueType* >(
            m_helperCellData->getBufferPtr(list_of(cellsDataSize), 0, sizeof(::fwData::Mesh::CellValueType))
            );
    std::copy(cell, cell+nb, buf);

    const ::fwData::Mesh::CellDataOffsetType id[1] = {cellsDataSize};
    m_helperCellDataOffsets->setItem(list_of(nbCells), id);


    cellsDataSize += nb;
    m_mesh->setCellDataSize(cellsDataSize);
    nbCells++;
    m_mesh->setNumberOfCells(nbCells);
    return nbCells;
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::insertNextCell(::fwData::Mesh::CellValueType p) throw(::fwData::Exception)
{
    ::fwData::Mesh::CellValueType point[1] = {p};
    return this->insertNextCell(::fwData::Mesh::POINT, point, 1);
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::insertNextCell(::fwData::Mesh::CellValueType p1,
                                        ::fwData::Mesh::CellValueType p2) throw(::fwData::Exception)
{
    ::fwData::Mesh::CellValueType p[2] = {p1, p2};
    return this->insertNextCell(::fwData::Mesh::EDGE, p, 2);
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::insertNextCell(::fwData::Mesh::CellValueType p1,
                                        ::fwData::Mesh::CellValueType p2,
                                        ::fwData::Mesh::CellValueType p3) throw(::fwData::Exception)
{
    ::fwData::Mesh::CellValueType p[3] = {p1, p2, p3};
    return this->insertNextCell(::fwData::Mesh::TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

::fwData::Mesh::Id Mesh::insertNextCell(::fwData::Mesh::CellValueType p1,
                                        ::fwData::Mesh::CellValueType p2,
                                        ::fwData::Mesh::CellValueType p3,
                                        ::fwData::Mesh::CellValueType p4) throw(::fwData::Exception)
{
    ::fwData::Mesh::CellValueType p[4] = {p1, p2, p3, p4};
    return this->insertNextCell(::fwData::Mesh::QUAD, p, 4);
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointsMultiArrayType Mesh::getPoints() const
{
    return ::fwData::Mesh::PointsMultiArrayType(
            static_cast< ::fwData::Mesh::PointsMultiArrayType::element* >(m_helperPoints->getBuffer()),
            ::boost::extents[m_mesh->getNumberOfPoints()][3]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellTypesMultiArrayType Mesh::getCellTypes() const
{
    return ::fwData::Mesh::CellTypesMultiArrayType(
            static_cast< ::fwData::Mesh::CellTypesMultiArrayType::element *> (m_helperCellTypes->getBuffer()),
            boost::extents[m_mesh->getNumberOfCells()]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellDataMultiArrayType Mesh::getCellData() const
{
    return ::fwData::Mesh::CellDataMultiArrayType(
            static_cast< ::fwData::Mesh::CellDataMultiArrayType::element * >(m_helperCellData->getBuffer()),
            ::boost::extents[m_mesh->getCellDataSize()]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellDataOffsetsMultiArrayType Mesh::getCellDataOffsets() const
{
    return ::fwData::Mesh::CellDataOffsetsMultiArrayType(
            static_cast< ::fwData::Mesh::CellDataOffsetsMultiArrayType::element *>(m_helperCellDataOffsets->getBuffer()),
            ::boost::extents[m_mesh->getNumberOfCells()]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointColorsMultiArrayType Mesh::getPointColors() const
{
    ::fwData::Array::sptr pointColors = m_mesh->getPointColorsArray();
    return ::fwData::Mesh::PointColorsMultiArrayType(
            static_cast< ::fwData::Mesh::PointColorsMultiArrayType::element *>(m_helperPointColors->getBuffer()),
            ::boost::extents[m_mesh->getNumberOfPoints()][pointColors->getNumberOfComponents()]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellColorsMultiArrayType Mesh::getCellColors() const
{
    ::fwData::Array::sptr cellColors = m_mesh->getCellColorsArray();
    return ::fwData::Mesh::CellColorsMultiArrayType(
            static_cast< ::fwData::Mesh::CellColorsMultiArrayType::element *>(m_helperCellColors->getBuffer()),
            ::boost::extents[m_mesh->getNumberOfCells()][cellColors->getNumberOfComponents()]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointNormalsMultiArrayType Mesh::getPointNormals() const
{
    ::fwData::Array::sptr pointNormals = m_mesh->getPointNormalsArray();
    return ::fwData::Mesh::PointNormalsMultiArrayType(
            static_cast< ::fwData::Mesh::PointNormalsMultiArrayType::element *>(m_helperPointNormals->getBuffer()),
            ::boost::extents[m_mesh->getNumberOfPoints()][pointNormals->getNumberOfComponents()]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellNormalsMultiArrayType Mesh::getCellNormals() const
{
    ::fwData::Array::sptr cellNormals = m_mesh->getCellNormalsArray();
    return ::fwData::Mesh::CellNormalsMultiArrayType(
            static_cast< ::fwData::Mesh::CellNormalsMultiArrayType::element *>(m_helperCellNormals->getBuffer()),
            ::boost::extents[m_mesh->getNumberOfCells()][cellNormals->getNumberOfComponents()]
            );
}

//------------------------------------------------------------------------------

::fwData::Mesh::csptr Mesh::getMesh() const
{
    return m_mesh;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace fwComEd
