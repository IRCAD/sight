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
    ++m_pointInfo.point;
    if (m_pointInfo.color)
    {
        ++m_pointInfo.color;
    }
    if (m_pointInfo.normal)
    {
        ++m_pointInfo.normal;
    }
    if (m_pointInfo.tex)
    {
        ++m_pointInfo.tex;
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
    ++m_pointInfo.point;
    if (m_pointInfo.color)
    {
        ++m_pointInfo.color;
    }
    if (m_pointInfo.normal)
    {
        ++m_pointInfo.normal;
    }
    if (m_pointInfo.tex)
    {
        ++m_pointInfo.tex;
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
    m_pointInfo.point += index;
    if (m_pointInfo.color)
    {
        m_pointInfo.color += index;
    }
    if (m_pointInfo.normal)
    {
        m_pointInfo.normal += index;
    }
    if (m_pointInfo.tex)
    {
        m_pointInfo.tex += index;
    }
    return *this;
}
//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    --m_pointInfo.point;
    if (m_pointInfo.color)
    {
        --m_pointInfo.color;
    }
    if (m_pointInfo.normal)
    {
        --m_pointInfo.normal;
    }
    if (m_pointInfo.tex)
    {
        --m_pointInfo.tex;
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
    --m_pointInfo.point;
    if (m_pointInfo.color)
    {
        --m_pointInfo.color;
    }
    if (m_pointInfo.normal)
    {
        --m_pointInfo.normal;
    }
    if (m_pointInfo.tex)
    {
        --m_pointInfo.tex;
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
    m_idx             -= index;
    m_pointInfo.point -= index;
    if (m_pointInfo.color)
    {
        m_pointInfo.color -= index;
    }
    if (m_pointInfo.normal)
    {
        m_pointInfo.normal -= index;
    }
    if (m_pointInfo.tex)
    {
        m_pointInfo.tex -= index;
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
typename PointIteratorBase<isConst>::PointInfo PointIteratorBase<isConst>::operator*()
{
    return m_pointInfo;
}

//------------------------------------------------------------------------------

template<bool isConst>
typename PointIteratorBase<isConst>::PointInfo* PointIteratorBase<isConst>::operator->()
{
    return &m_pointInfo;
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
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "[",
               m_idx <= m_numberOfElements );
    ++m_cellInfo.type;
    ++m_cellInfo.offset;

    std::uint64_t offset;
    if (m_idx < m_numberOfElements)
    {
        offset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset);
    }
    else
    {
        offset = m_cellDataSize;
    }

    const difference_type newOffset = static_cast<difference_type>(offset);
    m_cellInfo.pointIdx += newOffset - m_currentOffset;
    m_currentOffset      = newOffset;
    std::uint64_t nextOffset;
    if (m_idx < m_numberOfElements-1)
    {
        nextOffset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset+1);
    }
    else
    {
        nextOffset = m_cellDataSize;
    }

    m_cellInfo.nbPoints = nextOffset - offset;

    if (m_cellInfo.color)
    {
        ++m_cellInfo.color;
    }
    if (m_cellInfo.normal)
    {
        ++m_cellInfo.normal;
    }
    if (m_cellInfo.tex)
    {
        ++m_cellInfo.tex;
    }
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

    m_cellInfo.type   += index;
    m_cellInfo.offset += index;
    std::uint64_t offset;
    if (m_idx < m_numberOfElements)
    {
        offset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset);
    }
    else
    {
        offset = m_cellDataSize;
    }
    const difference_type newOffset = static_cast<difference_type>(offset);
    m_cellInfo.pointIdx += newOffset - m_currentOffset;
    m_currentOffset      = newOffset;
    std::uint64_t nextOffset;
    if (m_idx < m_numberOfElements-1)
    {
        nextOffset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset+1);
    }
    else
    {
        nextOffset = m_cellDataSize;
    }
    m_cellInfo.nbPoints = nextOffset - offset;

    if (m_cellInfo.color)
    {
        m_cellInfo.color += index;
    }
    if (m_cellInfo.normal)
    {
        m_cellInfo.normal += index;
    }
    if (m_cellInfo.tex)
    {
        m_cellInfo.tex += index;
    }

    return *this;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    --m_cellInfo.type;
    --m_cellInfo.offset;

    const cell_data_value_type offset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset);
    const difference_type newOffset   = static_cast<difference_type>(offset);
    m_cellInfo.nbPoints  = m_currentOffset - newOffset;
    m_cellInfo.pointIdx -= m_currentOffset - newOffset;
    m_currentOffset      = newOffset;

    if (m_cellInfo.color)
    {
        --m_cellInfo.color;
    }
    if (m_cellInfo.normal)
    {
        --m_cellInfo.normal;
    }
    if (m_cellInfo.tex)
    {
        --m_cellInfo.tex;
    }
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

    m_cellInfo.type   -= index;
    m_cellInfo.offset -= index;
    const cell_data_value_type offset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset);
    const difference_type newOffset   = static_cast<difference_type>(offset);
    m_cellInfo.pointIdx -= m_currentOffset - newOffset;
    m_currentOffset      = newOffset;

    std::uint64_t nextOffset;
    if (m_idx < m_numberOfElements-1)
    {
        nextOffset = *reinterpret_cast<cell_data_value_type*>(m_cellInfo.offset+1);
    }
    else
    {
        nextOffset = m_cellDataSize;
    }
    m_cellInfo.nbPoints = nextOffset - offset;

    if (m_cellInfo.color)
    {
        m_cellInfo.color -= index;
    }
    if (m_cellInfo.normal)
    {
        m_cellInfo.normal -= index;
    }
    if (m_cellInfo.tex)
    {
        m_cellInfo.tex -= index;
    }

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

template<bool isConst>
typename CellIteratorBase<isConst>::CellInfo CellIteratorBase<isConst>::operator*()
{
    return m_cellInfo;
}

//------------------------------------------------------------------------------

template<bool isConst>
typename CellIteratorBase<isConst>::CellInfo* CellIteratorBase<isConst>::operator->()
{
    return &m_cellInfo;
}

//------------------------------------------------------------------------------

template<bool isConst>
typename CellIteratorBase<isConst>::cell_data_value_type& CellIteratorBase<isConst>::operator[](size_t index)
{
    OSLM_ASSERT("Index out ou bounds, the cell contains only " << m_cellInfo.nbPoints << " points",
                index < m_cellInfo.nbPoints);
    return *(m_cellInfo.pointIdx+index);
}

//------------------------------------------------------------------------------

template<bool isConst>
size_t CellIteratorBase<isConst>::nbPoints() const
{
    return m_cellInfo.nbPoints;
}

//------------------------------------------------------------------------------

} // namespace iterator
} // namespace fwData
