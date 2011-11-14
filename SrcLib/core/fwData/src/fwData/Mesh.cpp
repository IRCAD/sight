/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <cstdlib>
#include <functional>
#include <numeric>

#include <boost/assign/list_of.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Mesh.hpp"

using namespace boost::assign;

namespace fwData
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Mesh, ::fwData::Mesh);

//------------------------------------------------------------------------------

Mesh::Mesh()
{
    m_nbPoints    = 0;
    m_nbCells     = 0;
    m_cellsDataSize = 0;
}

//------------------------------------------------------------------------------

Mesh::~Mesh()
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::shallowCopy( Mesh::csptr _source )
{
    this->::fwTools::Object::shallowCopyOfChildren( _source );

    //TODO
}

//------------------------------------------------------------------------------

void Mesh::deepCopy( Mesh::csptr _source )
{
    this->::fwTools::Object::deepCopyOfChildren( _source );

    //TODO
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

size_t Mesh::allocate(size_t nbPts, size_t nbCells, size_t nbCellsData) throw(::fwData::Exception)
{
    m_points          = ::fwData::Array::New();
    m_cellTypes       = ::fwData::Array::New();
    m_cellData        = ::fwData::Array::New();
    m_cellDataOffsets = ::fwData::Array::New();

    if (nbCellsData == 0)
    {
        nbCellsData = 3*nbCells;
    }

    size_t allocatedSize = 0;

    allocatedSize += m_points->allocate("float", list_of(nbPts), 3);
    allocatedSize += m_cellTypes->allocate("char", list_of(nbCells), 1);
    allocatedSize += m_cellData->allocate("uint64_t", list_of(nbCellsData), 1);
    allocatedSize += m_cellDataOffsets->allocate("uint64_t", list_of(nbCells), 1);

    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointNormals() throw(::fwData::Exception)
{
// TODO
    return 0;
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointColors() throw(::fwData::Exception)
{
// TODO
    return 0;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellNormals() throw(::fwData::Exception)
{
// TODO
    return 0;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellColors() throw(::fwData::Exception)
{
// TODO
    return 0;
}

//------------------------------------------------------------------------------

void Mesh::stripAllocatedMemory() throw(::fwData::Exception)
{
// TODO
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::insertNextPoint(const PointValueType p[3]) throw(::fwData::Exception)
{
    size_t allocatedPts = m_points->getSize().at(0);
    if( allocatedPts <= m_nbPoints )
    {
        m_points->resize(list_of(allocatedPts + POINT_REALLOC_STEP), true);
    }

    m_points->setItem(list_of(m_nbPoints), p);
    return m_nbPoints++;
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::insertNextPoint(PointValueType x, PointValueType y, PointValueType z) throw(::fwData::Exception)
{
    const PointValueType p[3] = {x,y,z};
    return insertNextPoint(p);
}

//------------------------------------------------------------------------------

void Mesh::setPoint(Id id, PointValueType x, PointValueType y, PointValueType z)
{
// TODO
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::insertNextCell(CellTypes type, const CellValueType *cell, size_t nb) throw(::fwData::Exception)
{
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'NONE'", type == NONE && nb == 0);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POINT'", type == POINT && nb == 1);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'EDGE'", type == EDGE && nb == 2);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'TRIANGLE'", type == TRIANGLE && nb == 3);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'QUAD'", type == QUAD && nb == 4);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POLY'", type == POLY && nb > 4);

    size_t allocatedCellTypes       = m_cellTypes->getSize().at(0);
    size_t allocatedCellDataOffsets = m_cellDataOffsets->getSize().at(0);

    if( allocatedCellTypes <= m_nbCells )
    {
        m_cellTypes->resize(list_of(allocatedCellTypes + CELL_REALLOC_STEP), true);
    }
    if( allocatedCellDataOffsets <= m_nbCells )
    {
        m_cellDataOffsets->resize(list_of(allocatedCellDataOffsets + CELL_REALLOC_STEP), true);
    }


    size_t allocatedCellData = m_cellData->getSize().at(0);

    if( allocatedCellData + nb <= m_cellsDataSize )
    {
        m_cellData->resize(list_of(allocatedCellData + CELLDATA_REALLOC_STEP), true);
    }


    const CellTypes t[1] = {type};
    m_cellTypes->setItem(list_of(m_nbCells), t);

    Id *buf = reinterpret_cast<Id*>(m_cellData->getBufferPtr(list_of(m_cellsDataSize), 0, sizeof(Id)));
    std::copy(cell, cell+nb, buf);

    const Id id[1] = {m_cellsDataSize};
    m_cellDataOffsets->setItem(list_of(m_nbCells), id);

    m_cellsDataSize += nb;

    return m_nbCells++;
}

//------------------------------------------------------------------------------


Mesh::Id Mesh::insertNextCell(CellValueType p) throw(::fwData::Exception)
{
    CellValueType point[1] = {p};
    return insertNextCell(POINT, point, 1);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::insertNextCell(CellValueType p1, CellValueType p2) throw(::fwData::Exception)
{
    CellValueType p[2] = {p1, p2};
    return insertNextCell(EDGE, p, 2);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3) throw(::fwData::Exception)
{
    CellValueType p[3] = {p1, p2, p3};
    return insertNextCell(TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::insertNextCell(CellValueType p1, CellValueType p2, CellValueType p3, CellValueType p4) throw(::fwData::Exception)
{
    CellValueType p[4] = {p1, p2, p3, p4};
    return insertNextCell(QUAD, p, 1);
}

//------------------------------------------------------------------------------

void Mesh::cleanCells()
{
// TODO
}

//------------------------------------------------------------------------------

Mesh::PointMultiArrayType Mesh::getPoints() const
{
    return PointMultiArrayType(
            static_cast<PointMultiArrayType::element*>(m_points->getBuffer()),
            boost::extents[m_nbPoints][3]
            );
}

//------------------------------------------------------------------------------

Mesh::CellTypesMultiArrayType Mesh::getCellTypes() const
{
    return CellTypesMultiArrayType(
            static_cast<CellTypesMultiArrayType::element *>(m_cellTypes->getBuffer()),
            boost::extents[m_nbCells]
            );
}

//------------------------------------------------------------------------------

Mesh::CellDataMultiArrayType Mesh::getCellData() const
{
    return CellDataMultiArrayType(
            static_cast<CellDataMultiArrayType::element *>(m_cellData->getBuffer()),
            boost::extents[m_cellsDataSize]
            );
}

//------------------------------------------------------------------------------

Mesh::CellDataOffsetsMultiArrayType Mesh::getCellDataOffsets() const
{
    return CellDataOffsetsMultiArrayType(
            static_cast<CellDataOffsetsMultiArrayType::element *>(m_cellDataOffsets->getBuffer()),
            boost::extents[m_nbCells]
            );
}

//------------------------------------------------------------------------------

//Mesh::PointColorsMultiArrayType Mesh::getPointColors() const
//{
//// TODO
//}

////------------------------------------------------------------------------------

//Mesh::CellColorsMultiArrayType Mesh::getCellColors() const
//{
//// TODO
//}

////------------------------------------------------------------------------------

//Mesh::PointNormalsMultiArrayType Mesh::getPointNormals() const
//{
//// TODO
//}

////------------------------------------------------------------------------------

//Mesh::CellNormalsMultiArrayType Mesh::getCellNormals() const
//{
//// TODO
//}

//------------------------------------------------------------------------------

void Mesh::setPointArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::setCellTypesArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::setCellDataArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::setCellDataOffsetsArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::setPointColorsArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::setCellColorsArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::setPointNormalsArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::setCellNormalsArray (::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getPointsArray () const
{
    return m_points;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellTypesArray () const
{
    return m_cellTypes;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellDataArray () const
{
    return m_cellData;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellDataOffsetsArray () const
{
    return m_cellDataOffsets;
}

//------------------------------------------------------------------------------

//::fwData::Array::sptr Mesh::getPointColorsArray () const
//{
    //return m_pointColors;
//}

////------------------------------------------------------------------------------

//::fwData::Array::sptr Mesh::getCellColorsArray () const
//{
    //return m_cellColors;
//}

////------------------------------------------------------------------------------

//::fwData::Array::sptr Mesh::getPointNormalsArray () const
//{
    //return m_pointNormals;
//}

////------------------------------------------------------------------------------

//::fwData::Array::sptr Mesh::getCellNormalsArray () const
//{
    //return m_cellNormals;
//}

//------------------------------------------------------------------------------

void Mesh::clearPoints()
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::clearCells()
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::clear()
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::clearPointNormals()
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::clearPointColors()
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::clearCellNormals()
{
// TODO
}

//------------------------------------------------------------------------------

void Mesh::clearCellColors()
{
// TODO
}

//------------------------------------------------------------------------------

size_t Mesh::getNumberOfPoints() const
{
    return m_nbPoints;
}

//------------------------------------------------------------------------------

size_t Mesh::getNumberOfCells() const
{
    return m_nbCells;
}

//------------------------------------------------------------------------------

size_t Mesh::getDataSizeInBytes() const
{
    //TODO
    size_t size = 0;

    m_points         && (size += m_points->getElementSizeInBytes() * m_nbPoints);
    m_cellTypes      && (size += m_cellTypes->getElementSizeInBytes() * m_nbCells );
    m_cellData       && (size += m_cellData->getElementSizeInBytes() * m_cellsDataSize);
    m_cellDataOffsets&& (size += m_cellDataOffsets->getElementSizeInBytes() * m_nbCells);
    m_pointColors    && (size += m_pointColors->getElementSizeInBytes() * m_nbPoints);
    m_cellColors     && (size += m_cellColors->getElementSizeInBytes() * m_nbCells);
    m_pointNormals   && (size += m_pointNormals->getElementSizeInBytes() * m_nbPoints);
    m_cellNormals    && (size += m_cellNormals->getElementSizeInBytes() * m_nbCells);

    return size;
}

//------------------------------------------------------------------------------

size_t Mesh::getAllocatedSizeInBytes() const
{
    size_t size = 0;

    m_points          && (size += m_points->getSizeInBytes());
    m_cellTypes       && (size += m_cellTypes->getSizeInBytes() );
    m_cellData        && (size += m_cellData->getSizeInBytes());
    m_cellDataOffsets && (size += m_cellDataOffsets->getSizeInBytes());
    m_pointColors     && (size += m_pointColors->getSizeInBytes());
    m_cellColors      && (size += m_cellColors->getSizeInBytes());
    m_pointNormals    && (size += m_pointNormals->getSizeInBytes());
    m_cellNormals     && (size += m_cellNormals->getSizeInBytes());

    return size;
}

//------------------------------------------------------------------------------

void Mesh::addDataArray(const std::string &name, ::fwData::Array::sptr array)
{
// TODO
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getDataArray(const std::string &name) const
{
    ::fwData::Array::sptr res;
    ArrayMapType::const_iterator iter = m_arrayMap.find(name);
    if( iter != m_arrayMap.end())
    {
        res = iter->second;
    }
    return res;
}

//------------------------------------------------------------------------------

void Mesh::removeDataArray(const std::string &name)
{
// TODO
}

//------------------------------------------------------------------------------





}//namespace fwData
