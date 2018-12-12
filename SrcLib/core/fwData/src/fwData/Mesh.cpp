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

#include "fwData/Mesh.hpp"

#include <fwCom/Signal.hxx>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include <functional>
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

Mesh::Mesh(::fwData::Object::Key key) : m_nbPoints(0),
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

    m_points->setType(::fwTools::Type::create<PointValueType>());
    m_points->setNumberOfComponents(3);

    m_cellTypes->setType(::fwTools::Type::create<CellTypes>());
    m_cellTypes->setNumberOfComponents(1);

    m_cellData->setType(::fwTools::Type::create<CellValueType>());
    m_cellData->setNumberOfComponents(1);

    m_cellDataOffsets->setType(::fwTools::Type::create<CellDataOffsetType>());
    m_cellDataOffsets->setNumberOfComponents(1);
}

//------------------------------------------------------------------------------

void Mesh::shallowCopy(const Object::csptr &_source )
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

void Mesh::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
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

size_t Mesh::allocate(size_t nbPts, size_t nbCells, size_t nbCellsData)
{
    if (nbCellsData == 0)
    {
        nbCellsData = 3 * nbCells;
    }

    size_t allocatedSize = 0;

    allocatedSize += m_points->resize( {nbPts}, true );
    allocatedSize += m_cellTypes->resize( {nbCells}, true );
    allocatedSize += m_cellData->resize( {nbCellsData}, true );
    allocatedSize += m_cellDataOffsets->resize( {nbCells}, true );

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

bool Mesh::adjustAllocatedMemory()
{
    size_t oldAllocatedSize = this->getAllocatedSizeInBytes();

    if(!m_points)
    {
        this->initArrays();
    }

    m_points->resize({size_t(m_nbPoints)}, true);
    m_cellTypes->resize({size_t(m_nbCells)}, true);
    m_cellData->resize({size_t(m_cellsDataSize)}, true);
    m_cellDataOffsets->resize({size_t(m_nbCells)}, true);

    m_pointColors    && (m_pointColors->resize({size_t(m_nbPoints)}, true));
    m_cellColors     && (m_cellColors->resize({size_t(m_nbCells)}, true));
    m_pointNormals   && (m_pointNormals->resize({size_t(m_nbPoints)}, true));
    m_cellNormals    && (m_cellNormals->resize({size_t(m_nbCells)}, true));
    m_pointTexCoords && (m_pointTexCoords->resize({size_t(m_nbPoints)}, true));
    m_cellTexCoords  && (m_cellTexCoords->resize({size_t(m_nbCells)}, true));


    size_t newAllocatedSize = this->getAllocatedSizeInBytes();
    SLM_ASSERT("Error adjusting memory : allocated size: " << newAllocatedSize
                                                           << " != data size : " << this->getDataSizeInBytes(),
               newAllocatedSize == this->getDataSizeInBytes());
    return oldAllocatedSize != newAllocatedSize;
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

    m_points          && (size += m_points->getElementSizeInBytes() * m_nbPoints);
    m_cellTypes       && (size += m_cellTypes->getElementSizeInBytes() * m_nbCells );
    m_cellData        && (size += m_cellData->getElementSizeInBytes() * m_cellsDataSize);
    m_cellDataOffsets && (size += m_cellDataOffsets->getElementSizeInBytes() * m_nbCells);
    m_pointColors     && (size += m_pointColors->getElementSizeInBytes() * m_nbPoints);
    m_cellColors      && (size += m_cellColors->getElementSizeInBytes() * m_nbCells);
    m_pointNormals    && (size += m_pointNormals->getElementSizeInBytes() * m_nbPoints);
    m_cellNormals     && (size += m_cellNormals->getElementSizeInBytes() * m_nbCells);
    m_pointTexCoords  && (size += m_pointTexCoords->getElementSizeInBytes() * m_nbPoints);
    m_cellTexCoords   && (size += m_cellTexCoords->getElementSizeInBytes() * m_nbCells);

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
    m_pointTexCoords  && (size += m_pointTexCoords->getSizeInBytes());
    m_cellTexCoords   && (size += m_cellTexCoords->getSizeInBytes());

    return size;
}

//------------------------------------------------------------------------------

void Mesh::addDataArray(const std::string &name, ::fwData::Array::sptr array)
{
    m_arrayMap[name] = array;
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

std::vector<std::string> Mesh::getDataArrayNames() const
{
    std::vector<std::string> vectNames;
    std::transform( m_arrayMap.begin(), m_arrayMap.end(),
                    std::back_inserter(vectNames),
                    std::bind(&ArrayMapType::value_type::first, std::placeholders::_1) );
    return vectNames;
}

//------------------------------------------------------------------------------

void Mesh::removeDataArray(const std::string &name)
{
    m_arrayMap.erase(name);
}

//------------------------------------------------------------------------------

} //namespace fwData
