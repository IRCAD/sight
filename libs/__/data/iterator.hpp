/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

// cspell:ignore NOLINT

#pragma once

#include <data/iterator/types.hpp>

#include <iterator>

namespace sight::data
{

#ifdef SIGHT_DEBUG_ITERATOR
#define SIGHT_IF_DEBUG(x) , x
#else
#define SIGHT_IF_DEBUG(x)
#endif

/**
 * @brief Iterator on an array buffer
 *
 * By default, this does not check out of bounds conditions. However it is possible to enable them temporarily
 * by defining the definition SIGHT_DEBUG_ITERATOR before including this file. Also, you should consider to disable
 * the PCH in the current target to avoid skipping this definition.
 *
 * @tparam T format used to iterate through the buffer, should be one of the formats defined in IterationBase
 *
 * Example:
 * @code{.cpp}
    data::image::sptr img = std::make_shared<data::image>();
    img->resize(1920, 1080, 0, core::type::UINT8, data::image::PixelFormat::RGBA);
    auto iter    = img->begin<RGBA>();
    const auto iterEnd = img->end<RGBA>();

    for (; iter != iterEnd; ++iter)
    {
        iter->r = val1;
        iter->g = val2;
        iter->b = val2;
        iter->a = val4;
    }
   @endcode
 */

template<class T>
class array_iterator
{
public:

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */

    using pointer    = T*;
    using value_type = T;
    using reference  = T&;

    /// Define difference type
    typedef std::ptrdiff_t difference_type;

    /// define the category of the iterator.
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    array_iterator() = default;
    constexpr array_iterator(pointer begin);
    array_iterator(const array_iterator<T>& other) = default;
    array_iterator(array_iterator<T>&& other) noexcept = default;
    ~array_iterator() = default;

    template<bool isConst = std::is_const_v<T>, typename = typename std::enable_if_t<isConst> >
    constexpr array_iterator(const array_iterator<std::remove_const_t<T> >& other);

    array_iterator& operator=(const array_iterator& other) = default;
    array_iterator& operator=(array_iterator&& other) noexcept = default;

    /// Comparison operators
    constexpr bool operator==(const array_iterator& other) const noexcept;
    constexpr bool operator!=(const array_iterator& other) const noexcept;

    /// Increment/Decrement operators
    array_iterator& operator++();
    const array_iterator operator++(int); // NOLINT(readability-const-return-type)
    constexpr array_iterator operator+(difference_type index) const;
    array_iterator& operator+=(difference_type index);
    array_iterator& operator--();
    const array_iterator operator--(int); // NOLINT(readability-const-return-type)
    constexpr array_iterator operator-(difference_type index) const;
    array_iterator& operator-=(difference_type index);

    constexpr difference_type operator-(const array_iterator& other) const noexcept;

    /// Value access operators
    reference operator*() const;

    /// Value access operators
    constexpr pointer operator->() const noexcept;

protected:

    /// allow to create a ConstIterator from an Iterator
    friend class array_iterator<const T>;

    pointer m_current {nullptr};
#ifdef SIGHT_DEBUG_ITERATOR
    pointer m_begin {nullptr};
    pointer m_end {nullptr};
#endif
};

template<typename T>
/// Image const iterator
using const_array_iterator = array_iterator<const T>;

} // namespace sight::data

#include "data/iterator.hxx"
