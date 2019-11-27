/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <fwData/Mesh.hpp>

#include <iterator>

namespace fwData
{

namespace iterator
{

//------------------------------------------------------------------------------

PointIterator::PointIterator(::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_points->lock());
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointInfo.point  = static_cast<point_value_type*>(mesh->m_points->getBuffer());

    if (mesh->m_pointColors && mesh->m_pointColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_pointColors->lock());
        m_pointInfo.color = static_cast<color_value_type*>(mesh->m_pointColors->getBuffer());
    }
    if (mesh->m_pointNormals)
    {
        m_locks.push_back(mesh->m_pointNormals->lock());
        m_pointInfo.normal = static_cast<normal_value_type*>(mesh->m_pointNormals->getBuffer());
    }
    if (mesh->m_pointTexCoords)
    {
        m_locks.push_back(mesh->m_pointTexCoords->lock());
        m_pointInfo.tex = static_cast<tex_value_type*>(mesh->m_pointTexCoords->getBuffer());
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
    if (this != &other)
    {
        m_pointInfo        = other.m_pointInfo;
        m_locks            = other.m_locks;
        m_idx              = other.m_idx;
        m_numberOfElements = other.m_numberOfElements;
    }
    return *this;
}

//------------------------------------------------------------------------------

ConstPointIterator::ConstPointIterator(const ::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_points->lock());
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointInfo.point  = static_cast<point_value_type*>(mesh->m_points->getBuffer());

    if (mesh->m_pointColors && mesh->m_pointColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_pointColors->lock());
        m_pointInfo.color = static_cast<color_value_type*>(mesh->m_pointColors->getBuffer());
    }
    if (mesh->m_pointNormals)
    {
        m_locks.push_back(mesh->m_pointNormals->lock());
        m_pointInfo.normal = static_cast<normal_value_type*>(mesh->m_pointNormals->getBuffer());
    }
    if (mesh->m_pointTexCoords)
    {
        m_locks.push_back(mesh->m_pointTexCoords->lock());
        m_pointInfo.tex = static_cast<tex_value_type*>(mesh->m_pointTexCoords->getBuffer());
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
    if (this != &other)
    {
        m_pointInfo        = other.m_pointInfo;
        m_locks            = other.m_locks;
        m_idx              = other.m_idx;
        m_numberOfElements = other.m_numberOfElements;
    }
    return *this;
}

//------------------------------------------------------------------------------

CellIterator::CellIterator(::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_cellData->lock());
    m_locks.push_back(mesh->m_cellTypes->lock());
    m_locks.push_back(mesh->m_cellDataOffsets->lock());
    m_numberOfElements  = static_cast<difference_type>(mesh->getNumberOfCells());
    m_cellDataSize      = mesh->getCellDataSize();
    m_cellInfo.pointIdx = static_cast<cell_data_value_type*>(mesh->m_cellData->getBuffer());
    m_cellInfo.offset   = static_cast<cell_offset_value_type*>(mesh->m_cellDataOffsets->getBuffer());
    m_cellInfo.type     = static_cast<cell_type_value_type*>(mesh->m_cellTypes->getBuffer());

    if (mesh->m_cellColors && mesh->m_cellColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_cellColors->lock());
        m_cellInfo.color = static_cast<color_value_type*>(mesh->m_cellColors->getBuffer());
    }
    if (mesh->m_cellNormals)
    {
        m_locks.push_back(mesh->m_cellNormals->lock());
        m_cellInfo.normal = static_cast<normal_value_type*>(mesh->m_cellNormals->getBuffer());
    }
    if (mesh->m_cellTexCoords)
    {
        m_locks.push_back(mesh->m_cellTexCoords->lock());
        m_cellInfo.tex = static_cast<tex_value_type*>(mesh->m_cellTexCoords->getBuffer());
    }
    const cell_offset_value_type nextOffset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset+1);
    m_cellInfo.nbPoints = nextOffset;
}

//------------------------------------------------------------------------------

CellIterator::~CellIterator()
{
    m_locks.clear();
}

//------------------------------------------------------------------------------

ConstCellIterator::ConstCellIterator(const ::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_cellData->lock());
    m_locks.push_back(mesh->m_cellTypes->lock());
    m_locks.push_back(mesh->m_cellDataOffsets->lock());
    m_numberOfElements  = static_cast<difference_type>(mesh->getNumberOfCells());
    m_cellDataSize      = mesh->getCellDataSize();
    m_cellInfo.pointIdx = static_cast<cell_data_value_type*>(mesh->m_cellData->getBuffer());
    m_cellInfo.offset   = static_cast<cell_offset_value_type*>(mesh->m_cellDataOffsets->getBuffer());
    m_cellInfo.type     = static_cast<cell_type_value_type*>(mesh->m_cellTypes->getBuffer());

    if (mesh->m_cellColors && mesh->m_cellColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_cellColors->lock());
        m_cellInfo.color = static_cast<color_value_type*>(mesh->m_cellColors->getBuffer());
    }
    if (mesh->m_cellNormals)
    {
        m_locks.push_back(mesh->m_cellNormals->lock());
        m_cellInfo.normal = static_cast<normal_value_type*>(mesh->m_cellNormals->getBuffer());
    }
    if (mesh->m_cellTexCoords)
    {
        m_locks.push_back(mesh->m_cellTexCoords->lock());
        m_cellInfo.tex = static_cast<tex_value_type*>(mesh->m_cellTexCoords->getBuffer());
    }
    const cell_offset_value_type nextOffset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset+1);
    m_cellInfo.nbPoints = nextOffset;
}

//------------------------------------------------------------------------------

ConstCellIterator::ConstCellIterator(const CellIterator& other)
{
    m_locks             = other.m_locks;
    m_idx               = other.m_idx;
    m_numberOfElements  = other.m_numberOfElements;
    m_cellDataSize      = other.m_cellDataSize;
    m_cellInfo.pointIdx = other.m_cellInfo.pointIdx;
    m_cellInfo.type     = other.m_cellInfo.type;
    m_cellInfo.offset   = other.m_cellInfo.offset;
    m_cellInfo.normal   = other.m_cellInfo.normal;
    m_cellInfo.color    = other.m_cellInfo.color;
    m_cellInfo.tex      = other.m_cellInfo.tex;
    m_cellInfo.nbPoints = other.m_cellInfo.nbPoints;
}

//------------------------------------------------------------------------------

ConstCellIterator::~ConstCellIterator()
{
    m_locks.clear();
}

//------------------------------------------------------------------------------

} // namespace iterator
} // namespace fwData
