/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
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

#include "fwMemory/BufferObject.hpp"
#include <iterator>

namespace fwData
{

class Image;

namespace iterator
{

/// Used to iterate through a RGB image
struct RGB {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

/// Used to iterate through a RGBA image
struct RGBA {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

/// Used to iterate through a BGR image
struct BGR {
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
};

/// Used to iterate through a BGRA image
struct BGRA {
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;
};

/**
 * @brief Iterator on Image buffer
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * @tparam FORMAT format used to iterate through the buffer, should be one of the formats defined in IterationBase
 *
 * Example:
 * @code{.cpp}
    ::fwData::Image::sptr img = ::fwData::Image::New();
    img->resize({1920, 1080}, ::fwTools::Type::s_UINT8, ::fwData::Image::PixelFormat::RGBA);
    ImageIteratorBase<RGBA> iter    = img->begin<RGBA>();
    const ImageIteratorBase<RGBA> iterEnd = img->end<RGBA>();

    for (; iter != iterEnd; ++iter)
    {
        iter->r = val1;
        iter->g = val2;
        iter->b = val2;
        iter->a = val4;
    }
   @endcode
 */
template <class FORMAT, bool isConstIterator = true>
class ImageIteratorBase
{
public:

    /**
     * For ConstIterator:   define input to be a const Image*
     * For Iterator: define input to be a Image*
     */
    typedef typename std::conditional<isConstIterator, const Image*, Image*>::type ImageType;

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    /**
     * For ConstIterator:   define buffer type to be a const TYPE*
     * For Iterator: define buffer type to be a TYPE*
     */
    typedef typename std::conditional<isConstIterator, const FORMAT*, FORMAT*>::type pointer;

    /**
     * For const_iterator:   define value_type to be a   const TYPE
     * For regular iterator: define value_type to be a   TYPE
     */
    typedef typename std::conditional<isConstIterator, const FORMAT, FORMAT>::type value_type;

    /**
     * For const_iterator:   define reference to be a   const TYPE&
     * For regular iterator: define reference to be a   TYPE&
     */
    typedef typename std::conditional<isConstIterator, const FORMAT&, FORMAT&>::type reference;

    /// Define difference type
    typedef std::ptrdiff_t difference_type;

    /// define the category of the iterator.
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    /// Constructor
    ImageIteratorBase(ImageType image);
    /// Copy constructor
    ImageIteratorBase(const ImageIteratorBase<FORMAT, false>& other);
    /// Copy constructor
    ImageIteratorBase(const ImageIteratorBase<FORMAT, true>& other);
    /// Destructor
    ~ImageIteratorBase();

    ImageIteratorBase& operator=(const ImageIteratorBase& other);

    /// Comparison operators
    bool operator==(const ImageIteratorBase& other) const;
    bool operator!=(const ImageIteratorBase& other) const;

    /// Increment/Decrement operators
    ImageIteratorBase& operator++();
    ImageIteratorBase operator++(int);
    ImageIteratorBase operator+(difference_type index) const;
    ImageIteratorBase& operator+=(difference_type index);
    ImageIteratorBase& operator--();
    ImageIteratorBase operator--(int);
    ImageIteratorBase operator-(difference_type index) const;
    ImageIteratorBase& operator-=(difference_type index);

    difference_type operator+(const ImageIteratorBase& other) const;
    difference_type operator-(const ImageIteratorBase& other) const;

    /// Value access operators
    reference operator*() const;

    /// Value access operators
    value_type* operator->() const;

protected:

    /// allow to create a ConstIterator from an Iterator
    friend class ImageIteratorBase<FORMAT, true>;

    pointer m_pointer{nullptr};
    ::fwMemory::BufferObject::Lock m_lock;
    difference_type m_idx{0};
    difference_type m_numberOfElements{0};
};

} // namespace iterator
} // namespace fwData

#include "fwData/iterator/ImageIterator.hxx"
