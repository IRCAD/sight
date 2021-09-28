/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "core/memory/BufferObject.hpp"
#include <iterator>

namespace sight::data
{

class Image;

namespace iterator
{

/// Used to iterate through a RGB image of type 'uint8'
struct RGB
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

/// Used to iterate through a RGBA image of type 'uint8'
struct RGBA
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

/// Used to iterate through a BGR image of type 'uint8'
struct BGR
{
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
};

/// Used to iterate through a BGRA image of type 'uint8'
struct BGRA
{
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;
};

/// Used to iterate through a RGBA image of type 'uint8'
struct RGBA16
{
    std::uint16_t r;
    std::uint16_t g;
    std::uint16_t b;
    std::uint16_t a;
};

/**
 * @brief Iterator on image pixel buffer
 *
 * By default, this does not check out of bounds conditions. However it is possible to enable them temporarily
 * by defining the definition SIGHT_DEBUG_ITERATOR before including this file. Also, you should consider to disable
 * the PCH in the current target to avoid skipping this definition.
 *
 * @tparam FORMAT format used to iterate through the buffer, should be one of the formats defined in IterationBase
 *
 * Example:
 * @code{.cpp}
    data::Image::sptr img = data::Image::New();
    img->resize(1920, 1080, 0, core::tools::Type::s_UINT8, data::Image::PixelFormat::RGBA);
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

template<class FORMAT>
class ImageIteratorBase
{
public:

    /**
     * For ConstIterator:   define input to be a const Image*
     * For Iterator: define input to be a Image*
     */
    typedef typename std::conditional_t<std::is_const_v<FORMAT>, const Image*, Image*> ImageType;

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */

    using pointer    = FORMAT*;
    using value_type = FORMAT;
    using reference  = FORMAT&;

    /// Define difference type
    typedef std::ptrdiff_t difference_type;

    /// define the category of the iterator.
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    /// empty Constructor
    ImageIteratorBase() = default;
    constexpr ImageIteratorBase(ImageType image);
    ImageIteratorBase(const ImageIteratorBase<FORMAT>& other) = default;
    ImageIteratorBase(ImageIteratorBase<FORMAT>&& other)      = default;
    ~ImageIteratorBase()                                      = default;

    template<bool isConst = std::is_const_v<FORMAT>, typename = typename std::enable_if_t<isConst> >
    constexpr ImageIteratorBase(const ImageIteratorBase<std::remove_const_t<FORMAT> >& other);

    ImageIteratorBase& operator=(const ImageIteratorBase& other) = default;
    ImageIteratorBase& operator=(ImageIteratorBase&& other)      = default;

    /// Comparison operators
    constexpr bool operator==(const ImageIteratorBase& other) const noexcept;
    constexpr bool operator!=(const ImageIteratorBase& other) const noexcept;

    /// Increment/Decrement operators
    ImageIteratorBase& operator++();
    ImageIteratorBase operator++(int);
    constexpr ImageIteratorBase operator+(difference_type index) const;
    ImageIteratorBase& operator+=(difference_type index);
    ImageIteratorBase& operator--();
    ImageIteratorBase operator--(int);
    constexpr ImageIteratorBase operator-(difference_type index) const;
    ImageIteratorBase& operator-=(difference_type index);

    constexpr difference_type operator-(const ImageIteratorBase& other) const noexcept;

    /// Value access operators
    reference operator*() const;

    /// Value access operators
    constexpr pointer operator->() const noexcept;

protected:

    /// allow to create a ConstIterator from an Iterator
    friend class ImageIteratorBase<const FORMAT>;

    pointer m_current {nullptr};
#ifdef SIGHT_DEBUG_ITERATOR
    pointer m_begin {nullptr};
    pointer m_end {nullptr};
#endif
};

} // namespace iterator

} // namespace sight::data

#include "data/iterator/ImageIterator.hxx"
