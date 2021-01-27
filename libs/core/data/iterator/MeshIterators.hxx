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

#pragma once

#include <data/Mesh.hpp>

#include <iterator>

namespace sight::data
{

namespace iterator
{

//------------------------------------------------------------------------------

template<bool isConst>
PointInfoBase<isConst>& PointInfoBase<isConst>::operator=(
    const PointInfoBase<false>& other)
{
    static_assert(isConst == false, "Cannot affect values on a read-only struct.");
    FW_RAISE_IF("Point is not defined", nullptr == point);
    point->x = other.point->x;
    point->y = other.point->y;
    point->z = other.point->z;

    if(normal && other.normal)
    {
        normal->nx = other.normal->nx;
        normal->ny = other.normal->ny;
        normal->nz = other.normal->nz;
    }
    if(rgb && other.rgb)
    {
        rgb->r = other.rgb->r;
        rgb->g = other.rgb->g;
        rgb->b = other.rgb->b;
    }
    if(rgba && other.rgba)
    {
        rgba->r = other.rgba->r;
        rgba->g = other.rgba->g;
        rgba->b = other.rgba->b;
        rgba->a = other.rgba->a;
    }
    if (tex && other.tex)
    {
        tex->u = other.tex->u;
        tex->v = other.tex->v;
    }
    return *this;
}

//------------------------------------------------------------------------------

template<bool isConst>
PointInfoBase<isConst>& PointInfoBase<isConst>::operator=(
    const PointInfoBase<true>& other)
{
    static_assert(isConst == false, "Cannot affect values on a read-only struct.");
    FW_RAISE_IF("Point is not defined", nullptr == point);
    point->x = other.point->x;
    point->y = other.point->y;
    point->z = other.point->z;

    if(normal && other.normal)
    {
        normal->nx = other.normal->nx;
        normal->ny = other.normal->ny;
        normal->nz = other.normal->nz;
    }
    if(rgb && other.rgb)
    {
        rgb->r = other.rgb->r;
        rgb->g = other.rgb->g;
        rgb->b = other.rgb->b;
    }
    if(rgba && other.rgba)
    {
        rgba->r = other.rgba->r;
        rgba->g = other.rgba->g;
        rgba->b = other.rgba->b;
        rgba->a = other.rgba->a;
    }
    if (tex && other.tex)
    {
        tex->u = other.tex->u;
        tex->v = other.tex->v;
    }
    return *this;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool PointInfoBase<isConst>::operator==(const PointInfoBase& other)
{
    bool isEqual = true;
    isEqual &= point->x == other.point->x;
    isEqual &= point->y == other.point->y;
    isEqual &= point->z == other.point->z;

    isEqual &= (normal == nullptr && other.normal == nullptr) ||  (normal != nullptr && other.normal != nullptr);
    if(normal && other.normal)
    {
        isEqual &= normal->nx == other.normal->nx;
        isEqual &= normal->ny == other.normal->ny;
        isEqual &= normal->nz == other.normal->nz;
    }

    isEqual &= (rgb == nullptr && other.rgb == nullptr) ||  (rgb != nullptr && other.rgb != nullptr);
    if(rgb && other.rgb)
    {
        isEqual &= rgb->r == other.rgb->r;
        isEqual &= rgb->g == other.rgb->g;
        isEqual &= rgb->b == other.rgb->b;
    }
    isEqual &= (rgba == nullptr && other.rgba == nullptr) ||  (rgba != nullptr && other.rgba != nullptr);
    if(rgba && other.rgba)
    {
        isEqual &= rgba->r == other.rgba->r;
        isEqual &= rgba->g == other.rgba->g;
        isEqual &= rgba->b == other.rgba->b;
        isEqual &= rgba->a == other.rgba->a;
    }
    isEqual &= (tex == nullptr && other.tex == nullptr) ||  (tex != nullptr && other.tex != nullptr);
    if (tex && other.tex)
    {
        isEqual &= tex->u == other.tex->u;
        isEqual &= tex->v == other.tex->v;
    }
    return isEqual;
}

//------------------------------------------------------------------------------

template<bool isConst>
CellInfoBase<isConst>& CellInfoBase<isConst>::operator=(
    const CellInfoBase<false>& other)
{
    static_assert(isConst == false, "Cannot affect values on a read-only struct.");
    FW_RAISE_IF("Cell is not defined", nullptr == type || nullptr == offset || nullptr == pointIdx);
    *type    = *other.type;
    *offset  = *other.offset;
    nbPoints = other.nbPoints;

    // Check if we are still in the bound of offset array.
    if(idx + 1 < numberOfElements)
    {
        *(offset+1) = *offset + nbPoints;
    }

    std::copy(other.pointIdx, other.pointIdx+nbPoints, pointIdx);

    if(normal && other.normal)
    {
        normal->nx = other.normal->nx;
        normal->ny = other.normal->ny;
        normal->nz = other.normal->nz;
    }
    if(rgb && other.rgb)
    {
        rgb->r = other.rgb->r;
        rgb->g = other.rgb->g;
        rgb->b = other.rgb->b;
    }
    if(rgba && other.rgba)
    {
        rgba->r = other.rgba->r;
        rgba->g = other.rgba->g;
        rgba->b = other.rgba->b;
        rgba->a = other.rgba->a;
    }
    if (tex && other.tex)
    {
        tex->u = other.tex->u;
        tex->v = other.tex->v;
    }
    return *this;
}

//------------------------------------------------------------------------------

template<bool isConst>
CellInfoBase<isConst>& CellInfoBase<isConst>::operator=(
    const CellInfoBase<true>& other)
{
    static_assert(isConst == false, "Cannot affect values on a read-only struct.");
    FW_RAISE_IF("Cell is not defined", nullptr == type || nullptr == offset || nullptr == pointIdx);
    *type    = *other.type;
    *offset  = *other.offset;
    nbPoints = other.nbPoints;

    // Check if we are still in the bound of offset array.
    if(idx + 1 < numberOfElements)
    {
        *(offset+1) = *offset + nbPoints;
    }

    std::copy(other.pointIdx, other.pointIdx+nbPoints, pointIdx);

    if(normal && other.normal)
    {
        normal->nx = other.normal->nx;
        normal->ny = other.normal->ny;
        normal->nz = other.normal->nz;
    }
    if(rgb && other.rgb)
    {
        rgb->r = other.rgb->r;
        rgb->g = other.rgb->g;
        rgb->b = other.rgb->b;
    }
    if(rgba && other.rgba)
    {
        rgba->r = other.rgba->r;
        rgba->g = other.rgba->g;
        rgba->b = other.rgba->b;
        rgba->a = other.rgba->a;
    }
    if (tex && other.tex)
    {
        tex->u = other.tex->u;
        tex->v = other.tex->v;
    }
    return *this;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool CellInfoBase<isConst>::operator==(const CellInfoBase& other)
{
    bool isEqual = true;
    isEqual &= *type == *other.type;
    isEqual &= *offset == *other.offset;
    isEqual &= nbPoints == other.nbPoints;

    isEqual &= std::equal(other.pointIdx, other.pointIdx+nbPoints, pointIdx);

    isEqual &= (normal == nullptr && other.normal == nullptr) ||  (normal != nullptr && other.normal != nullptr);
    if(normal && other.normal)
    {
        isEqual &= normal->nx == other.normal->nx;
        isEqual &= normal->ny == other.normal->ny;
        isEqual &= normal->nz == other.normal->nz;
    }

    isEqual &= (rgb == nullptr && other.rgb == nullptr) ||  (rgb != nullptr && other.rgb != nullptr);
    if(rgb && other.rgb)
    {
        isEqual &= rgb->r == other.rgb->r;
        isEqual &= rgb->g == other.rgb->g;
        isEqual &= rgb->b == other.rgb->b;
    }
    isEqual &= (rgba == nullptr && other.rgba == nullptr) ||  (rgba != nullptr && other.rgba != nullptr);
    if(rgba && other.rgba)
    {
        isEqual &= rgba->r == other.rgba->r;
        isEqual &= rgba->g == other.rgba->g;
        isEqual &= rgba->b == other.rgba->b;
        isEqual &= rgba->a == other.rgba->a;
    }
    isEqual &= (tex == nullptr && other.tex == nullptr) ||  (tex != nullptr && other.tex != nullptr);
    if (tex && other.tex)
    {
        isEqual &= tex->u == other.tex->u;
        isEqual &= tex->v == other.tex->v;
    }
    return isEqual;
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>::PointIteratorBase()
{
    m_pointInfo = std::make_unique< PointInfo >();
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>::~PointIteratorBase()
{
    m_pointInfo.reset();
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>::PointIteratorBase(const PointIteratorBase& other)
{
    m_pointInfo         = std::make_unique< PointInfo >();
    m_pointInfo->point  = other.m_pointInfo->point;
    m_pointInfo->rgb    = other.m_pointInfo->rgb;
    m_pointInfo->rgba   = other.m_pointInfo->rgba;
    m_pointInfo->normal = other.m_pointInfo->normal;
    m_pointInfo->tex    = other.m_pointInfo->tex;
    m_idx               = other.m_idx;
    m_numberOfElements  = other.m_numberOfElements;
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator=(const PointIteratorBase& other)
{
    if (this != &other)
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
bool PointIteratorBase<isConst>::operator<(const PointIteratorBase& other) const
{
    return m_idx < other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool PointIteratorBase<isConst>::operator<=(const PointIteratorBase& other) const
{
    return m_idx <= other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool PointIteratorBase<isConst>::operator>(const PointIteratorBase& other) const
{
    return m_idx > other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool PointIteratorBase<isConst>::operator>=(const PointIteratorBase& other) const
{
    return m_idx >= other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator++()
{
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    ++m_pointInfo->point;
    if (m_pointInfo->rgba)
    {
        ++m_pointInfo->rgba;
    }
    if (m_pointInfo->rgb)
    {
        ++m_pointInfo->rgb;
    }
    if (m_pointInfo->normal)
    {
        ++m_pointInfo->normal;
    }
    if (m_pointInfo->tex)
    {
        ++m_pointInfo->tex;
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
    ++m_pointInfo->point;
    if (m_pointInfo->rgba)
    {
        ++m_pointInfo->rgba;
    }
    if (m_pointInfo->rgb)
    {
        ++m_pointInfo->rgb;
    }
    if (m_pointInfo->rgb)
    {
        ++m_pointInfo->rgb;
    }
    if (m_pointInfo->normal)
    {
        ++m_pointInfo->normal;
    }
    if (m_pointInfo->tex)
    {
        ++m_pointInfo->tex;
    }
    return tmp;
}
//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst> PointIteratorBase<isConst>::operator+(difference_type index) const
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
    m_pointInfo->point += index;
    if (m_pointInfo->rgba)
    {
        m_pointInfo->rgba += index;
    }
    if (m_pointInfo->rgb)
    {
        m_pointInfo->rgb += index;
    }
    if (m_pointInfo->rgb)
    {
        m_pointInfo->rgb += index;
    }
    if (m_pointInfo->normal)
    {
        m_pointInfo->normal += index;
    }
    if (m_pointInfo->tex)
    {
        m_pointInfo->tex += index;
    }
    return *this;
}
//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst>& PointIteratorBase<isConst>::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    --m_pointInfo->point;
    if (m_pointInfo->rgba)
    {
        --m_pointInfo->rgba;
    }
    if (m_pointInfo->rgb)
    {
        --m_pointInfo->rgb;
    }
    if (m_pointInfo->rgb)
    {
        --m_pointInfo->rgb;
    }
    if (m_pointInfo->normal)
    {
        --m_pointInfo->normal;
    }
    if (m_pointInfo->tex)
    {
        --m_pointInfo->tex;
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
    --m_pointInfo->point;
    if (m_pointInfo->rgba)
    {
        --m_pointInfo->rgba;
    }
    if (m_pointInfo->rgb)
    {
        --m_pointInfo->rgb;
    }
    if (m_pointInfo->rgb)
    {
        --m_pointInfo->rgb;
    }
    if (m_pointInfo->normal)
    {
        --m_pointInfo->normal;
    }
    if (m_pointInfo->tex)
    {
        --m_pointInfo->tex;
    }
    return tmp;
}

//------------------------------------------------------------------------------

template<bool isConst>
PointIteratorBase<isConst> PointIteratorBase<isConst>::operator-(difference_type index) const
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
    m_idx              -= index;
    m_pointInfo->point -= index;
    if (m_pointInfo->rgba)
    {
        m_pointInfo->rgba -= index;
    }
    if (m_pointInfo->rgb)
    {
        m_pointInfo->rgb -= index;
    }
    if (m_pointInfo->normal)
    {
        m_pointInfo->normal -= index;
    }
    if (m_pointInfo->tex)
    {
        m_pointInfo->tex -= index;
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
typename PointIteratorBase<isConst>::reference PointIteratorBase<isConst>::operator*() const
{
    return *m_pointInfo;
}

//------------------------------------------------------------------------------

template<bool isConst>
typename PointIteratorBase<isConst>::pointer PointIteratorBase<isConst>::operator->() const
{
    return m_pointInfo.get();
}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>::CellIteratorBase()
{
    m_cellInfo = std::make_unique< CellInfo >();
}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>::~CellIteratorBase()
{
    m_cellInfo.reset();
}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>::CellIteratorBase(const CellIteratorBase& other)
{
    m_cellInfo           = std::make_unique< CellInfo >();
    m_idx                = other.m_idx;
    m_numberOfElements   = other.m_numberOfElements;
    m_currentOffset      = other.m_currentOffset;
    m_cellDataSize       = other.m_cellDataSize;
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

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator=(const CellIteratorBase& other)
{
    if (this != &other)
    {
        m_idx                = other.m_idx;
        m_numberOfElements   = other.m_numberOfElements;
        m_currentOffset      = other.m_currentOffset;
        m_cellDataSize       = other.m_cellDataSize;
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
bool CellIteratorBase<isConst>::operator<(const CellIteratorBase& other) const
{
    return m_idx < other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool CellIteratorBase<isConst>::operator<=(const CellIteratorBase& other) const
{
    return m_idx <= other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool CellIteratorBase<isConst>::operator>(const CellIteratorBase& other) const
{
    return m_idx > other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
bool CellIteratorBase<isConst>::operator>=(const CellIteratorBase& other) const
{
    return m_idx >= other.m_idx;
}

//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator++()
{
    ++m_idx;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "[",
               m_idx <= m_numberOfElements );
    ++m_cellInfo->type;
    ++m_cellInfo->offset;

    ++m_cellInfo->idx;

    Size offset;
    if (m_idx < m_numberOfElements)
    {
        offset = *reinterpret_cast<typename CellInfo::cell_data_value_type*>(m_cellInfo->offset);
    }
    else
    {
        offset = m_cellDataSize;
    }

    const difference_type newOffset = static_cast<difference_type>(offset);
    m_cellInfo->pointIdx += newOffset - m_currentOffset;
    m_currentOffset       = newOffset;
    Size nextOffset;
    if (m_idx < m_numberOfElements-1)
    {
        nextOffset = *reinterpret_cast<typename CellInfo::cell_data_value_type*>(m_cellInfo->offset+1);
    }
    else
    {
        nextOffset = m_cellDataSize;
    }

    m_cellInfo->nbPoints = nextOffset - offset;

    if (m_cellInfo->rgba)
    {
        ++m_cellInfo->rgba;
    }
    if (m_cellInfo->rgb)
    {
        ++m_cellInfo->rgb;
    }
    if (m_cellInfo->normal)
    {
        ++m_cellInfo->normal;
    }
    if (m_cellInfo->tex)
    {
        ++m_cellInfo->tex;
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
CellIteratorBase<isConst> CellIteratorBase<isConst>::operator+(difference_type index) const
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

    m_cellInfo->type   += index;
    m_cellInfo->offset += index;
    m_cellInfo->idx    += index;

    Size offset;
    if (m_idx < m_numberOfElements)
    {
        offset = *reinterpret_cast<typename CellInfo::cell_data_value_type*>(m_cellInfo->offset);
    }
    else
    {
        offset = m_cellDataSize;
    }
    const difference_type newOffset = static_cast<difference_type>(offset);
    m_cellInfo->pointIdx += newOffset - m_currentOffset;
    m_currentOffset       = newOffset;
    Size nextOffset;
    if (m_idx < m_numberOfElements-1)
    {
        nextOffset = *reinterpret_cast<typename CellInfo::cell_data_value_type*>(m_cellInfo->offset+1);
    }
    else
    {
        nextOffset = m_cellDataSize;
    }
    m_cellInfo->nbPoints = nextOffset - offset;

    if (m_cellInfo->rgba)
    {
        m_cellInfo->rgba += index;
    }
    if (m_cellInfo->rgb)
    {
        m_cellInfo->rgb += index;
    }
    if (m_cellInfo->normal)
    {
        m_cellInfo->normal += index;
    }
    if (m_cellInfo->tex)
    {
        m_cellInfo->tex += index;
    }

    return *this;
}
//------------------------------------------------------------------------------

template<bool isConst>
CellIteratorBase<isConst>& CellIteratorBase<isConst>::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    --m_idx;
    --m_cellInfo->type;
    --m_cellInfo->offset;
    --m_cellInfo->idx;

    const typename CellInfo::cell_data_value_type offset =
        *reinterpret_cast<typename CellInfo::cell_data_value_type*>(m_cellInfo->offset);
    const difference_type newOffset = static_cast<difference_type>(offset);
    m_cellInfo->pointIdx -= m_currentOffset - newOffset;
    m_cellInfo->nbPoints  = static_cast<Size>(m_currentOffset - newOffset);
    m_currentOffset       = newOffset;

    if (m_cellInfo->rgba)
    {
        --m_cellInfo->rgba;
    }
    if (m_cellInfo->rgb)
    {
        --m_cellInfo->rgb;
    }
    if (m_cellInfo->normal)
    {
        --m_cellInfo->normal;
    }
    if (m_cellInfo->tex)
    {
        --m_cellInfo->tex;
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
CellIteratorBase<isConst> CellIteratorBase<isConst>::operator-(difference_type index) const
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

    m_cellInfo->type   -= index;
    m_cellInfo->offset -= index;
    m_cellInfo->idx    -= index;

    const typename CellInfo::cell_data_value_type offset =
        *reinterpret_cast<typename CellInfo::cell_data_value_type*>(m_cellInfo->offset);
    const difference_type newOffset = static_cast<difference_type>(offset);
    m_cellInfo->pointIdx -= m_currentOffset - newOffset;
    m_currentOffset       = newOffset;

    Size nextOffset;
    if (m_idx < m_numberOfElements-1)
    {
        nextOffset = *reinterpret_cast<typename CellInfo::cell_data_value_type*>(m_cellInfo->offset+1);
    }
    else
    {
        nextOffset = m_cellDataSize;
    }
    m_cellInfo->nbPoints = nextOffset - offset;

    if (m_cellInfo->rgba)
    {
        m_cellInfo->rgba -= index;
    }
    if (m_cellInfo->rgb)
    {
        m_cellInfo->rgb -= index;
    }
    if (m_cellInfo->normal)
    {
        m_cellInfo->normal -= index;
    }
    if (m_cellInfo->tex)
    {
        m_cellInfo->tex -= index;
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
typename CellIteratorBase<isConst>::reference CellIteratorBase<isConst>::operator*() const
{
    return *m_cellInfo;
}

//------------------------------------------------------------------------------

template<bool isConst>
typename CellIteratorBase<isConst>::pointer CellIteratorBase<isConst>::operator->() const
{
    return m_cellInfo.get();
}

//------------------------------------------------------------------------------

template<bool isConst>
typename CellIteratorBase<isConst>::CellInfo::cell_data_value_type& CellIteratorBase<isConst>::operator[](size_t index)
{
    SLM_ASSERT("Index out ou bounds, the cell contains only " << m_cellInfo->nbPoints << " points",
               index < m_cellInfo->nbPoints);
    return *(m_cellInfo->pointIdx+index);
}

//------------------------------------------------------------------------------

template<bool isConst>
Size CellIteratorBase<isConst>::nbPoints() const
{
    return m_cellInfo->nbPoints;
}

//------------------------------------------------------------------------------

} // namespace iterator
} // namespace sight::data
