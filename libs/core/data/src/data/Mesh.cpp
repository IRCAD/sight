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

#include "data/Mesh.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>

#include <cstdlib>
#include <functional>
#include <numeric>

namespace sight::data
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000

fwDataRegisterMacro( ::sight::data::Mesh );

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType Mesh::s_VERTEX_MODIFIED_SIG           = "vertexModified";
const core::com::Signals::SignalKeyType Mesh::s_POINT_COLORS_MODIFIED_SIG     = "pointColorsModified";
const core::com::Signals::SignalKeyType Mesh::s_CELL_COLORS_MODIFIED_SIG      = "cellColorsModified";
const core::com::Signals::SignalKeyType Mesh::s_POINT_NORMALS_MODIFIED_SIG    = "pointNormalsModified";
const core::com::Signals::SignalKeyType Mesh::s_CELL_NORMALS_MODIFIED_SIG     = "cellNormalsModified";
const core::com::Signals::SignalKeyType Mesh::s_POINT_TEX_COORDS_MODIFIED_SIG = "pointTexCoordsModified";
const core::com::Signals::SignalKeyType Mesh::s_CELL_TEX_COORDS_MODIFIED_SIG  = "cellTexCoordsModified";

//------------------------------------------------------------------------------

Mesh::Mesh(data::Object::Key) :
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

    m_points          = data::Array::New();
    m_cellTypes       = data::Array::New();
    m_cellData        = data::Array::New();
    m_cellDataOffsets = data::Array::New();
    m_pointColors     = data::Array::New();
    m_cellColors      = data::Array::New();
    m_pointNormals    = data::Array::New();
    m_cellNormals     = data::Array::New();
    m_pointTexCoords  = data::Array::New();
    m_cellTexCoords   = data::Array::New();

    // TODO sight 22.0: Add a second dimension on the array to replace the deprecated component
    m_points->setType(core::tools::Type::create<PointValueType>());
    m_cellTypes->setType(core::tools::Type::create<CellTypes>());
    m_cellData->setType(core::tools::Type::create<CellId>());
    m_cellDataOffsets->setType(core::tools::Type::create<CellId>());
}

//------------------------------------------------------------------------------

Mesh::~Mesh()
{
}

//------------------------------------------------------------------------------

void Mesh::shallowCopy(const Object::csptr& _source )
{
    Mesh::csptr other = Mesh::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
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

    m_attributes = other->m_attributes;

    m_arrayMap = other->m_arrayMap;
}

//------------------------------------------------------------------------------

void Mesh::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Mesh::csptr other = Mesh::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !other );
    this->fieldDeepCopy( _source, cache );

    m_nbPoints      = other->m_nbPoints;
    m_nbCells       = other->m_nbCells;
    m_cellsDataSize = other->m_cellsDataSize;

    m_attributes = other->m_attributes;

    m_points          = data::Object::copy( other->m_points, cache );
    m_cellTypes       = data::Object::copy( other->m_cellTypes, cache );
    m_cellData        = data::Object::copy( other->m_cellData, cache );
    m_cellDataOffsets = data::Object::copy( other->m_cellDataOffsets, cache );

    //Object::copy returns a null object if source object is null
    m_pointColors    = data::Object::copy( other->m_pointColors, cache );
    m_cellColors     = data::Object::copy( other->m_cellColors, cache );
    m_pointNormals   = data::Object::copy( other->m_pointNormals, cache );
    m_cellNormals    = data::Object::copy( other->m_cellNormals, cache );
    m_cellTexCoords  = data::Object::copy( other->m_cellTexCoords, cache );
    m_pointTexCoords = data::Object::copy( other->m_pointTexCoords, cache );

    m_arrayMap.clear();
    for(const ArrayMapType::value_type& element : other->m_arrayMap)
    {
        m_arrayMap[element.first] = data::Object::copy(element.second, cache);
    }

}

//------------------------------------------------------------------------------

size_t Mesh::reserve(Size nbPts, Size nbCells, CellType cellType, Attributes arrayMask)
{
    Size nbCellsData = 0;
    switch (cellType)
    {
        case CellType::POINT:
            nbCellsData = nbCells;
            break;
        case CellType::EDGE:
            nbCellsData = 2 * nbCells;
            break;
        case CellType::TRIANGLE:
            nbCellsData = 3 * nbCells;
            break;
        case CellType::QUAD:
            nbCellsData = 4 * nbCells;
            break;
        case CellType::TETRA:
            nbCellsData = 4 * nbCells;
            break;
        default:
            SLM_ERROR("Cannot determine the cell data size to allocate for this type of cell, please use "
                      "resize(size_t nbPts, size_t nbCells, Attribute arrayMask, size_t nbCellsData)")
            nbCellsData = 3 * nbCells;
            break;
    }

    return this->reserve(nbPts, nbCells, nbCellsData, arrayMask);
}

//------------------------------------------------------------------------------

size_t Mesh::reserve(Size nbPts, Size nbCells, Size nbCellsData, Attributes arrayMask)
{
    FW_RAISE_EXCEPTION_IF(data::Exception("Cannot not allocate empty size"), nbPts == 0 ||
                          nbCells == 0 || nbCellsData == 0);

    m_points->resizeTMP( {nbPts}, 3 );

    m_attributes = m_attributes | arrayMask;

    // TODO sight 22.0: Add a second dimension on the array to replace the deprecated component

    // Test attributes mask, if present resize corresponding array, if not check if array needs to be cleared.
    if (static_cast<int>(arrayMask & Attributes::POINT_COLORS))
    {
        m_pointColors->resizeTMP(core::tools::Type::s_UINT8, {nbPts}, 4);
    }
    if (static_cast<int>(arrayMask & Attributes::POINT_NORMALS))
    {
        m_pointNormals->resizeTMP(core::tools::Type::s_FLOAT, {nbPts}, 3 );
    }
    if (static_cast<int>(arrayMask & Attributes::POINT_TEX_COORDS))
    {
        m_pointTexCoords->resizeTMP( core::tools::Type::s_FLOAT, {nbPts}, 2 );
    }

    m_cellTypes->resize({nbCells});
    m_cellDataOffsets->resize({nbCells});
    m_cellData->resize({nbCellsData});

    if (static_cast<int>(arrayMask & Attributes::CELL_COLORS))
    {
        m_cellColors->resizeTMP( core::tools::Type::s_UINT8,  {nbCells}, 4 );
    }

    if (static_cast<int>(arrayMask & Attributes::CELL_NORMALS))
    {
        m_cellNormals->resizeTMP( core::tools::Type::s_FLOAT, {nbCells}, 3 );
    }

    if (static_cast<int>(arrayMask & Attributes::CELL_TEX_COORDS))
    {
        m_cellTexCoords->resizeTMP( core::tools::Type::s_FLOAT, {nbCells}, 2 );
    }

    return this->getAllocatedSizeInBytes();
}

//------------------------------------------------------------------------------
size_t Mesh::resize(Size nbPts, Size nbCells, CellType cellType, Attributes arrayMask)
{
    Size nbCellsData = 0;
    switch (cellType)
    {
        case CellType::POINT:
            nbCellsData = nbCells;
            break;
        case CellType::EDGE:
            nbCellsData = 2 * nbCells;
            break;
        case CellType::TRIANGLE:
            nbCellsData = 3 * nbCells;
            break;
        case CellType::QUAD:
            nbCellsData = 4 * nbCells;
            break;
        case CellType::TETRA:
            nbCellsData = 4 * nbCells;
            break;
        default:
            SLM_ERROR("Cannot determine the cell data size to allocate for this type of cell, please use "
                      "resize(size_t nbPts, size_t nbCells, Attribute arrayMask, size_t nbCellsData)")
            nbCellsData = 3 * nbCells;
            break;
    }

    return this->resize(nbPts, nbCells, nbCellsData, arrayMask);
}

//------------------------------------------------------------------------------

size_t Mesh::resize(Size nbPts, Size nbCells, Size nbCellsData, Attributes arrayMask)
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

    m_points->resizeTMP({size_t(m_nbPoints)}, 3);
    m_cellTypes->resize({size_t(m_nbCells)});
    m_cellData->resize({size_t(m_cellsDataSize)});
    m_cellDataOffsets->resize({size_t(m_nbCells)});

    if(static_cast<std::uint8_t>(m_attributes & Attributes::POINT_COLORS))
    {
        m_pointColors->resize( { size_t( m_nbPoints ) } );
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::CELL_COLORS))
    {
        m_cellColors->resize( { size_t( m_nbCells ) } );
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::POINT_NORMALS))
    {
        m_pointNormals->resize( { size_t( m_nbPoints ) } );
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::CELL_NORMALS))
    {
        m_cellNormals->resize( { size_t( m_nbCells ) } );
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::POINT_TEX_COORDS))
    {
        m_pointTexCoords->resize( { size_t( m_nbPoints ) } );
    }

    if(static_cast<std::uint8_t>(m_attributes & Attributes::CELL_TEX_COORDS))
    {
        m_cellTexCoords->resize( { size_t( m_nbCells ) } );
    }

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
    // Clear all arrays.
    m_points->clear();
    m_cellData->clear();
    m_cellDataOffsets->clear();
    m_cellTypes->clear();
    m_pointColors->clear();
    m_pointNormals->clear();
    m_pointTexCoords->clear();
    m_cellColors->clear();
    m_cellNormals->clear();
    m_cellTexCoords->clear();

    // Reset nbPoints, nbCells & cellsDataSize.
    m_nbPoints      = 0;
    m_nbCells       = 0;
    m_cellsDataSize = 0;

    // Reset type of points, cellTypes, cellData & cellDataOffsets.
    m_points->setType(core::tools::Type::create<PointValueType>());
    m_cellTypes->setType(core::tools::Type::create<CellTypes>());
    m_cellData->setType(core::tools::Type::create<CellId>());
    m_cellDataOffsets->setType(core::tools::Type::create<CellId>());

    m_attributes = Attributes::NONE;
}

//------------------------------------------------------------------------------

void Mesh::clearPointNormals()
{
    m_attributes = m_attributes & ~Attributes::POINT_NORMALS;
    m_pointNormals->clear();
}

//------------------------------------------------------------------------------

void Mesh::clearPointColors()
{
    m_attributes = m_attributes & ~Attributes::POINT_COLORS;
    m_pointColors->clear();
}

//------------------------------------------------------------------------------

void Mesh::clearPointTexCoords()
{
    m_attributes = m_attributes & ~Attributes::POINT_TEX_COORDS;
    m_pointTexCoords->clear();
}

//------------------------------------------------------------------------------

void Mesh::clearCellNormals()
{
    m_attributes = m_attributes & ~Attributes::CELL_NORMALS;
    m_cellNormals->clear();
}

//------------------------------------------------------------------------------

void Mesh::clearCellColors()
{
    m_attributes = m_attributes & ~Attributes::CELL_COLORS;
    m_cellColors->clear();
}

//------------------------------------------------------------------------------

void Mesh::clearCellTexCoords()
{
    m_attributes = m_attributes & ~Attributes::CELL_TEX_COORDS;
    m_cellTexCoords->clear();
}

//------------------------------------------------------------------------------

void Mesh::setNumberOfPoints(Size nb)
{
    m_nbPoints = nb;
}

//------------------------------------------------------------------------------

Mesh::Size Mesh::getNumberOfPoints() const
{
    return m_nbPoints;
}

//------------------------------------------------------------------------------

void Mesh::setNumberOfCells(Size nb)
{
    m_nbCells = nb;
}

//------------------------------------------------------------------------------

Mesh::Size Mesh::getNumberOfCells() const
{
    return m_nbCells;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataSize(Size size)
{
    m_cellsDataSize = size;
}

//------------------------------------------------------------------------------

Mesh::Size Mesh::getCellDataSize() const
{
    return m_cellsDataSize;
}

//------------------------------------------------------------------------------

size_t Mesh::getDataSizeInBytes() const
{
    size_t size = 0;

    if(!m_points->empty())
    {
        size += m_points->getElementSizeInBytes() * m_nbPoints;
    }
    if(!m_cellTypes->empty())
    {
        size += m_cellTypes->getElementSizeInBytes() * m_nbCells;
    }
    if(!m_cellData->empty())
    {
        size += m_cellData->getElementSizeInBytes() * m_cellsDataSize;
    }
    if(!m_cellDataOffsets->empty())
    {
        size += m_cellDataOffsets->getElementSizeInBytes() * m_nbCells;
    }
    if(static_cast<int>(m_attributes & Attributes::POINT_COLORS))
    {
        size += m_pointColors->getElementSizeInBytes() * m_nbPoints;
    }

    if(static_cast<int>(m_attributes & Attributes::CELL_COLORS))
    {
        size += m_cellColors->getElementSizeInBytes() * m_nbCells;
    }

    if(static_cast<int>(m_attributes & Attributes::POINT_NORMALS))
    {
        size += m_pointNormals->getElementSizeInBytes() * m_nbPoints;
    }

    if(static_cast<int>(m_attributes & Attributes::CELL_NORMALS))
    {
        size += m_cellNormals->getElementSizeInBytes() * m_nbCells;
    }

    if(static_cast<int>(m_attributes & Attributes::POINT_TEX_COORDS))
    {
        size += m_pointTexCoords->getElementSizeInBytes() * m_nbPoints;
    }

    if(static_cast<int>(m_attributes & Attributes::CELL_TEX_COORDS))
    {
        size += m_cellTexCoords->getElementSizeInBytes() * m_nbCells;
    }

    return size;
}

//------------------------------------------------------------------------------

size_t Mesh::getAllocatedSizeInBytes() const
{
    size_t size = 0;

    if(!m_points->empty())
    {
        size += m_points->getSizeInBytes();
    }
    if(!m_cellTypes->empty())
    {
        size += m_cellTypes->getSizeInBytes();
    }
    if(!m_cellData->empty())
    {
        size += m_cellData->getSizeInBytes();
    }
    if(!m_cellDataOffsets->empty())
    {
        size += m_cellDataOffsets->getSizeInBytes();
    }
    if(static_cast<int>(m_attributes & Attributes::POINT_COLORS))
    {
        size += m_pointColors->getSizeInBytes();
    }

    if(static_cast<int>(m_attributes & Attributes::CELL_COLORS))
    {
        size += m_cellColors->getSizeInBytes();
    }

    if(static_cast<int>(m_attributes & Attributes::POINT_NORMALS))
    {
        size += m_pointNormals->getSizeInBytes();
    }

    if(static_cast<int>(m_attributes & Attributes::CELL_NORMALS))
    {
        size += m_cellNormals->getSizeInBytes();
    }

    if(static_cast<int>(m_attributes & Attributes::POINT_TEX_COORDS))
    {
        size += m_pointTexCoords->getSizeInBytes();
    }

    if(static_cast<int>(m_attributes & Attributes::CELL_TEX_COORDS))
    {
        size += m_cellTexCoords->getSizeInBytes();
    }

    return size;
}

//------------------------------------------------------------------------------

Mesh::PointId Mesh::pushPoint(const PointValueType p[3])
{
    Size nbPoints       = m_nbPoints;
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

Mesh::PointId Mesh::pushPoint(PointValueType x, PointValueType y, PointValueType z)
{
    const data::Mesh::PointValueType p[3] = {x, y, z};
    return this->pushPoint(p);
}

//------------------------------------------------------------------------------

Mesh::CellId Mesh::pushCell(PointId idPt)
{
    PointId point[1] = { idPt };
    return this->pushCell(data::Mesh::CellType::POINT, point, 1);
}

//------------------------------------------------------------------------------

Mesh::CellId Mesh::pushCell(PointId idP1, PointId idP2)
{
    const PointId p[2] = {idP1, idP2};
    return this->pushCell(data::Mesh::CellType::EDGE, p, 2);
}

//------------------------------------------------------------------------------

Mesh::CellId Mesh::pushCell(PointId idP1, PointId idP2, PointId idP3)
{
    const PointId p[3] = {idP1, idP2, idP3};
    return this->pushCell(data::Mesh::CellType::TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

Mesh::CellId Mesh::pushCell(PointId idP1, PointId idP2, PointId idP3, PointId idP4,
                            CellType type)
{
    const PointId p[4] = {idP1, idP2, idP3, idP4};
    return this->pushCell(type, p, 4);
}

//------------------------------------------------------------------------------

Mesh::CellId Mesh::pushCell(CellType type,
                            const std::vector<PointId> pointIds)
{
    return this->pushCell(type, pointIds.data(), static_cast< PointId >(pointIds.size()));
}

//------------------------------------------------------------------------------

Mesh::CellId Mesh::pushCell(CellType type,
                            const PointId* pointIds,
                            Size nbPoints )
{
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'NO_CELL'",
               type != data::Mesh::CellType::NO_CELL || nbPoints == 0);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POINT'",
               type != data::Mesh::CellType::POINT || nbPoints == 1);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'EDGE'",
               type != data::Mesh::CellType::EDGE || nbPoints == 2);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TRIANGLE'",
               type != data::Mesh::CellType::TRIANGLE || nbPoints == 3);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'QUAD'",
               type != data::Mesh::CellType::QUAD || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TETRA'",
               type != data::Mesh::CellType::TETRA || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POLY'",
               type != data::Mesh::CellType::POLY || nbPoints > 4);

    Size nbCells = m_nbCells;

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

    if( allocatedCellData < m_cellsDataSize + nbPoints )
    {
        m_cellData->resize({allocatedCellData + CELLDATA_REALLOC_STEP});
    }

    m_cellTypes->at< CellTypes >(nbCells) = static_cast< CellTypes >(type);
    for (size_t i = 0; i < nbPoints; ++i )
    {
        const PointId cellValue = pointIds[i];
        m_cellData->at< CellId >(m_cellsDataSize + i) = cellValue;
    }

    m_cellDataOffsets->at<CellId>(nbCells) = m_cellsDataSize;

    m_cellsDataSize += nbPoints;
    ++m_nbCells;
    return nbCells;
}

//------------------------------------------------------------------------------

void Mesh::setPoint(PointId id, const data::Mesh::PointValueType p[3])
{
    m_points->at<PointValueType>(3*id)   = p[0];
    m_points->at<PointValueType>(3*id+1) = p[1];
    m_points->at<PointValueType>(3*id+2) = p[2];
}

//------------------------------------------------------------------------------

void Mesh::setPoint(PointId id,
                    data::Mesh::PointValueType x,
                    data::Mesh::PointValueType y,
                    data::Mesh::PointValueType z )
{
    const data::Mesh::PointValueType p[3] = {x, y, z};
    this->setPoint(id, p);
}

//------------------------------------------------------------------------------

void Mesh::setCell(CellId id, PointId idPt)
{
    const PointId p[1] = {idPt};
    this->setCell(id, data::Mesh::CellType::POINT, p, 12);
}

//------------------------------------------------------------------------------

void Mesh::setCell(CellId id, PointId idP1, PointId idP2)
{
    const PointId p[2] = {idP1, idP2};
    this->setCell(id, data::Mesh::CellType::EDGE, p, 2);
}

//------------------------------------------------------------------------------

void Mesh::setCell(CellId id, PointId idP1, PointId idP2, PointId idP3)
{
    const PointId p[3] = {idP1, idP2, idP3};
    this->setCell(id, data::Mesh::CellType::TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

void Mesh::setCell(CellId id, PointId idP1, PointId idP2, PointId idP3,
                   PointId idP4,
                   CellType type)
{
    const PointId p[4] = {idP1, idP2, idP3, idP4};
    this->setCell(id, type, p, 4);
}
//------------------------------------------------------------------------------

void Mesh::setCell(CellId id, CellType type, const std::vector<PointId>& pointIds)
{
    this->setCell(id, type, pointIds.data(), static_cast< Size >(pointIds.size()));
}
//------------------------------------------------------------------------------

void Mesh::setCell(CellId id, CellType type, const PointId* pointIds, Size nbPoints )
{
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'NO_CELL'",
               type != data::Mesh::CellType::NO_CELL || nbPoints == 0);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POINT'",
               type != data::Mesh::CellType::POINT || nbPoints == 1);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'EDGE'",
               type != data::Mesh::CellType::EDGE || nbPoints == 2);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TRIANGLE'",
               type != data::Mesh::CellType::TRIANGLE || nbPoints == 3);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'QUAD'",
               type != data::Mesh::CellType::QUAD || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'TETRA'",
               type != data::Mesh::CellType::TETRA || nbPoints == 4);
    SLM_ASSERT("Bad number of points ("<< nbPoints << ") for cell type: 'POLY'",
               type != data::Mesh::CellType::POLY || nbPoints > 4);

    m_cellTypes->at< CellTypes >(id) = static_cast< CellTypes >(type);

    if (id == 0)
    {
        m_cellDataOffsets->at<CellId>(id) = 0;
    }
    CellId currentOffset = m_cellDataOffsets->at<CellId>(id);

    if (id < m_nbCells -1)
    {
        m_cellDataOffsets->at<CellId>(id+1) = currentOffset + nbPoints;
    }

    for (size_t i = 0; i < nbPoints; ++i )
    {
        const CellId cellValue = pointIds[i];
        m_cellData->at< CellId >(currentOffset + i) = cellValue;
    }
}

//------------------------------------------------------------------------------

void Mesh::setPointColor(PointId id, const std::array< data::Mesh::ColorValueType, 4>& c)
{
    this->setPointColor(id, c[0], c[1], c[2], c[3]);
}

//------------------------------------------------------------------------------

void Mesh::setPointColor(PointId id, data::Mesh::ColorValueType r, data::Mesh::ColorValueType g,
                         data::Mesh::ColorValueType b,
                         data::Mesh::ColorValueType a)
{
    m_pointColors->at<ColorValueType>(4*id)   = r;
    m_pointColors->at<ColorValueType>(4*id+1) = g;
    m_pointColors->at<ColorValueType>(4*id+2) = b;
    m_pointColors->at<ColorValueType>(4*id+3) = a;
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(CellId id, const std::array< data::Mesh::ColorValueType, 4>& c)
{
    this->setCellColor(id, c[0], c[1], c[2], c[3]);
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(CellId id, data::Mesh::ColorValueType r, data::Mesh::ColorValueType g,
                        data::Mesh::ColorValueType b,
                        data::Mesh::ColorValueType a)
{
    m_cellColors->at<ColorValueType>(4*id)   = r;
    m_cellColors->at<ColorValueType>(4*id+1) = g;
    m_cellColors->at<ColorValueType>(4*id+2) = b;
    m_cellColors->at<ColorValueType>(4*id+3) = a;
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(PointId id, const std::array< data::Mesh::NormalValueType, 3>& n)
{
    this->setPointNormal(id, n[0], n[1], n[2]);
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(PointId id, data::Mesh::NormalValueType nx, data::Mesh::NormalValueType ny,
                          data::Mesh::NormalValueType nz)
{
    m_pointNormals->at<NormalValueType>(3*id)   = nx;
    m_pointNormals->at<NormalValueType>(3*id+1) = ny;
    m_pointNormals->at<NormalValueType>(3*id+2) = nz;
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(CellId id, const std::array< data::Mesh::NormalValueType, 3>& n)
{
    this->setCellNormal(id, n[0], n[1], n[2]);
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(CellId id, data::Mesh::NormalValueType nx, data::Mesh::NormalValueType ny,
                         data::Mesh::NormalValueType nz)
{
    m_cellNormals->at<NormalValueType>(3*id)   = nx;
    m_cellNormals->at<NormalValueType>(3*id+1) = ny;
    m_cellNormals->at<NormalValueType>(3*id+2) = nz;
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoord(PointId id, const std::array< data::Mesh::TexCoordValueType, 2>& t)
{
    this->setPointTexCoord(id, t[0], t[1]);
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoord(PointId id, data::Mesh::TexCoordValueType u,
                            data::Mesh::TexCoordValueType v)
{
    m_pointTexCoords->at<TexCoordValueType>(2*id)   = u;
    m_pointTexCoords->at<TexCoordValueType>(2*id+1) = v;
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoord(CellId id, const std::array< data::Mesh::TexCoordValueType, 2>& t)
{
    this->setCellTexCoord(id, t[0], t[1]);
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoord(CellId id, data::Mesh::TexCoordValueType u,
                           data::Mesh::TexCoordValueType v)
{
    m_cellTexCoords->at<TexCoordValueType>(2*id)   = u;
    m_cellTexCoords->at<TexCoordValueType>(2*id+1) = v;
}

//------------------------------------------------------------------------------

Mesh::LocksType Mesh::lock() const
{
    LocksType locks;
    locks.push_back(m_points->lock());
    locks.push_back(m_cellTypes->lock());
    locks.push_back(m_cellData->lock());
    locks.push_back(m_cellDataOffsets->lock());
    locks.push_back(m_pointColors->lock());
    locks.push_back(m_cellColors->lock());
    locks.push_back(m_pointNormals->lock());
    locks.push_back(m_cellNormals->lock());
    locks.push_back(m_cellTexCoords->lock());
    locks.push_back(m_pointTexCoords->lock());

    return locks;
}

//------------------------------------------------------------------------------

const Mesh::ColorValueType* Mesh::getPointColorsBuffer() const
{
    return static_cast<ColorValueType*>(m_pointColors->getBuffer());
}

//------------------------------------------------------------------------------

const Mesh::ColorValueType* Mesh::getCellColorsBuffer() const
{
    return static_cast<ColorValueType*>(m_cellColors->getBuffer());
}
//------------------------------------------------------------------------------

void Mesh::lockBuffer(std::vector< core::memory::BufferObject::Lock >& locks) const
{
    const LocksType& myLocks = this->lock();
    locks.insert(std::end(locks), std::begin(myLocks), std::end(myLocks));
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
    m_attributes = m_attributes | Attributes::POINT_NORMALS;
    return m_pointNormals->resize( core::tools::Type::create<NormalValueType>(), {size_t(m_nbPoints)}, 3, true);
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointColors(ColorArrayTypes t)
{
    SLM_ASSERT("Bad ColorArrayTypes : " << t, t == RGB || t == RGBA);

    m_attributes = m_attributes | Attributes::POINT_COLORS;

    return m_pointColors->resize( core::tools::Type::create<ColorValueType>(), {size_t(m_nbPoints)}, t, true);
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointTexCoords()
{
    m_attributes = m_attributes | Attributes::POINT_TEX_COORDS;

    return m_pointTexCoords->resize( core::tools::Type::create<TexCoordValueType>(), {size_t(
                                                                                          m_nbPoints)}, 2,
                                     true);
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellNormals()
{
    m_attributes = m_attributes | Attributes::CELL_NORMALS;
    return m_cellNormals->resize( core::tools::Type::create<NormalValueType>(), {size_t(m_nbCells)}, 3, true);
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellColors(ColorArrayTypes t)
{
    OSLM_ASSERT("Bad ColorArrayTypes : " << t, t == RGB || t == RGBA);
    size_t allocatedSize = 0;
    if (!m_cellColors )
    {
        m_cellColors = data::Array::New();
    }
    allocatedSize += m_cellColors->resize( core::tools::Type::create<ColorValueType>(), {size_t(m_nbCells)}, t, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellTexCoords()
{
    m_attributes = m_attributes | Attributes::CELL_TEX_COORDS;
    return m_cellTexCoords->resize( core::tools::Type::create<TexCoordValueType>(), {size_t(m_nbCells)}, 2, true);
}

//------------------------------------------------------------------------------

void Mesh::setPointsArray(const data::Array::sptr& array)
{
    m_points = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellTypesArray(const data::Array::sptr& array)
{
    m_cellTypes = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataArray (const data::Array::sptr& array)
{
    m_cellData = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataOffsetsArray(const data::Array::sptr& array)
{
    m_cellDataOffsets = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointColorsArray(const data::Array::sptr& array)
{
    m_attributes  = m_attributes | Attributes::POINT_COLORS;
    m_pointColors = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellColorsArray(const data::Array::sptr& array)
{
    m_attributes = m_attributes | Attributes::CELL_COLORS;
    m_cellColors = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointNormalsArray(const data::Array::sptr& array)
{
    m_attributes   = m_attributes | Attributes::POINT_NORMALS;
    m_pointNormals = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellNormalsArray(const data::Array::sptr& array)
{
    m_attributes  = m_attributes | Attributes::CELL_NORMALS;
    m_cellNormals = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoordsArray(const data::Array::sptr& array)
{
    m_attributes     = m_attributes | Attributes::POINT_TEX_COORDS;
    m_pointTexCoords = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoordsArray(const data::Array::sptr& array)
{
    m_attributes    = m_attributes | Attributes::CELL_TEX_COORDS;
    m_cellTexCoords = array;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getPointsArray() const
{
    return m_points;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getCellTypesArray() const
{
    return m_cellTypes;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getCellDataArray() const
{
    return m_cellData;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getCellDataOffsetsArray() const
{
    return m_cellDataOffsets;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getPointColorsArray() const
{
    const bool hasData = static_cast<std::uint8_t>(m_attributes & Attributes::POINT_COLORS) != 0;

    return hasData ? m_pointColors : nullptr;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getCellColorsArray() const
{
    const bool hasData = static_cast<std::uint8_t>(m_attributes & Attributes::CELL_COLORS) != 0;

    return hasData ? m_cellColors : nullptr;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getPointNormalsArray() const
{
    const bool hasData = static_cast<std::uint8_t>(m_attributes & Attributes::POINT_NORMALS) != 0;

    return hasData ? m_pointNormals : nullptr;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getCellNormalsArray() const
{
    const bool hasData = static_cast<std::uint8_t>(m_attributes & Attributes::CELL_NORMALS) != 0;

    return hasData ? m_cellNormals : nullptr;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getPointTexCoordsArray() const
{
    const bool hasData = static_cast<std::uint8_t>(m_attributes & Attributes::POINT_TEX_COORDS) != 0;

    return hasData ? m_pointTexCoords : nullptr;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getCellTexCoordsArray() const
{
    const bool hasData = static_cast<std::uint8_t>(m_attributes & Attributes::CELL_TEX_COORDS) != 0;

    return hasData ? m_cellTexCoords : nullptr;
}

//------------------------------------------------------------------------------

void Mesh::addDataArray(const std::string& name, data::Array::sptr array)
{
    m_arrayMap[name] = array;
}

//------------------------------------------------------------------------------

data::Array::sptr Mesh::getDataArray(const std::string& name) const
{
    data::Array::sptr res;
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

} //namespace sight::data
