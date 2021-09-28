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

#include "data/Image.hpp"

namespace sight::data
{

namespace iterator
{

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE ImageIteratorBase<FORMAT>::ImageIteratorBase(ImageType image) :
    m_current(static_cast<pointer>(image->getBuffer()))
{
#ifdef SIGHT_DEBUG_ITERATOR
    m_begin = m_current;
    m_end   = m_current + static_cast<difference_type>(image->getSizeInBytes());
#endif
}

//------------------------------------------------------------------------------

template<class FORMAT>
template<bool isConst, typename>
constexpr FINLINE ImageIteratorBase<FORMAT>::ImageIteratorBase(
    const ImageIteratorBase<std::remove_const_t<FORMAT> >& other
) :
    m_current(other.m_current)
{
#ifdef SIGHT_DEBUG_ITERATOR
    m_begin = other.m_begin;
    m_end   = other.m_end;
#endif
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE bool ImageIteratorBase<FORMAT>::operator==(const ImageIteratorBase& other) const noexcept
{
    return m_current == other.m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE bool ImageIteratorBase<FORMAT>::operator!=(const ImageIteratorBase& other) const noexcept
{
    return m_current != other.m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE typename ImageIteratorBase<FORMAT>::reference ImageIteratorBase<FORMAT>::operator*() const
{
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator needs to be initialized", m_current);
#endif
    return *m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE typename ImageIteratorBase<FORMAT>::pointer ImageIteratorBase<FORMAT>::operator->() const noexcept
{
    return m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE ImageIteratorBase<FORMAT>& ImageIteratorBase<FORMAT>::operator++()
{
    ++m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_current <= m_end);
#endif
    return *this;
}

//------------------------------------------------------------------------------

template<class FORMAT>
FINLINE ImageIteratorBase<FORMAT> ImageIteratorBase<FORMAT>::operator++(int)
{
    ImageIteratorBase tmp(*this);
    ++m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_current <= m_end);
#endif
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE ImageIteratorBase<FORMAT> ImageIteratorBase<FORMAT>::operator+(difference_type index) const
{
    ImageIteratorBase tmp(*this);
    tmp += index;
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE ImageIteratorBase<FORMAT>& ImageIteratorBase<FORMAT>::operator+=(difference_type index)
{
    m_current += index;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_current <= m_end);
#endif
    return *this;
}

//------------------------------------------------------------------------------

template<class FORMAT>
FINLINE ImageIteratorBase<FORMAT>& ImageIteratorBase<FORMAT>::operator--()
{
    --m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_begin <= m_current);
#endif
    return *this;
}

//------------------------------------------------------------------------------

template<class FORMAT>
FINLINE ImageIteratorBase<FORMAT> ImageIteratorBase<FORMAT>::operator--(int)
{
    ImageIteratorBase tmp(*this);
    --m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_begin <= m_current);
#endif
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE ImageIteratorBase<FORMAT> ImageIteratorBase<FORMAT>::operator-(difference_type index) const
{
    ImageIteratorBase tmp(*this);
    tmp -= index;
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
FINLINE ImageIteratorBase<FORMAT>& ImageIteratorBase<FORMAT>::operator-=(difference_type index)
{
    m_current -= index;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_begin <= m_current);
#endif
    return *this;
}

//-----------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE typename ImageIteratorBase<FORMAT>::difference_type ImageIteratorBase<FORMAT>::operator-(
    const ImageIteratorBase& other
) const noexcept
{
    return m_current - other.m_current;
}

//------------------------------------------------------------------------------

} // namespace iterator

} // namespace sight::data
