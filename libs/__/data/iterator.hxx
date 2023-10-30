/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#pragma once

namespace sight::data
{

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE array_iterator<FORMAT>::array_iterator(pointer_t _begin) :
    m_current(_begin)
{
#ifdef SIGHT_DEBUG_ITERATOR
    m_begin = m_current;
    m_end   = m_current + static_cast<difference_type>(image->size_in_bytes());
#endif
}

//------------------------------------------------------------------------------

template<class FORMAT>
template<bool isConst, typename>
constexpr FINLINE array_iterator<FORMAT>::array_iterator(
    const array_iterator<std::remove_const_t<FORMAT> >& _other
) :
    m_current(_other.m_current)
{
#ifdef SIGHT_DEBUG_ITERATOR
    m_begin = other.m_begin;
    m_end   = other.m_end;
#endif
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE bool array_iterator<FORMAT>::operator==(const array_iterator& _other) const noexcept
{
    return m_current == _other.m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE bool array_iterator<FORMAT>::operator!=(const array_iterator& _other) const noexcept
{
    return m_current != _other.m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE typename array_iterator<FORMAT>::reference array_iterator<FORMAT>::operator*() const
{
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator needs to be initialized", m_current);
#endif
    return *m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE typename array_iterator<FORMAT>::pointer_t array_iterator<FORMAT>::operator->() const noexcept
{
    return m_current;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE array_iterator<FORMAT>& array_iterator<FORMAT>::operator++()
{
    ++m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_current <= m_end);
#endif
    return *this;
}

//------------------------------------------------------------------------------

template<class FORMAT>
// NOLINTNEXTLINE(readability-const-return-type)
inline FINLINE const array_iterator<FORMAT> array_iterator<FORMAT>::operator++(int)
{
    array_iterator tmp(*this);
    ++m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_current <= m_end);
#endif
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE array_iterator<FORMAT> array_iterator<FORMAT>::operator+(difference_type _index) const
{
    array_iterator tmp(*this);
    tmp += _index;
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE array_iterator<FORMAT>& array_iterator<FORMAT>::operator+=(difference_type _index)
{
    m_current += _index;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_current <= m_end);
#endif
    return *this;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE array_iterator<FORMAT>& array_iterator<FORMAT>::operator--()
{
    --m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_begin <= m_current);
#endif
    return *this;
}

//------------------------------------------------------------------------------

template<class FORMAT>
// NOLINTNEXTLINE(readability-const-return-type)
inline FINLINE const array_iterator<FORMAT> array_iterator<FORMAT>::operator--(int)
{
    array_iterator tmp(*this);
    --m_current;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_begin <= m_current);
#endif
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE array_iterator<FORMAT> array_iterator<FORMAT>::operator-(difference_type _index) const
{
    array_iterator tmp(*this);
    tmp -= _index;
    return tmp;
}

//------------------------------------------------------------------------------

template<class FORMAT>
inline FINLINE array_iterator<FORMAT>& array_iterator<FORMAT>::operator-=(difference_type _index)
{
    m_current -= _index;
#ifdef SIGHT_DEBUG_ITERATOR
    SIGHT_ASSERT("Iterator out of bounds ", m_begin <= m_current);
#endif
    return *this;
}

//-----------------------------------------------------------------------------

template<class FORMAT>
constexpr FINLINE typename array_iterator<FORMAT>::difference_type array_iterator<FORMAT>::operator-(
    const array_iterator& _other
) const noexcept
{
    return m_current - _other.m_current;
}

//------------------------------------------------------------------------------

} // namespace sight::data
