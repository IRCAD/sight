/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include <iterator>

namespace sight::data
{

namespace iterator
{

//------------------------------------------------------------------------------

PointIterator::PointIterator()
{
}

//------------------------------------------------------------------------------

PointIterator::PointIterator(data::Mesh* mesh)
{
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointInfo->point = static_cast<PointInfo::point_value_type*>(mesh->m_points->getBuffer());

    if(mesh->hasPointColors() && mesh->m_pointColors->getElementSizeInBytes() == 4)
    {
        m_pointInfo->rgba = static_cast<PointInfo::rgba_value_type*>(mesh->m_pointColors->getBuffer());
    }
    else if(mesh->hasPointColors() && mesh->m_pointColors->getElementSizeInBytes() == 3)
    {
        m_pointInfo->rgb = static_cast<PointInfo::rgb_value_type*>(mesh->m_pointColors->getBuffer());
    }

    if(mesh->hasPointNormals())
    {
        m_pointInfo->normal = static_cast<PointInfo::normal_value_type*>(mesh->m_pointNormals->getBuffer());
    }

    if(mesh->hasPointTexCoords())
    {
        m_pointInfo->tex = static_cast<PointInfo::tex_value_type*>(mesh->m_pointTexCoords->getBuffer());
    }
}

//------------------------------------------------------------------------------

PointIterator::PointIterator(const PointIteratorBase& other) :
    PointIteratorBase(other)
{
}

//------------------------------------------------------------------------------

PointIterator::~PointIterator()
{
}

//------------------------------------------------------------------------------

PointIterator& PointIterator::operator=(const PointIteratorBase& other)
{
    if(this != &other)
    {
        m_pointInfo->point  = other.m_pointInfo->point;
        m_pointInfo->rgb    = other.m_pointInfo->rgb;
        m_pointInfo->rgba   = other.m_pointInfo->rgba;
        m_pointInfo->normal = other.m_pointInfo->normal;
        m_pointInfo->tex    = other.m_pointInfo->tex;
        m_idx               = other.m_idx;
        m_numberOfElements  = other.m_numberOfElements;
    }

    return *this;
}

//------------------------------------------------------------------------------

ConstPointIterator::ConstPointIterator()
{
}

//------------------------------------------------------------------------------

ConstPointIterator::ConstPointIterator(const data::Mesh* mesh)
{
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointInfo->point = static_cast<PointInfo::point_value_type*>(mesh->m_points->getBuffer());

    if(mesh->hasPointColors() && mesh->m_pointColors->getElementSizeInBytes() == 4)
    {
        m_pointInfo->rgba = static_cast<PointInfo::rgba_value_type*>(mesh->m_pointColors->getBuffer());
    }
    else if(mesh->hasPointColors() && mesh->m_pointColors->getElementSizeInBytes() == 3)
    {
        m_pointInfo->rgb = static_cast<PointInfo::rgb_value_type*>(mesh->m_pointColors->getBuffer());
    }

    if(mesh->hasPointNormals())
    {
        m_pointInfo->normal = static_cast<PointInfo::normal_value_type*>(mesh->m_pointNormals->getBuffer());
    }

    if(mesh->hasPointTexCoords())
    {
        m_pointInfo->tex = static_cast<PointInfo::tex_value_type*>(mesh->m_pointTexCoords->getBuffer());
    }
}

//------------------------------------------------------------------------------

ConstPointIterator::ConstPointIterator(const PointIteratorBase& other) :
    PointIteratorBase(other)
{
}

//------------------------------------------------------------------------------

ConstPointIterator::~ConstPointIterator()
{
}

//------------------------------------------------------------------------------

ConstPointIterator& ConstPointIterator::operator=(const PointIteratorBase& other)
{
    if(this != &other)
    {
        m_pointInfo->point  = other.m_pointInfo->point;
        m_pointInfo->rgb    = other.m_pointInfo->rgb;
        m_pointInfo->rgba   = other.m_pointInfo->rgba;
        m_pointInfo->normal = other.m_pointInfo->normal;
        m_pointInfo->tex    = other.m_pointInfo->tex;
        m_idx               = other.m_idx;
        m_numberOfElements  = other.m_numberOfElements;
    }

    return *this;
}

//------------------------------------------------------------------------------

CellIterator::CellIterator()
{
}

//------------------------------------------------------------------------------

CellIterator::CellIterator(data::Mesh* mesh)
{
    m_numberOfElements   = static_cast<difference_type>(mesh->getNumberOfCells());
    m_cellDataSize       = mesh->getCellDataSize();
    m_cellInfo->pointIdx = static_cast<CellInfo::cell_data_value_type*>(mesh->m_cellData->getBuffer());
    m_cellInfo->offset   = static_cast<CellInfo::cell_offset_value_type*>(mesh->m_cellDataOffsets->getBuffer());
    m_cellInfo->type     = static_cast<CellInfo::cell_type_value_type*>(mesh->m_cellTypes->getBuffer());

    m_cellInfo->numberOfElements = m_numberOfElements;

    if(mesh->hasCellColors() && mesh->m_cellColors->getElementSizeInBytes() == 4)
    {
        m_cellInfo->rgba = static_cast<CellInfo::rgba_value_type*>(mesh->m_cellColors->getBuffer());
    }
    else if(mesh->hasCellColors() && mesh->m_cellColors->getElementSizeInBytes() == 3)
    {
        m_cellInfo->rgb = static_cast<CellInfo::rgb_value_type*>(mesh->m_cellColors->getBuffer());
    }

    if(mesh->hasCellNormals())
    {
        m_cellInfo->normal = static_cast<CellInfo::normal_value_type*>(mesh->m_cellNormals->getBuffer());
    }

    if(mesh->hasCellTexCoords())
    {
        m_cellInfo->tex = static_cast<CellInfo::tex_value_type*>(mesh->m_cellTexCoords->getBuffer());
    }

    const CellInfo::cell_offset_value_type nextOffset = *(m_cellInfo->offset + 1);
    m_cellInfo->nbPoints = nextOffset;
}

//------------------------------------------------------------------------------

CellIterator::~CellIterator()
{
}

//------------------------------------------------------------------------------

CellIterator& CellIterator::operator=(const CellIteratorBase& other)
{
    if(this != &other)
    {
        m_idx                = other.m_idx;
        m_numberOfElements   = other.m_numberOfElements;
        m_cellDataSize       = other.m_cellDataSize;
        m_currentOffset      = other.m_currentOffset;
        m_cellInfo->pointIdx = other.m_cellInfo->pointIdx;
        m_cellInfo->type     = other.m_cellInfo->type;
        m_cellInfo->offset   = other.m_cellInfo->offset;
        m_cellInfo->normal   = other.m_cellInfo->normal;
        m_cellInfo->rgba     = other.m_cellInfo->rgba;
        m_cellInfo->tex      = other.m_cellInfo->tex;
        m_cellInfo->nbPoints = other.m_cellInfo->nbPoints;

        m_cellInfo->idx              = other.m_cellInfo->idx;
        m_cellInfo->numberOfElements = other.m_cellInfo->numberOfElements;
    }

    return *this;
}

//------------------------------------------------------------------------------

ConstCellIterator::ConstCellIterator()
{
}

//------------------------------------------------------------------------------

ConstCellIterator::ConstCellIterator(const data::Mesh* mesh)
{
    m_numberOfElements   = static_cast<difference_type>(mesh->getNumberOfCells());
    m_cellDataSize       = mesh->getCellDataSize();
    m_cellInfo->pointIdx = static_cast<CellInfo::cell_data_value_type*>(mesh->m_cellData->getBuffer());
    m_cellInfo->offset   = static_cast<CellInfo::cell_offset_value_type*>(mesh->m_cellDataOffsets->getBuffer());
    m_cellInfo->type     = static_cast<CellInfo::cell_type_value_type*>(mesh->m_cellTypes->getBuffer());

    m_cellInfo->numberOfElements = m_numberOfElements;

    if(mesh->hasCellColors() && mesh->m_cellColors->getElementSizeInBytes() == 4)
    {
        m_cellInfo->rgba = static_cast<CellInfo::rgba_value_type*>(mesh->m_cellColors->getBuffer());
    }
    else if(mesh->hasCellColors() && mesh->m_cellColors->getElementSizeInBytes() == 3)
    {
        m_cellInfo->rgb = static_cast<CellInfo::rgb_value_type*>(mesh->m_cellColors->getBuffer());
    }

    if(mesh->hasCellNormals())
    {
        m_cellInfo->normal = static_cast<CellInfo::normal_value_type*>(mesh->m_cellNormals->getBuffer());
    }

    if(mesh->hasCellTexCoords())
    {
        m_cellInfo->tex = static_cast<CellInfo::tex_value_type*>(mesh->m_cellTexCoords->getBuffer());
    }

    const CellInfo::cell_offset_value_type nextOffset = *(m_cellInfo->offset + 1);
    m_cellInfo->nbPoints = nextOffset;
}

//------------------------------------------------------------------------------

ConstCellIterator::ConstCellIterator(const CellIterator& other)
{
    m_idx                = other.m_idx;
    m_numberOfElements   = other.m_numberOfElements;
    m_cellDataSize       = other.m_cellDataSize;
    m_currentOffset      = other.m_currentOffset;
    m_cellInfo->pointIdx = other.m_cellInfo->pointIdx;
    m_cellInfo->type     = other.m_cellInfo->type;
    m_cellInfo->offset   = other.m_cellInfo->offset;
    m_cellInfo->normal   = other.m_cellInfo->normal;
    m_cellInfo->rgba     = other.m_cellInfo->rgba;
    m_cellInfo->tex      = other.m_cellInfo->tex;
    m_cellInfo->nbPoints = other.m_cellInfo->nbPoints;

    m_cellInfo->idx              = other.m_cellInfo->idx;
    m_cellInfo->numberOfElements = other.m_cellInfo->numberOfElements;
}

//------------------------------------------------------------------------------

ConstCellIterator::~ConstCellIterator()
{
}

//------------------------------------------------------------------------------

ConstCellIterator& ConstCellIterator::operator=(const CellIteratorBase& other)
{
    if(this != &other)
    {
        m_idx                = other.m_idx;
        m_numberOfElements   = other.m_numberOfElements;
        m_cellDataSize       = other.m_cellDataSize;
        m_currentOffset      = other.m_currentOffset;
        m_cellInfo->pointIdx = other.m_cellInfo->pointIdx;
        m_cellInfo->type     = other.m_cellInfo->type;
        m_cellInfo->offset   = other.m_cellInfo->offset;
        m_cellInfo->normal   = other.m_cellInfo->normal;
        m_cellInfo->rgba     = other.m_cellInfo->rgba;
        m_cellInfo->tex      = other.m_cellInfo->tex;
        m_cellInfo->nbPoints = other.m_cellInfo->nbPoints;

        m_cellInfo->idx              = other.m_cellInfo->idx;
        m_cellInfo->numberOfElements = other.m_cellInfo->numberOfElements;
    }

    return *this;
}

//------------------------------------------------------------------------------

} // namespace iterator

} // namespace sight::data
