/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <cstdlib>
#include <functional>
#include <numeric>

#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Mesh.hpp"

using namespace boost::assign;

namespace fwData
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000


fwDataRegisterMacro( ::fwData::Mesh );

//------------------------------------------------------------------------------

Mesh::Mesh(::fwData::Object::Key key) : m_nbPoints(0), m_nbCells(0),  m_cellsDataSize(0)
{
    this->initArrays();
}

//------------------------------------------------------------------------------

Mesh::~Mesh()
{}

//------------------------------------------------------------------------------

void Mesh::initArrays()
{
    if (!m_points)
    {
        m_points          = ::fwData::Array::New();
    }
    if (!m_cellTypes)
    {
        m_cellTypes       = ::fwData::Array::New();
    }
    if (!m_cellData)
    {
        m_cellData        = ::fwData::Array::New();
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

void Mesh::shallowCopy( Mesh::csptr _source )
{
    this->fieldShallowCopy( _source );

    m_nbPoints        = _source->m_nbPoints;
    m_nbCells         = _source->m_nbCells;
    m_cellsDataSize   = _source->m_cellsDataSize;

    m_points          = _source->m_points;
    m_cellTypes       = _source->m_cellTypes;
    m_cellData        = _source->m_cellData;
    m_cellDataOffsets = _source->m_cellDataOffsets;

    m_pointColors     = _source->m_pointColors;
    m_cellColors      = _source->m_cellColors;
    m_pointNormals    = _source->m_pointNormals;
    m_cellNormals     = _source->m_cellNormals;

    m_arrayMap        = _source->m_arrayMap;
}

//------------------------------------------------------------------------------

void Mesh::deepCopy( Mesh::csptr _source )
{
    this->fieldDeepCopy( _source );

    m_nbPoints      = _source->m_nbPoints;
    m_nbCells       = _source->m_nbCells;
    m_cellsDataSize = _source->m_cellsDataSize;

    this->initArrays();

    m_points->deepCopy(_source->m_points);
    m_cellTypes->deepCopy(_source->m_cellTypes);
    m_cellData->deepCopy(_source->m_cellData);
    m_cellDataOffsets->deepCopy(_source->m_cellDataOffsets);

    m_pointColors  = ::fwData::Object::copy(_source->m_pointColors );
    m_cellColors   = ::fwData::Object::copy(_source->m_cellColors  );
    m_pointNormals = ::fwData::Object::copy(_source->m_pointNormals);
    m_cellNormals  = ::fwData::Object::copy(_source->m_cellNormals );

    m_arrayMap.clear();
    BOOST_FOREACH(ArrayMapType::value_type element, _source->m_arrayMap)
    {
        m_arrayMap[element.first]->deepCopy(element.second);
    }

}

//------------------------------------------------------------------------------

size_t Mesh::allocate(size_t nbPts, size_t nbCells, size_t nbCellsData) throw(::fwData::Exception)
{
    if (nbCellsData == 0)
    {
        nbCellsData = 3*nbCells;
    }

    size_t allocatedSize = 0;

    allocatedSize += m_points->resize         ( list_of(nbPts)      , true);
    allocatedSize += m_cellTypes->resize      ( list_of(nbCells)    , true);
    allocatedSize += m_cellData->resize       ( list_of(nbCellsData), true);
    allocatedSize += m_cellDataOffsets->resize( list_of(nbCells)    , true);

    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointNormals() throw(::fwData::Exception)
{
    size_t allocatedSize = 0;
    if (! m_pointNormals )
    {
        m_pointNormals  = ::fwData::Array::New();
    }
    allocatedSize += m_pointNormals->resize( ::fwTools::Type::create<NormalValueType>(), list_of(m_nbPoints), 3, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocatePointColors(ColorArrayTypes t) throw(::fwData::Exception)
{
    OSLM_ASSERT("Bad ColorArrayTypes : " << t, t == RGB || t == RGBA);
    size_t allocatedSize = 0;
    if (! m_pointColors )
    {
        m_pointColors  = ::fwData::Array::New();
    }
    allocatedSize += m_pointColors->resize( ::fwTools::Type::create<ColorValueType>(), list_of(m_nbPoints), t, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellNormals() throw(::fwData::Exception)
{
    size_t allocatedSize = 0;
    if (! m_cellNormals )
    {
        m_cellNormals  = ::fwData::Array::New();
    }
    allocatedSize += m_cellNormals->resize( ::fwTools::Type::create<NormalValueType>(), list_of(m_nbCells), 3, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

size_t Mesh::allocateCellColors(ColorArrayTypes t) throw(::fwData::Exception)
{
    OSLM_ASSERT("Bad ColorArrayTypes : " << t, t == RGB || t == RGBA);
    size_t allocatedSize = 0;
    if (! m_cellColors )
    {
        m_cellColors  = ::fwData::Array::New();
    }
    allocatedSize += m_cellColors->resize( ::fwTools::Type::create<ColorValueType>(), list_of(m_nbCells), t, true);
    return allocatedSize;
}

//------------------------------------------------------------------------------

bool Mesh::adjustAllocatedMemory() throw(::fwData::Exception)
{
    size_t oldAllocatedSize = this->getAllocatedSizeInBytes();

    if(! m_points)
    {
        this->initArrays();
    }

    m_points->resize(list_of(m_nbPoints), true);
    m_cellTypes->resize(list_of(m_nbCells), true);
    m_cellData->resize(list_of(m_cellsDataSize), true);
    m_cellDataOffsets->resize(list_of(m_nbCells), true);
    m_pointColors  && (m_pointColors->resize(list_of(m_nbPoints), true));
    m_cellColors   && (m_cellColors->resize(list_of(m_nbCells), true));
    m_pointNormals && (m_pointNormals->resize(list_of(m_nbPoints), true));
    m_cellNormals  && (m_cellNormals->resize(list_of(m_nbCells), true));


    size_t newAllocatedSize = this->getAllocatedSizeInBytes();
    SLM_ASSERT(
            "Error adjusting memory : allocated size: " << newAllocatedSize
            << " != data size : " << this->getDataSizeInBytes(),
            newAllocatedSize == this->getDataSizeInBytes());
    return oldAllocatedSize != newAllocatedSize;
}

//------------------------------------------------------------------------------

void Mesh::setPointsArray (::fwData::Array::sptr array)
{
    m_points = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellTypesArray (::fwData::Array::sptr array)
{
    m_cellTypes = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataArray (::fwData::Array::sptr array)
{
    m_cellData = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellDataOffsetsArray (::fwData::Array::sptr array)
{
    m_cellDataOffsets = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointColorsArray (::fwData::Array::sptr array)
{
    m_pointColors = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellColorsArray (::fwData::Array::sptr array)
{
    m_cellColors = array;
}

//------------------------------------------------------------------------------

void Mesh::setPointNormalsArray (::fwData::Array::sptr array)
{
    m_pointNormals = array;
}

//------------------------------------------------------------------------------

void Mesh::setCellNormalsArray (::fwData::Array::sptr array)
{
    m_cellNormals = array;
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

::fwData::Array::sptr Mesh::getPointColorsArray () const
{
    return m_pointColors;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellColorsArray () const
{
    return m_cellColors;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getPointNormalsArray () const
{
    return m_pointNormals;
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Mesh::getCellNormalsArray () const
{
    return m_cellNormals;
}

//------------------------------------------------------------------------------

void Mesh::clearPoints()
{
    m_nbPoints = 0;
}

//------------------------------------------------------------------------------

void Mesh::clearCells()
{
    m_nbCells = 0;
    m_cellsDataSize = 0;
}

//------------------------------------------------------------------------------

void Mesh::clear()
{
    this->clearPointNormals();
    this->clearPointColors();
    this->clearCellNormals();
    this->clearCellColors();

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
            ::boost::bind(& ArrayMapType::value_type::first,_1) );
    return vectNames;
}

//------------------------------------------------------------------------------

void Mesh::removeDataArray(const std::string &name)
{
    m_arrayMap.erase(name);
}

//------------------------------------------------------------------------------

}//namespace fwData
