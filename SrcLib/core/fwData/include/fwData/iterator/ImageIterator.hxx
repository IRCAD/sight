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

#include "fwData/Image.hpp"

namespace fwData
{
namespace iterator
{

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst>::ImageIteratorBase(ImageType image) :
    m_pointer(static_cast<pointer>(image->getBuffer())),
    m_lock(image->lock()),
    m_idx(0),
    m_elementSize(FORMAT::elementSize),
    m_numberOfElements(static_cast<difference_type>(image->getNumElements()))
{
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst>::ImageIteratorBase(const ImageIteratorBase<FORMAT, false>& other) :
    m_pointer(other.m_pointer),
    m_lock(other.m_lock),
    m_idx(other.m_idx),
    m_elementSize(other.m_elementSize),
    m_numberOfElements(other.m_numberOfElements)
{
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst>::~ImageIteratorBase()
{
    m_lock.reset();
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
ImageIteratorBase<FORMAT, isConst>& ImageIteratorBase<FORMAT, isConst>::operator=(const ImageIteratorBase& other)
{
    if (this != &other)
    {
        m_pointer          = other.m_pointer;
        m_lock             = other.m_lock;
        m_idx              = other.m_idx;
        m_elementSize      = other.m_elementSize;
        m_numberOfElements = other.m_numberOfElements;
    }
    return *this;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline bool ImageIteratorBase<FORMAT, isConst>::operator==(const ImageIteratorBase& other) const
{
    return m_pointer == other.m_pointer;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline bool ImageIteratorBase<FORMAT, isConst>::operator!=(const ImageIteratorBase& other) const
{
    return m_pointer != other.m_pointer;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline typename ImageIteratorBase<FORMAT, isConst>::reference ImageIteratorBase<FORMAT, isConst>::operator*()
{
    return *(reinterpret_cast<ImageIteratorBase<FORMAT, isConst>::value_type*>(m_pointer));
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline typename ImageIteratorBase<FORMAT, isConst>::value_type* ImageIteratorBase<FORMAT, isConst>::operator->()
{
    return reinterpret_cast<ImageIteratorBase<FORMAT, isConst>::value_type*>(m_pointer);
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst>& ImageIteratorBase<FORMAT, isConst>::operator++()
{
    m_idx += m_elementSize;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += m_elementSize;
    return *this;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst> ImageIteratorBase<FORMAT, isConst>::operator++(int)
{
    ImageIteratorBase tmp(*this);
    m_idx += m_elementSize;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += m_elementSize;
    return tmp;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst> ImageIteratorBase<FORMAT, isConst>::operator+(difference_type index)
{
    ImageIteratorBase tmp(*this);
    tmp += index;
    return tmp;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst>& ImageIteratorBase<FORMAT, isConst>::operator+=(difference_type index)
{
    m_idx += index*m_elementSize;
    SLM_ASSERT("Array out of bounds: index " << m_idx << " is not in [0-"<<m_numberOfElements << "]",
               m_idx <= m_numberOfElements );
    m_pointer += index*m_elementSize;
    return *this;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst>& ImageIteratorBase<FORMAT, isConst>::operator--()
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    m_idx     -= m_elementSize;
    m_pointer -= m_elementSize;
    return *this;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst> ImageIteratorBase<FORMAT, isConst>::operator--(int)
{
    SLM_ASSERT("Array out of bounds: index -1 is not in [0-"<<m_numberOfElements << "]", m_idx > 0 );
    m_idx -= m_elementSize;
    ImageIteratorBase tmp(*this);
    m_pointer -= m_elementSize;
    return tmp;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst> ImageIteratorBase<FORMAT, isConst>::operator-(difference_type index)
{
    ImageIteratorBase tmp(*this);
    tmp -= index;
    return tmp;
}

//------------------------------------------------------------------------------

template <class FORMAT, bool isConst>
inline ImageIteratorBase<FORMAT, isConst>& ImageIteratorBase<FORMAT, isConst>::operator-=(difference_type index)
{
    SLM_ASSERT("Array out of bounds: index " << (static_cast<std::int64_t>(m_idx) - static_cast<std::int64_t>(index))
                                             << " is not in [0-"<<m_numberOfElements << "]", m_idx >= index );
    m_idx     -= index*m_elementSize;
    m_pointer -= index*m_elementSize;
    return *this;
}

//-----------------------------------------------------------------------------

template <class FORMAT, bool isConst>
typename ImageIteratorBase<FORMAT, isConst>::difference_type ImageIteratorBase<FORMAT, isConst>::operator+(
    const ImageIteratorBase& other) const
{
    return (m_idx + other.m_idx) / m_elementSize;
}

//-----------------------------------------------------------------------------

template <class FORMAT, bool isConst>
typename ImageIteratorBase<FORMAT, isConst>::difference_type ImageIteratorBase<FORMAT, isConst>::operator-(
    const ImageIteratorBase& other) const
{
    return (m_idx - other.m_idx) / m_elementSize;
}

//------------------------------------------------------------------------------

} // namespace iterator
} // namespace fwData
