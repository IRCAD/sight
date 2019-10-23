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

MeshIteratorBase::MeshIteratorBase()
{

}

//------------------------------------------------------------------------------

MeshIteratorBase::MeshIteratorBase(const MeshIteratorBase& other) :
    m_pointers(other.m_pointers),
    m_locks(other.m_locks),
    m_idx(other.m_idx),
    m_numberOfElements(other.m_numberOfElements),
    m_elementSizes(other.m_elementSizes),
    m_nbArrays(other.m_nbArrays)
{

}

//------------------------------------------------------------------------------

MeshIteratorBase::~MeshIteratorBase()
{
    m_locks.clear();
}

//------------------------------------------------------------------------------

MeshIteratorBase& MeshIteratorBase::operator=(const MeshIteratorBase& other)
{
    if (this != &other)
    {
        m_pointers         = other.m_pointers;
        m_locks            = other.m_locks;
        m_idx              = other.m_idx;
        m_numberOfElements = other.m_numberOfElements;
        m_elementSizes     = other.m_elementSizes;
        m_nbArrays         = other.m_nbArrays;
    }
    return *this;
}

//------------------------------------------------------------------------------

bool MeshIteratorBase::operator==(const MeshIteratorBase& other) const
{
    return m_idx == other.m_idx;
}
//------------------------------------------------------------------------------

bool MeshIteratorBase::operator!=(const MeshIteratorBase& other) const
{
    return m_idx != other.m_idx;
}

//------------------------------------------------------------------------------

MeshIteratorBase& MeshIteratorBase::operator++()
{
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] += m_elementSizes[i];
    }
    return *this;
}
//------------------------------------------------------------------------------

MeshIteratorBase MeshIteratorBase::operator++(int)
{
    MeshIteratorBase tmp(*this);
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] += m_elementSizes[i];
    }
    return tmp;
}
//------------------------------------------------------------------------------

MeshIteratorBase MeshIteratorBase::operator+(difference_type index)
{
    MeshIteratorBase tmp(*this);
    tmp += index;
    return tmp;
}
//------------------------------------------------------------------------------

MeshIteratorBase& MeshIteratorBase::operator+=(difference_type index)
{
    m_idx += index;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] += index*m_elementSizes[i];
    }
    return *this;
}
//------------------------------------------------------------------------------

MeshIteratorBase& MeshIteratorBase::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] -= m_elementSizes[i];
    }
    return *this;
}
//------------------------------------------------------------------------------

MeshIteratorBase MeshIteratorBase::operator--(int)
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    MeshIteratorBase tmp(*this);
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] -= m_elementSizes[i];
    }
    return tmp;
}

//------------------------------------------------------------------------------

MeshIteratorBase MeshIteratorBase::operator-(difference_type index)
{
    MeshIteratorBase tmp(*this);
    tmp -= index;
    return tmp;
}
//------------------------------------------------------------------------------

MeshIteratorBase& MeshIteratorBase::operator-=(difference_type index)
{
    SLM_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                             << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );
    m_idx -= index;
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] -= index*m_elementSizes[i];
    }
    return *this;
}

//------------------------------------------------------------------------------

MeshIteratorBase::difference_type MeshIteratorBase::operator+(const MeshIteratorBase& other) const
{
    return m_idx + other.m_idx;
}
//------------------------------------------------------------------------------

MeshIteratorBase::difference_type MeshIteratorBase::operator-(const MeshIteratorBase& other) const
{
    return m_idx - other.m_idx;
}

//------------------------------------------------------------------------------

PointIterator::PointIterator(::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_points->lock());
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointers[0]      = static_cast<char*>(mesh->m_points->getBuffer());
    m_elementSizes[0]  = sizeof (point_value_type);
    m_nbArrays         = 1;

    if (mesh->m_pointColors && mesh->m_pointColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_pointColors->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_pointColors->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (color_value_type);
        m_colorIdx                 = m_nbArrays;
        ++m_nbArrays;
    }
    if (mesh->m_pointNormals)
    {
        m_locks.push_back(mesh->m_pointNormals->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_pointNormals->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (normal_value_type);
        m_normalIdx                = m_nbArrays;
        ++m_nbArrays;
    }
    if (mesh->m_pointTexCoords)
    {
        m_locks.push_back(mesh->m_pointTexCoords->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_pointTexCoords->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (tex_value_type);
        m_texIdx                   = m_nbArrays;
        ++m_nbArrays;
    }
}

//------------------------------------------------------------------------------

PointIterator::PointIterator(const MeshIteratorBase& other) :
    MeshIteratorBase(other)
{

}

//------------------------------------------------------------------------------

PointIterator::~PointIterator()
{

}

//------------------------------------------------------------------------------

PointIterator& PointIterator::operator=(const MeshIteratorBase& other)
{
    if (this != &other)
    {
        m_pointers         = other.m_pointers;
        m_locks            = other.m_locks;
        m_idx              = other.m_idx;
        m_numberOfElements = other.m_numberOfElements;
        m_elementSizes     = other.m_elementSizes;
        m_nbArrays         = other.m_nbArrays;
    }
    return *this;
}

//------------------------------------------------------------------------------

PointIterator::point_reference PointIterator::point()
{
    return *(reinterpret_cast<point_value_type*>(m_pointers[0]));
}

//------------------------------------------------------------------------------

PointIterator::color_reference PointIterator::color()
{
    return *(reinterpret_cast<color_value_type*>(m_pointers[m_colorIdx]));
}

//------------------------------------------------------------------------------

PointIterator::normal_reference PointIterator::normal()
{
    return *(reinterpret_cast<normal_value_type*>(m_pointers[m_normalIdx]));
}

//------------------------------------------------------------------------------

PointIterator::tex_reference PointIterator::tex()
{
    return *(reinterpret_cast<tex_value_type*>(m_pointers[m_texIdx]));
}

//------------------------------------------------------------------------------

PointIterator::point_reference PointIterator::operator*()
{
    return *(reinterpret_cast<point_value_type*>(m_pointers[0]));
}

//------------------------------------------------------------------------------

PointIterator* PointIterator::operator->()
{
    return this;
}

//------------------------------------------------------------------------------

ConstPointIterator::ConstPointIterator(const ::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_points->lock());
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfPoints());
    m_pointers[0]      = static_cast<char*>(mesh->m_points->getBuffer());
    m_elementSizes[0]  = sizeof (point_value_type);
    m_nbArrays         = 1;

    if (mesh->m_pointColors && mesh->m_pointColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_pointColors->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_pointColors->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (color_value_type);
        ++m_nbArrays;
    }
    if (mesh->m_pointNormals)
    {
        m_locks.push_back(mesh->m_pointNormals->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_pointNormals->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (normal_value_type);
        ++m_nbArrays;
    }
    if (mesh->m_pointTexCoords)
    {
        m_locks.push_back(mesh->m_pointTexCoords->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_pointTexCoords->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (tex_value_type);
        ++m_nbArrays;
    }
}

//------------------------------------------------------------------------------

ConstPointIterator::ConstPointIterator(const MeshIteratorBase& other) :
    MeshIteratorBase(other)
{

}

//------------------------------------------------------------------------------

ConstPointIterator::~ConstPointIterator()
{

}

//------------------------------------------------------------------------------

ConstPointIterator& ConstPointIterator::operator=(const MeshIteratorBase& other)
{
    if (this != &other)
    {
        m_pointers         = other.m_pointers;
        m_locks            = other.m_locks;
        m_idx              = other.m_idx;
        m_numberOfElements = other.m_numberOfElements;
        m_elementSizes     = other.m_elementSizes;
        m_nbArrays         = other.m_nbArrays;
    }
    return *this;
}

//------------------------------------------------------------------------------

ConstPointIterator::point_reference ConstPointIterator::point()
{
    return *(reinterpret_cast<point_value_type*>(m_pointers[0]));
}

//------------------------------------------------------------------------------

ConstPointIterator::color_reference ConstPointIterator::color()
{
    return *(reinterpret_cast<color_value_type*>(m_pointers[m_colorIdx]));
}

//------------------------------------------------------------------------------

ConstPointIterator::normal_reference ConstPointIterator::normal()
{
    return *(reinterpret_cast<normal_value_type*>(m_pointers[m_normalIdx]));
}

//------------------------------------------------------------------------------

ConstPointIterator::tex_reference ConstPointIterator::tex()
{
    return *(reinterpret_cast<tex_value_type*>(m_pointers[m_texIdx]));
}

//------------------------------------------------------------------------------

ConstPointIterator::point_reference ConstPointIterator::operator*()
{
    return *(reinterpret_cast<point_value_type*>(m_pointers[0]));
}

//------------------------------------------------------------------------------

ConstPointIterator* ConstPointIterator::operator->()
{
    return this;
}

//------------------------------------------------------------------------------

CellIterator::CellIterator(::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_cellData->lock());
    m_locks.push_back(mesh->m_cellTypes->lock());
    m_locks.push_back(mesh->m_cellDataOffsets->lock());
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfCells());
    m_cellDataPointer  = static_cast<std::uint64_t*>(mesh->m_cellData->getBuffer());
    m_cellDataSize     = mesh->getCellDataSize();
    m_pointers[0]      = static_cast<char*>(mesh->m_cellDataOffsets->getBuffer());
    m_pointers[1]      = static_cast<char*>(mesh->m_cellTypes->getBuffer());
    m_elementSizes[0]  = sizeof (celloffset_value_type);
    m_elementSizes[1]  = sizeof (celltype_value_type);
    m_nbArrays         = 2;

    if (mesh->m_cellColors && mesh->m_cellColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_cellColors->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_cellColors->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (color_value_type);
        m_colorIdx                 = m_nbArrays;
        ++m_nbArrays;
    }
    if (mesh->m_cellNormals)
    {
        m_locks.push_back(mesh->m_cellNormals->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_cellNormals->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (normal_value_type);
        m_normalIdx                = m_nbArrays;
        ++m_nbArrays;
    }
    if (mesh->m_cellTexCoords)
    {
        m_locks.push_back(mesh->m_cellTexCoords->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_cellTexCoords->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (tex_value_type);
        m_texIdx                   = m_nbArrays;
        ++m_nbArrays;
    }
}

//------------------------------------------------------------------------------

CellIterator::CellIterator(const CellIterator& other) :
    MeshIteratorBase(other),
    m_cellDataPointer(other.m_cellDataPointer)
{

}

//------------------------------------------------------------------------------

CellIterator::~CellIterator()
{
    m_locks.clear();
}

//------------------------------------------------------------------------------

CellIterator& CellIterator::operator=(const CellIterator& other)
{
    if (this != &other)
    {
        m_cellDataPointer  = other.m_cellDataPointer;
        m_pointers         = other.m_pointers;
        m_locks            = other.m_locks;
        m_idx              = other.m_idx;
        m_numberOfElements = other.m_numberOfElements;
        m_elementSizes     = other.m_elementSizes;
        m_nbArrays         = other.m_nbArrays;
    }
    return *this;
}

//------------------------------------------------------------------------------

CellIterator::point_idx_reference CellIterator::pointIdx(size_t id)
{
    const point_idx_value_type offset = *reinterpret_cast<point_idx_value_type*>(m_pointers[0]) + id;
    return *(m_cellDataPointer + offset);
}

//------------------------------------------------------------------------------

CellIterator::color_reference CellIterator::color()
{
    return *(reinterpret_cast<color_value_type*>(m_pointers[m_colorIdx]));
}

//------------------------------------------------------------------------------

CellIterator::normal_reference CellIterator::normal()
{
    return *(reinterpret_cast<normal_value_type*>(m_pointers[m_normalIdx]));
}

//------------------------------------------------------------------------------

CellIterator::tex_reference CellIterator::tex()
{
    return *(reinterpret_cast<tex_value_type*>(m_pointers[m_texIdx]));
}

//------------------------------------------------------------------------------

CellIterator::celltype_reference CellIterator::cellType()
{
    return *(reinterpret_cast<celltype_value_type*>(m_pointers[1]));
}

//------------------------------------------------------------------------------

CellIterator::celloffset_reference CellIterator::cellOffset()
{
    return *(reinterpret_cast<celloffset_value_type*>(m_pointers[0]));
}

//------------------------------------------------------------------------------

size_t CellIterator::nbPoints() const
{
    const auto currentOffset = *(reinterpret_cast<celloffset_value_type*>(m_pointers[0]));
    celloffset_value_type nextOffset;
    if (m_idx < m_numberOfElements -1)
    {
        nextOffset = *(reinterpret_cast<celloffset_value_type*>(m_pointers[0])+1);
    }
    else
    {
        nextOffset = m_cellDataSize;
    }
    return nextOffset - currentOffset;
}

//------------------------------------------------------------------------------

ConstCellIterator::ConstCellIterator(const ::fwData::Mesh* mesh)
{
    m_locks.push_back(mesh->m_cellData->lock());
    m_locks.push_back(mesh->m_cellTypes->lock());
    m_locks.push_back(mesh->m_cellDataOffsets->lock());
    m_numberOfElements = static_cast<difference_type>(mesh->getNumberOfCells());
    m_cellDataPointer  = static_cast<const std::uint64_t*>(mesh->m_cellData->getBuffer());
    m_cellDataSize     = mesh->getCellDataSize();
    m_pointers[0]      = static_cast<char*>(mesh->m_cellDataOffsets->getBuffer());
    m_pointers[1]      = static_cast<char*>(mesh->m_cellTypes->getBuffer());
    m_elementSizes[0]  = sizeof (celloffset_value_type);
    m_elementSizes[1]  = sizeof (celltype_value_type);
    m_nbArrays         = 2;

    if (mesh->m_cellColors && mesh->m_cellColors->getElementSizeInBytes() == 4)
    {
        m_locks.push_back(mesh->m_cellColors->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_cellColors->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (color_value_type);
        m_colorIdx                 = m_nbArrays;
        ++m_nbArrays;
    }
    if (mesh->m_cellNormals)
    {
        m_locks.push_back(mesh->m_cellNormals->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_cellNormals->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (normal_value_type);
        m_normalIdx                = m_nbArrays;
        ++m_nbArrays;
    }
    if (mesh->m_cellTexCoords)
    {
        m_locks.push_back(mesh->m_cellTexCoords->lock());
        m_pointers[m_nbArrays]     = static_cast<char*>(mesh->m_cellTexCoords->getBuffer());
        m_elementSizes[m_nbArrays] = sizeof (tex_value_type);
        m_texIdx                   = m_nbArrays;
        ++m_nbArrays;
    }
}

//------------------------------------------------------------------------------

ConstCellIterator::ConstCellIterator(const CellIterator& other) :
    MeshIteratorBase(other),
    m_cellDataPointer(other.m_cellDataPointer)
{

}

//------------------------------------------------------------------------------

ConstCellIterator::~ConstCellIterator()
{
    m_locks.clear();
}

//------------------------------------------------------------------------------

ConstCellIterator& ConstCellIterator::operator=(const ConstCellIterator& other)
{
    if (this != &other)
    {
        m_cellDataPointer  = other.m_cellDataPointer;
        m_pointers         = other.m_pointers;
        m_locks            = other.m_locks;
        m_idx              = other.m_idx;
        m_numberOfElements = other.m_numberOfElements;
        m_elementSizes     = other.m_elementSizes;
        m_nbArrays         = other.m_nbArrays;
    }
    return *this;
}

//------------------------------------------------------------------------------

ConstCellIterator::point_idx_reference ConstCellIterator::pointIdx(size_t id)
{
    const point_idx_value_type offset = *reinterpret_cast<point_idx_value_type*>(m_pointers[0]) + id;
    return *(m_cellDataPointer + offset);
}

//------------------------------------------------------------------------------

ConstCellIterator::color_reference ConstCellIterator::color()
{
    return *(reinterpret_cast<color_value_type*>(m_pointers[m_colorIdx]));
}

//------------------------------------------------------------------------------

ConstCellIterator::normal_reference ConstCellIterator::normal()
{
    return *(reinterpret_cast<normal_value_type*>(m_pointers[m_normalIdx]));
}

//------------------------------------------------------------------------------

ConstCellIterator::tex_reference ConstCellIterator::tex()
{
    return *(reinterpret_cast<tex_value_type*>(m_pointers[m_texIdx]));
}

//------------------------------------------------------------------------------

ConstCellIterator::celltype_reference ConstCellIterator::cellType()
{
    return *(reinterpret_cast<celltype_value_type*>(m_pointers[1]));
}

//------------------------------------------------------------------------------

ConstCellIterator::celloffset_reference ConstCellIterator::cellOffset()
{
    return *(reinterpret_cast<celloffset_value_type*>(m_pointers[0]));
}

//------------------------------------------------------------------------------

size_t ConstCellIterator::nbPoints() const
{
    const auto currentOffset = *(reinterpret_cast<celloffset_value_type*>(m_pointers[0]));
    std::uint64_t nextOffset;
    if (m_idx < m_numberOfElements -1)
    {
        nextOffset = *(reinterpret_cast<std::uint64_t*>(m_pointers[0])+1);
    }
    else
    {
        nextOffset = m_cellDataSize-1;
    }
    return nextOffset - currentOffset;
}

//------------------------------------------------------------------------------

} // namespace iterator
} // namespace fwData
