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

#pragma once

#include <fwData/Mesh.hpp>

#include <iterator>

namespace fwData
{

namespace iterator
{

template<bool isConst>
PointIteratorBase<isConst>::PointIteratorBase()
{

}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>::~PointIteratorBase()
{
    m_locks.clear();
}

//------------------------------------------------------------------------------

template<bool isConst>
bool PointIteratorBase<isConst>::operator==(const PointIteratorBase& other) const
{
    return m_idx == other.m_idx;
}
//------------------------------------------------------------------------------

template<bool isConst>
bool PointIteratorBase<isConst>::operator!=(const PointIteratorBase& other) const
{
    return m_idx != other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator++()
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

template<bool isConst>
PointIteratorBase<isConst> PointIteratorBase<isConst>::operator++(int)
{
    PointIteratorBase tmp(*this);
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

template<bool isConst>
PointIteratorBase<isConst> PointIteratorBase<isConst>::operator+(difference_type index)
{
    PointIteratorBase tmp(*this);
    tmp += index;
    return tmp;
}
//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator+=(difference_type index)
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

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator--()
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

template<bool isConst>
PointIteratorBase<isConst> PointIteratorBase<isConst>::operator--(int)
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    PointIteratorBase tmp(*this);
    --m_idx;
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] -= m_elementSizes[i];
    }
    return tmp;
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst> PointIteratorBase<isConst>::operator-(difference_type index)
{
    PointIteratorBase tmp(*this);
    tmp -= index;
    return tmp;
}
//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator-=(difference_type index)
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

template<bool isConst>
typename PointIteratorBase<isConst>::difference_type PointIteratorBase<isConst>::operator+(
    const PointIteratorBase& other) const
{
    return m_idx + other.m_idx;
}
//------------------------------------------------------------------------------

template<bool isConst>
typename PointIteratorBase<isConst>::difference_type PointIteratorBase<isConst>::operator-(
    const PointIteratorBase& other) const
{
    return m_idx - other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>::CellIteratorBase()
{

}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>::~CellIteratorBase()
{
    m_locks.clear();
}

//------------------------------------------------------------------------------

template<bool isConst>
bool CellIteratorBase<isConst>::operator==(const CellIteratorBase& other) const
{
    return m_idx == other.m_idx;
}
//------------------------------------------------------------------------------

template<bool isConst>
bool CellIteratorBase<isConst>::operator!=(const CellIteratorBase& other) const
{
    return m_idx != other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator++()
{
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] += m_elementSizes[i];
    }
    const cellData_value_type offset = *reinterpret_cast<cellData_value_type*>(m_pointers[0]);
    const difference_type newOffset  = static_cast<difference_type>(offset);
    m_cellDataPointer += newOffset - m_currentOffset;
    m_currentOffset    = newOffset;
    return *this;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst> CellIteratorBase<isConst>::operator++(int)
{
    CellIteratorBase tmp(*this);
    this->operator++();
    return tmp;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst> CellIteratorBase<isConst>::operator+(difference_type index)
{
    CellIteratorBase tmp(*this);
    tmp += index;
    return tmp;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator+=(difference_type index)
{
    m_idx += index;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] += index*m_elementSizes[i];
    }
    const cellData_value_type offset = *reinterpret_cast<cellData_value_type*>(m_pointers[0]);
    const difference_type newOffset  = static_cast<difference_type>(offset);
    m_cellDataPointer += newOffset - m_currentOffset;
    m_currentOffset    = newOffset;
    return *this;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] -= m_elementSizes[i];
    }
    const std::uint64_t offset = *reinterpret_cast<std::uint64_t*>(m_pointers[0]);
    difference_type newOffset  = static_cast<difference_type>(offset);
    m_cellDataPointer -= newOffset - m_currentOffset;
    m_currentOffset    = newOffset;
    return *this;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst> CellIteratorBase<isConst>::operator--(int)
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    CellIteratorBase tmp(*this);
    this->operator--();
    return tmp;
}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst> CellIteratorBase<isConst>::operator-(difference_type index)
{
    CellIteratorBase tmp(*this);
    tmp -= index;
    return tmp;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator-=(difference_type index)
{
    SLM_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                             << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );

    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    m_idx -= index;
    for(size_t i = 0; i < m_nbArrays; ++i)
    {
        m_pointers[i] -= index*m_elementSizes[i];
    }
    const std::uint64_t offset = *reinterpret_cast<std::uint64_t*>(m_pointers[0]);
    difference_type newOffset  = static_cast<difference_type>(offset);
    m_cellDataPointer -= newOffset - m_currentOffset;
    m_currentOffset    = newOffset;

    return *this;
}

//------------------------------------------------------------------------------

template<bool isConst>
typename CellIteratorBase<isConst>::difference_type CellIteratorBase<isConst>::operator+(
    const CellIteratorBase& other) const
{
    return m_idx + other.m_idx;
}
//------------------------------------------------------------------------------

template<bool isConst>
typename CellIteratorBase<isConst>::difference_type CellIteratorBase<isConst>::operator-(
    const CellIteratorBase& other) const
{
    return m_idx - other.m_idx;
}

//------------------------------------------------------------------------------

} // namespace iterator
} // namespace fwData
