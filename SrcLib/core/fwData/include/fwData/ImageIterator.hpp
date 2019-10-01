/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <vector>

namespace fwData
{

class Image;

/**
 * @brief Iterator on Image buffer
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 */
template <class TYPE, bool isConstIterator = true>
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
    typedef typename std::conditional<isConstIterator, const TYPE*, TYPE*>::type pointer;

    /**
     * For const_iterator:   define value_type to be a   const TYPE
     * For regular iterator: define value_type to be a   TYPE
     */
    typedef typename std::conditional<isConstIterator, const TYPE, TYPE>::type value_type;

    /**
     * For const_iterator:   define reference to be a   const TYPE&
     * For regular iterator: define reference to be a   TYPE&
     */
    typedef typename std::conditional<isConstIterator, const TYPE&, TYPE&>::type reference;

    /// Define difference type
    typedef std::ptrdiff_t difference_type;

    /// define the category of the iterator.
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    /// Constructor
    ImageIteratorBase(ImageType array);
    /// Copy constructor
    ImageIteratorBase(const ImageIteratorBase<TYPE, false>& other);
    /// Destructor
    ~ImageIteratorBase();

    /// Comparison operators
    bool operator==(const ImageIteratorBase& other) const;
    bool operator!=(const ImageIteratorBase& other) const;

    /// Increment/Decrement operators
    ImageIteratorBase& operator++();
    ImageIteratorBase operator++(int);
    ImageIteratorBase& operator+(difference_type index);
    ImageIteratorBase& operator+=(difference_type index);
    ImageIteratorBase& operator--();
    ImageIteratorBase operator--(int);
    ImageIteratorBase& operator-(difference_type index);
    ImageIteratorBase& operator-=(difference_type index);

    difference_type operator+(const ImageIteratorBase& other) const;
    difference_type operator-(const ImageIteratorBase& other) const;

    /// Value access operators
    reference operator*();

private:

    /// allow to create a ConstIterator from an Iterator
    friend class ImageIteratorBase<TYPE, true>;

    pointer m_pointer{nullptr};
    difference_type m_idx{0};
    const difference_type m_numberOfElements;
};

} // namespace fwData
