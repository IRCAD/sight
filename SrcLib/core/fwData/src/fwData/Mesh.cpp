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

#include "fwData/Mesh.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hxx>

#include <cstdlib>
#include <functional>
#include <numeric>

namespace fwData
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000

fwDataRegisterMacro( ::fwData::Mesh );

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType Mesh::s_VERTEX_MODIFIED_SIG           = "vertexModified";
const ::fwCom::Signals::SignalKeyType Mesh::s_POINT_COLORS_MODIFIED_SIG     = "pointColorsModified";
const ::fwCom::Signals::SignalKeyType Mesh::s_CELL_COLORS_MODIFIED_SIG      = "cellColorsModified";
const ::fwCom::Signals::SignalKeyType Mesh::s_POINT_NORMALS_MODIFIED_SIG    = "pointNormalsModified";
const ::fwCom::Signals::SignalKeyType Mesh::s_CELL_NORMALS_MODIFIED_SIG     = "cellNormalsModified";
const ::fwCom::Signals::SignalKeyType Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG = "pointTexCoordsModified";
const ::fwCom::Signals::SignalKeyType Mesh::s_CELL_TEX_COORDS_MODIFIED_SIG  = "cellTexCoordsModified";

//------------------------------------------------------------------------------

Mesh::Mesh(::fwData::Object::Key) :
    m_nbPoints(0),
    m_nbCells(0),
    m_cellsDataSize(0)
{
    newSignal<VertexModifiedSignalType>(s_VERTEX_MODIFIED_SIG);
    newSignal<PointColorsModifiedSignalType>(s_POINT_COLORS_MODIFIED_SIG);
    newSignal<CellColorsModifiedSignalType>(s_CELL_COLORS_MODIFIED_SIG);
    newSignal<PointNormalsModifiedSignalType>(s_POINT_NORMALS_MODIFIED_SIG);
    newSignal<CellNormalsModifiedSignalType>(s_CELL_NORMALS_MODIFIED_SIG);
    newSignal<PointTexCoordsModifiedSignalType>(s_POINT_TEX_COORDS_MODIFIED_SIG);
    newSignal<CellTexCoordsModifiedSignalType>(s_CELL_TEX_COORDS_MODIFIED_SIG);

    this->initArrays();
}

//------------------------------------------------------------------------------

Mesh::~Mesh()
{
}

//------------------------------------------------------------------------------

void Mesh::initArrays()
{
    if (!m_points)
    {
        m_points = ::fwData::Array::New();
    }
    if (!m_cellTypes)
    {
        m_cellTypes = ::fwData::Array::New();
    }
    if (!m_cellData)
    {
        m_cellData = ::fwData::Array::New();
    }
    if (!m_cellDataOffsets)
    {
        m_cellDataOffsets = ::fwData::Array::New();
    }

    // TODO sight 22.0: Add a second dimension on the array to replace the deprecated component
    m_points->setType(::fwTools::Type::create<PointValueType>());
    m_cellTypes->setType(::fwTools::Type::create<CellTypes>());
    m_cellData->setType(::fwTools::Type::create<CellValueType>());
    m_cellDataOffsets->setType(::fwTools::Type::create<CellDataOffsetType>());
}

//------------------------------------------------------------------------------

void Mesh::shallowCopy(const Object::csptr& _source )
{
    Mesh::csptr other = Mesh::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_nbPoints      = other->m_nbPoints;
    m_nbCells       = other->m_nbCells;
    m_cellsDataSize = other->m_cellsDataSize;

    m_points          = other->m_points;
    m_cellTypes       = other->m_cellTypes;
    m_cellData        = other->m_cellData;
    m_cellDataOffsets = other->m_cellDataOffsets;

    m_pointColors    = other->m_pointColors;
    m_cellColors     = other->m_cellColors;
    m_pointNormals   = other->m_pointNormals;
    m_cellNormals    = other->m_cellNormals;
    m_cellTexCoords  = other->m_cellTexCoords;
    m_pointTexCoords = other->m_pointTexCoords;

    m_arrayMap = other->m_arrayMap;
}

//------------------------------------------------------------------------------

void Mesh::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Mesh::csptr other = Mesh::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !other );
    this->fieldDeepCopy( _source, cache );

    m_nbPoints      = other->m_nbPoints;
    m_nbCells       = other->m_nbCells;
    m_cellsDataSize = other->m_cellsDataSize;

    this->initArrays();

    m_points          = ::fwData::Object::copy( other->m_points, cache );
    m_cellTypes       = ::fwData::Object::copy( other->m_cellTypes, cache );
    m_cellData        = ::fwData::Object::copy( other->m_cellData, cache );
    m_cellDataOffsets = ::fwData::Object::copy( other->m_cellDataOffsets, cache );

    //Object::copy returns a null object if source object is null
    m_pointColors    = ::fwData::Object::copy( other->m_pointColors, cache );
    m_cellColors     = ::fwData::Object::copy( other->m_cellColors, cache );
    m_pointNormals   = ::fwData::Object::copy( other->m_pointNormals, cache );
    m_cellNormals    = ::fwData::Object::copy( other->m_cellNormals, cache );
    m_cellTexCoords  = ::fwData::Object::copy( other->m_cellTexCoords, cache );
    m_pointTexCoords = ::fwData::Object::copy( other->m_pointTexCoords, cache );

    m_arrayMap.clear();
    for(const ArrayMapType::value_type& element : other->m_arrayMap)
    {
        m_arrayMap[element.first] = ::fwData::Object::copy(element.second, cache);
    }

}

//------------------------------------------------------------------------------

size_t Mesh::reserve(size_t nbPts, size_t nbCells, CellTypesEnum cellType, ExtraArrayType arrayMask)
{
    size_t nbCellsData = 0;
    switch (cellType)
    {
        case POINT:
            nbCellsData = nbCells;
            break;
        case EDGE:
            nbCellsData = 2 * nbCells;
            break;
        case TRIANGLE:
            nbCellsData = 3 * nbCells;
            break;
        case QUAD:
            nbCellsData = 4 * nbCells;
            break;
        case TETRA:
            nbCellsData = 4 * nbCells;
            break;
        default:
            SLM_ERROR("Cannot determine the cell data size to allocate for this type of cell, please use "
                      "resize(size_t nbPts, size_t nbCells, ExtraArrayType arrayMask, size_t nbCellsData)")
            nbCellsData = 3 * nbCells;
            break;
    }

    return this->reserve(nbPts, nbCells, nbCellsData, arrayMask);
}

//------------------------------------------------------------------------------

size_t Mesh::reserve(size_t nbPts, size_t nbCells, size_t nbCellsData, ExtraArrayType arrayMask)
{
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Cannot not allocate empty size"), nbPts == 0 ||
                          nbCells == 0 || nbCellsData == 0);

    m_points->resizeTMP( {nbPts}, 3 );

    // TODO sight 22.0: Add a second dimension on the array to replace the deprecated component
    if (static_cast<int>(arrayMask & ExtraArrayType::POINT_COLORS))
    {
        if (!m_pointColors )
        {
            m_pointColors = ::fwData::Array::New();
        }
        m_pointColors->resizeTMP(::fwTools::Type::s_UINT8, {nbPts}, 4);
    }
    if (static_cast<int>(arrayMask & ExtraArrayType::POINT_NORMALS))
    {
        if (!m_pointNormals )
        {
            m_pointNormals = ::fwData::Array::New();
        }
        m_pointNormals->resizeTMP(::fwTools::Type::s_FLOAT, {nbPts}, 3 );
    }
    if (static_cast<int>(arrayMask & ExtraArrayType::POINT_TEX_COORDS))
    {
        if (!m_pointTexCoords )
        {
            m_pointTexCoords = ::fwData::Array::New();
        }
        m_pointTexCoords->resizeTMP( ::fwTools::Type::s_FLOAT, {nbPts}, 2 );
    }

    m_cellTypes->resize({nbCells});
    m_cellDataOffsets->resize({nbCells});
    m_cellData->resize({nbCellsData});

    if (static_cast<int>(arrayMask & ExtraArrayType::CELL_COLORS))
    {
        if (!m_cellColors )
        {
            m_cellColors = ::fwData::Array::New();
        }
        m_cellColors->resizeTMP( ::fwTools::Type::s_UINT8,  {nbCells}, 4 );
    }

    if (static_cast<int>(arrayMask & ExtraArrayType::CELL_NORMALS))
    {
        if (!m_cellNormals )
        {
            m_cellNormals = ::fwData::Array::New();
        }
        m_cellNormals->resizeTMP( ::fwTools::Type::s_FLOAT, {nbCells}, 3 );
    }
    if (static_cast<int>(arrayMask & ExtraArrayType::CELL_TEX_COORDS))
    {
        if (!m_cellTexCoords )
        {
            m_cellTexCoords = ::fwData::Array::New();
        }
        m_cellTexCoords->resizeTMP( ::fwTools::Type::s_FLOAT, {nbCells}, 2 );
    }

    return this->getAllocatedSizeInBytes();
}

//------------------------------------------------------------------------------

size_t Mesh::resize(size_t nbPts, size_t nbCells, CellTypesEnum cellType, ExtraArrayType arrayMask)
{
    size_t nbCellsData = 0;
    switch (cellType)
    {
        case POINT:
            nbCellsData = nbCells;
            break;
        case EDGE:
            nbCellsData = 2 * nbCells;
            break;
        case TRIANGLE:
            nbCellsData = 3 * nbCells;
            break;
        case QUAD:
            nbCellsData = 4 * nbCells;
            break;
        case TETRA:
            nbCellsData = 4 * nbCells;
            break;
        default:
            SLM_ERROR("Cannot determine the cell data size to allocate for this type of cell, please use "
                      "resize(size_t nbPts, size_t nbCells, ExtraArrayType arrayMask, size_t nbCellsData)")
            nbCellsData = 3 * nbCells;
            break;
    }

    return this->resize(nbPts, nbCells, nbCellsData, arrayMask);
}

//------------------------------------------------------------------------------

size_t Mesh::resize(size_t nbPts, size_t nbCells, size_t nbCellsData, ExtraArrayType arrayMask)
{
    const size_t size = this->reserve(nbPts, nbCells, nbCellsData, arrayMask);
    m_nbPoints      = nbPts;
    m_nbCells       = nbCells;
    m_cellsDataSize = nbCellsData;
    return size;
}

//------------------------------------------------------------------------------

bool Mesh::adjustAllocatedMemory()
{
    size_t oldAllocatedSize = this->getAllocatedSizeInBytes();

    if(!m_points)
    {
        this->initArrays();
    }

    m_points->resize({size_t(m_nbPoints)});
    m_cellTypes->resize({size_t(m_nbCells)});
    m_cellData->resize({size_t(m_cellsDataSize)});
    m_cellDataOffsets->resize({size_t(m_nbCells)});

    m_pointColors&& (m_pointColors->resize({size_t(m_nbPoints)}));
    m_cellColors&& (m_cellColors->resize({size_t(m_nbCells)}));
    m_pointNormals&& (m_pointNormals->resize({size_t(m_nbPoints)}));
    m_cellNormals&& (m_cellNormals->resize({size_t(m_nbCells)}));
    m_pointTexCoords&& (m_pointTexCoords->resize({size_t(m_nbPoints)}));
    m_cellTexCoords&& (m_cellTexCoords->resize({size_t(m_nbCells)}));

    size_t newAllocatedSize = this->getAllocatedSizeInBytes();
    SLM_ASSERT("Error adjusting memory : allocated size: " << newAllocatedSize
                                                           << " != data size : " << this->getDataSizeInBytes(),
               newAllocatedSize == this->getDataSizeInBytes());
    return oldAllocatedSize != newAllocatedSize;
}

//------------------------------------------------------------------------------

void Mesh::clearPoints()
{
    m_nbPoints = 0;
}

//------------------------------------------------------------------------------

void Mesh::clearCells()
{
    m_nbCells       = 0;
    m_cellsDataSize = 0;
}

//------------------------------------------------------------------------------

void Mesh::clear()
{
    this->clearPointNormals();
    this->clearPointColors();
    this->clearPointTexCoords();
    this->clearCellNormals();
    this->clearCellColors();
    this->clearCellTexCoords();

    m_points->clear();
    m_cellData->clear();
    m_cellDataOffsets->clear();
    m_cellTypes->clear();
    this->initArrays();

    this->clearPoints();
    this->clearCells();
}

//------------------------------------------------------------------------------

void Mesh::clearPointNormals()
{
    m_pointNormals.reset();
}

//------------------------------------------------------------------------------

void Mesh::clearPointColors()
{
    m_pointColors.reset();
}

//------------------------------------------------------------------------------

void Mesh::clearPointTexCoords()
{
    m_pointTexCoords.reset();
}

//------------------------------------------------------------------------------

void Mesh::clearCellNormals()
{
    m_cellNormals.reset();
}

//------------------------------------------------------------------------------

void Mesh::clearCellColors()
{
    m_cellColors.reset();
}

//------------------------------------------------------------------------------

void Mesh::clearCellTexCoords()
{
    m_cellTexCoords.reset();
}

//------------------------------------------------------------------------------

void Mesh::setNumberOfPoints(Mesh::Id nb)
{
    m_nbPoints = nb;
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::getNumberOfPoints() const
{
    return m_nbPoints;
}

//------------------------------------------------------------------------------

void Mesh::setNumberOfCells(Mesh::Id nb)
{
    m_nbCells = nb;
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::getNumberOfCells() const
{
    return m_nbCells;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataSize(Mesh::Id size)
{
    m_cellsDataSize = size;
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::getCellDataSize() const
{
    return m_cellsDataSize;
}

//------------------------------------------------------------------------------

size_t Mesh::getDataSizeInBytes() const
{
    size_t size = 0;

    m_points&& (size += m_points->getElementSizeInBytes() * m_nbPoints);
    m_cellTypes&& (size += m_cellTypes->getElementSizeInBytes() * m_nbCells );
    m_cellData&& (size += m_cellData->getElementSizeInBytes() * m_cellsDataSize);
    m_cellDataOffsets&& (size += m_cellDataOffsets->getElementSizeInBytes() * m_nbCells);
    m_pointColors&& (size += m_pointColors->getElementSizeInBytes() * m_nbPoints);
    m_cellColors&& (size += m_cellColors->getElementSizeInBytes() * m_nbCells);
    m_pointNormals&& (size += m_pointNormals->getElementSizeInBytes() * m_nbPoints);
    m_cellNormals&& (size += m_cellNormals->getElementSizeInBytes() * m_nbCells);
    m_pointTexCoords&& (size += m_pointTexCoords->getElementSizeInBytes() * m_nbPoints);
    m_cellTexCoords&& (size += m_cellTexCoords->getElementSizeInBytes() * m_nbCells);

    return size;
}

//------------------------------------------------------------------------------

size_t Mesh::getAllocatedSizeInBytes() const
{
    size_t size = 0;

    m_points&& (size += m_points->getSizeInBytes());
    m_cellTypes&& (size += m_cellTypes->getSizeInBytes() );
    m_cellData&& (size += m_cellData->getSizeInBytes());
    m_cellDataOffsets&& (size += m_cellDataOffsets->getSizeInBytes());
    m_pointColors&& (size += m_pointColors->getSizeInBytes());
    m_cellColors&& (size += m_cellColors->getSizeInBytes());
    m_pointNormals&& (size += m_pointNormals->getSizeInBytes());
    m_cellNormals&& (size += m_cellNormals->getSizeInBytes());
    m_pointTexCoords&& (size += m_pointTexCoords->getSizeInBytes());
    m_cellTexCoords&& (size += m_cellTexCoords->getSizeInBytes());

    return size;
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushPoint(const PointValueType p[3])
{
    ::fwData::Mesh::Id nbPoints = m_nbPoints;
    size_t allocatedPts = m_points->empty() ? 0 : m_points->getSize().at(0);

    if( allocatedPts <= nbPoints )
    {
        m_points->resizeTMP({allocatedPts + POINT_REALLOC_STEP}, 3);
    }
    this->setPoint(nbPoints, p);

    ++m_nbPoints;
    return nbPoints;
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushPoint(PointValueType x, PointValueType y, PointValueType z)
{
    const ::fwData::Mesh::PointValueType p[3] = {x, y, z};
    return this->pushPoint(p);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushCell(CellValueType idPt)
{
    ::fwData::Mesh::CellValueType point[1] = {idPt};
    return this->pushCell(::fwData::Mesh::POINT, point, 1);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushCell(CellValueType idP1, CellValueType idP2)
{
    const ::fwData::Mesh::CellValueType p[2] = {idP1, idP2};
    return this->pushCell(::fwData::Mesh::EDGE, p, 2);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushCell(CellValueType idP1, CellValueType idP2, CellValueType idP3)
{
    const ::fwData::Mesh::CellValueType p[3] = {idP1, idP2, idP3};
    return this->pushCell(::fwData::Mesh::TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushCell(CellValueType idP1, CellValueType idP2, CellValueType idP3, CellValueType idP4,
                        CellTypesEnum type)
{
    const ::fwData::Mesh::CellValueType p[4] = {idP1, idP2, idP3, idP4};
    return this->pushCell(type, p, 4);
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushCell(CellTypesEnum type,
                        const std::vector<CellValueType> pointIds)
{
    return this->pushCell(type, pointIds.data(), pointIds.size());
}

//------------------------------------------------------------------------------

Mesh::Id Mesh::pushCell(CellTypesEnum type,
                        const CellValueType* pointIds,
                        size_t nbPoints )
{
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'NO_CELL'",
               type != ::fwData::Mesh::NO_CELL || nbPoints == 0);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POINT'",
               type != ::fwData::Mesh::POINT || nbPoints == 1);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'EDGE'",
               type != ::fwData::Mesh::EDGE || nbPoints == 2);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TRIANGLE'",
               type != ::fwData::Mesh::TRIANGLE || nbPoints == 3);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'QUAD'",
               type != ::fwData::Mesh::QUAD || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TETRA'",
               type != ::fwData::Mesh::TETRA || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POLY'",
               type != ::fwData::Mesh::POLY || nbPoints > 4);

    ::fwData::Mesh::Id nbCells = m_nbCells;

    const size_t allocatedCellTypes       = m_cellTypes->empty() ? 0 : m_cellTypes->getSize().at(0);
    const size_t allocatedCellDataOffsets = m_cellDataOffsets->empty() ? 0 : m_cellDataOffsets->getSize().at(0);
    const size_t allocatedCellData        = m_cellData->empty() ? 0 : m_cellData->getSize().at(0);

    if( allocatedCellTypes <= nbCells )
    {
        m_cellTypes->resize({allocatedCellTypes + CELL_REALLOC_STEP});
    }
    if( allocatedCellDataOffsets <= nbCells )
    {
        m_cellDataOffsets->resize({allocatedCellDataOffsets + CELL_REALLOC_STEP});
    }

    if( allocatedCellData <= m_cellsDataSize + nbPoints )
    {
        m_cellData->resize({allocatedCellData + CELLDATA_REALLOC_STEP});
    }

    m_cellTypes->at< CellTypes >(nbCells) = static_cast< CellTypes >(type);
    for (size_t i = 0; i < nbPoints; ++i )
    {
        const CellValueType cellValue = pointIds[i];
        m_cellData->at< CellValueType >(m_cellsDataSize + i) = cellValue;
    }

    m_cellDataOffsets->at<CellDataOffsetType>(nbCells) = m_cellsDataSize;

    m_cellsDataSize += nbPoints;
    ++m_nbCells;
    return nbCells;
}

//------------------------------------------------------------------------------

void Mesh::setPoint(::fwData::Mesh::Id id, const ::fwData::Mesh::PointValueType p[3])
{
    m_points->at<PointValueType>(3*id)   = p[0];
    m_points->at<PointValueType>(3*id+1) = p[1];
    m_points->at<PointValueType>(3*id+2) = p[2];
}

//------------------------------------------------------------------------------

void Mesh::setPoint(::fwData::Mesh::Id id,
                    ::fwData::Mesh::PointValueType x,
                    ::fwData::Mesh::PointValueType y,
                    ::fwData::Mesh::PointValueType z )
{
    const ::fwData::Mesh::PointValueType p[3] = {x, y, z};
    this->setPoint(id, p);
}

//------------------------------------------------------------------------------

void Mesh::setCell(::fwData::Mesh::Id id, CellValueType idPt)
{
    const ::fwData::Mesh::CellValueType p[1] = {idPt};
    this->setCell(id, ::fwData::Mesh::POINT, p, 12);
}

//------------------------------------------------------------------------------

void Mesh::setCell(::fwData::Mesh::Id id, CellValueType idP1, CellValueType idP2)
{
    const ::fwData::Mesh::CellValueType p[2] = {idP1, idP2};
    this->setCell(id, ::fwData::Mesh::EDGE, p, 2);
}

//------------------------------------------------------------------------------

void Mesh::setCell(::fwData::Mesh::Id id, CellValueType idP1, CellValueType idP2, CellValueType idP3)
{
    const ::fwData::Mesh::CellValueType p[3] = {idP1, idP2, idP3};
    this->setCell(id, ::fwData::Mesh::TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

void Mesh::setCell(::fwData::Mesh::Id id, CellValueType idP1, CellValueType idP2, CellValueType idP3,
                   CellValueType idP4,
                   CellTypesEnum type)
{
    const ::fwData::Mesh::CellValueType p[4] = {idP1, idP2, idP3, idP4};
    this->setCell(id, type, p, 4);
}
//------------------------------------------------------------------------------

void Mesh::setCell(::fwData::Mesh::Id id, CellTypesEnum type, const std::vector<CellValueType> pointIds)
{
    this->setCell(id, type, pointIds.data(), pointIds.size());
}
//------------------------------------------------------------------------------

void Mesh::setCell(::fwData::Mesh::Id id, CellTypesEnum type, const CellValueType* pointIds, size_t nbPoints )
{
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'NO_CELL'",
               type != ::fwData::Mesh::NO_CELL || nbPoints == 0);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POINT'",
               type != ::fwData::Mesh::POINT || nbPoints == 1);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'EDGE'",
               type != ::fwData::Mesh::EDGE || nbPoints == 2);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TRIANGLE'",
               type != ::fwData::Mesh::TRIANGLE || nbPoints == 3);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'QUAD'",
               type != ::fwData::Mesh::QUAD || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TETRA'",
               type != ::fwData::Mesh::TETRA || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POLY'",
               type != ::fwData::Mesh::POLY || nbPoints > 4);

    m_cellTypes->at< CellTypes >(id) = static_cast< CellTypes >(type);

    if (id == 0)
    {
        m_cellDataOffsets->at<CellDataOffsetType>(id) = 0;
    }
    CellDataOffsetType currentOffset = m_cellDataOffsets->at<CellDataOffsetType>(id);

    if (id < m_nbCells -1)
    {
        m_cellDataOffsets->at<CellDataOffsetType>(id+1) = currentOffset + nbPoints;
    }

    for (size_t i = 0; i < nbPoints; ++i )
    {
        const CellValueType cellValue = pointIds[i];
        m_cellData->at< CellValueType >(currentOffset + i) = cellValue;
    }
}

//------------------------------------------------------------------------------

void Mesh::setPointColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4])
{
    m_pointColors->at<ColorValueType>(4*id)   = c[0];
    m_pointColors->at<ColorValueType>(4*id+1) = c[1];
    m_pointColors->at<ColorValueType>(4*id+2) = c[2];
    m_pointColors->at<ColorValueType>(4*id+3) = c[3];
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4])
{
    m_cellColors->at<ColorValueType>(4*id)   = c[0];
    m_cellColors->at<ColorValueType>(4*id+1) = c[1];
    m_cellColors->at<ColorValueType>(4*id+2) = c[2];
    m_cellColors->at<ColorValueType>(4*id+3) = c[3];
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3])
{
    m_pointNormals->at<NormalValueType>(3*id)   = n[0];
    m_pointNormals->at<NormalValueType>(3*id+1) = n[1];
    m_pointNormals->at<NormalValueType>(3*id+2) = n[2];
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3])
{
    m_cellNormals->at<NormalValueType>(3*id)   = n[0];
    m_cellNormals->at<NormalValueType>(3*id+1) = n[1];
    m_cellNormals->at<NormalValueType>(3*id+2) = n[2];
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoord(::fwData::Mesh::Id id, const ::fwData::Mesh::TexCoordValueType t[2])
{
    m_pointTexCoords->at<TexCoordValueType>(2*id)   = t[0];
    m_pointTexCoords->at<TexCoordValueType>(2*id+1) = t[1];
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoord(::fwData::Mesh::Id id, const ::fwData::Mesh::TexCoordValueType t[2])
{
    m_cellTexCoords->at<TexCoordValueType>(2*id)   = t[0];
    m_cellTexCoords->at<TexCoordValueType>(2*id+1) = t[1];
}

//------------------------------------------------------------------------------

Mesh::LocksType Mesh::lock() const
{
    LocksType locks;
    locks.push_back(m_points->lock());
    locks.push_back(m_cellTypes->lock());
    locks.push_back(m_cellData->lock());
    locks.push_back(m_cellDataOffsets->lock());

    if (m_pointColors)
    {
        locks.push_back(m_pointColors->lock());
    }
    if (m_cellColors)
    {
        locks.push_back(m_cellColors->lock());
    }
    if (m_pointNormals)
    {
        locks.push_back(m_pointNormals->lock());
    }
    if (m_cellNormals)
    {
        locks.push_back(m_cellNormals->lock());
    }
    if (m_cellTexCoords)
    {
        locks.push_back(m_cellTexCoords->lock());
    }
    if (m_pointTexCoords)
    {
        locks.push_back(m_pointTexCoords->lock());
    }

    return locks;
}

//------------------------------------------------------------------------------
// Deprecated API
//------------------------------------------------------------------------------

size_t Mesh::allocate(size_t nbPts, size_t nbCells, size_t nbCellsData)
{
    if (nbCellsData == 0)
    {
        nbCellsData = 3 * nbCells;
    }

    size_t allocatedSize = 0;

    allocatedSize += m_points->resize( {nbPts}, 3, true);
    allocatedSize += m_cellTypes->resize( {nbCells}, 1, true);
    allocatedSize += m_cellData->resize( {nbCellsData}, 1, true);
    allocatedSize += m_cellDataOffsets->resize( {nbCells}, 1, true );

    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointNormals()
{
    size_t allocatedSize = 0;
    if (!m_pointNormals )
    {
        m_pointNormals = ::fwData::Array::New();
    }
    allocatedSize += m_pointNormals->resize( ::fwTools::Type::create<NormalValueType>(), {size_t(m_nbPoints)}, 3, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointColors(ColorArrayTypes t)
{
    OSLM_ASSERT("Bad ColorArrayTypes : " << t, t == RGB || t == RGBA);
    size_t allocatedSize = 0;
    if (!m_pointColors )
    {
        m_pointColors = ::fwData::Array::New();
    }
    allocatedSize += m_pointColors->resize( ::fwTools::Type::create<ColorValueType>(), {size_t(m_nbPoints)}, t, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointTexCoords()
{
    size_t allocatedSize = 0;
    if (!m_pointTexCoords )
    {
        m_pointTexCoords = ::fwData::Array::New();
    }
    allocatedSize += m_pointTexCoords->resize( ::fwTools::Type::create<TexCoordValueType>(), {size_t(
                                                                                                  m_nbPoints)}, 2,
                                               true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellNormals()
{
    size_t allocatedSize = 0;
    if (!m_cellNormals )
    {
        m_cellNormals = ::fwData::Array::New();
    }
    allocatedSize += m_cellNormals->resize( ::fwTools::Type::create<NormalValueType>(), {size_t(m_nbCells)}, 3, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellColors(ColorArrayTypes t)
{
    OSLM_ASSERT("Bad ColorArrayTypes : " << t, t == RGB || t == RGBA);
    size_t allocatedSize = 0;
    if (!m_cellColors )
    {
        m_cellColors = ::fwData::Array::New();
    }
    allocatedSize += m_cellColors->resize( ::fwTools::Type::create<ColorValueType>(), {size_t(m_nbCells)}, t, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellTexCoords()
{
    size_t allocatedSize = 0;
    if (!m_cellTexCoords )
    {
        m_cellTexCoords = ::fwData::Array::New();
    }
    allocatedSize +=
        m_cellTexCoords->resize( ::fwTools::Type::create<TexCoordValueType>(), {size_t(m_nbCells)}, 2, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

void Mesh::setPointsArray(const ::fwData::Array::sptr& array)
{
    m_points = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellTypesArray(const ::fwData::Array::sptr& array)
{
    m_cellTypes = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataArray (const ::fwData::Array::sptr& array)
{
    m_cellData = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataOffsetsArray(const ::fwData::Array::sptr& array)
{
    m_cellDataOffsets = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointColorsArray(const ::fwData::Array::sptr& array)
{
    m_pointColors = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellColorsArray(const ::fwData::Array::sptr& array)
{
    m_cellColors = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointNormalsArray(const ::fwData::Array::sptr& array)
{
    m_pointNormals = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellNormalsArray(const ::fwData::Array::sptr& array)
{
    m_cellNormals = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoordsArray(const ::fwData::Array::sptr& array)
{
    m_pointTexCoords = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoordsArray(const ::fwData::Array::sptr& array)
{
    m_cellTexCoords = array;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getPointsArray() const
{
    return m_points;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellTypesArray() const
{
    return m_cellTypes;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellDataArray() const
{
    return m_cellData;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellDataOffsetsArray() const
{
    return m_cellDataOffsets;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getPointColorsArray() const
{
    return m_pointColors;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellColorsArray() const
{
    return m_cellColors;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getPointNormalsArray() const
{
    return m_pointNormals;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellNormalsArray() const
{
    return m_cellNormals;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getPointTexCoordsArray() const
{
    return m_pointTexCoords;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellTexCoordsArray() const
{
    return m_cellTexCoords;
}

//------------------------------------------------------------------------------

void Mesh::addDataArray(const std::string& name, ::fwData::Array::sptr array)
{
    m_arrayMap[name] = array;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getDataArray(const std::string& name) const
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

std::vector<std::string> Mesh::getDataArrayNames() const
{
    std::vector<std::string> vectNames;
    std::transform( m_arrayMap.begin(), m_arrayMap.end(),
                    std::back_inserter(vectNames),
                    std::bind(&ArrayMapType::value_type::first, std::placeholders::_1) );
    return vectNames;
}

//------------------------------------------------------------------------------

void Mesh::removeDataArray(const std::string& name)
{
    m_arrayMap.erase(name);
}

//------------------------------------------------------------------------------

} //namespace fwData
