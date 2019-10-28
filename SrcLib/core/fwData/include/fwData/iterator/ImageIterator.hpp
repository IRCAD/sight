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

#include "fwMemory/BufferObject.hpp"
#include <iterator>

namespace fwData
{

class Image;

namespace iterator
{

/**
 *  @brief Class for Image iteration
 *
 * Define the formats used to iterate through an image buffer
 */
template <class TYPE>
class IterationBase
{

public:

    /// Used to iterate through all the values of an image
    struct Raw {
        typedef TYPE type;
        typedef TYPE value_type;

        //------------------------------------------------------------------------------

        Raw& operator=(const value_type& val)
        {
            value = val;
            return *this;
        }
        //------------------------------------------------------------------------------

        type& operator*()
        {
            return value;
        }
        type value;
        static constexpr size_t elementSize{1};
    };

    /// Used to iterate through a RGB image
    struct RGB {
        typedef TYPE type;
        typedef std::array<type, 3> value_type;

        //------------------------------------------------------------------------------

        RGB& operator=(const value_type& val)
        {
            r = val[0];
            g = val[1];
            b = val[2];
            return *this;
        }

        type r;
        type g;
        type b;
        static constexpr size_t elementSize{3};
    };

    /// Used to iterate through a RGBA image
    struct RGBA {
        typedef TYPE type;
        typedef std::array<type, 4> value_type;
        //------------------------------------------------------------------------------

        RGBA& operator=(const value_type& val)
        {
            r = val[0];
            g = val[1];
            b = val[2];
            a = val[3];
            return *this;
        }
        type r;
        type g;
        type b;
        type a;
        static constexpr size_t elementSize{4};
    };

    /// Used to iterate through a BGR image
    struct BGR {
        typedef TYPE type;
        typedef std::array<type, 3> value_type;
        //------------------------------------------------------------------------------

        BGR& operator=(const value_type& val)
        {
            b = val[0];
            g = val[1];
            r = val[2];
            return *this;
        }
        type b;
        type g;
        type r;
        static constexpr size_t elementSize{3};
    };

    /// Used to iterate through a BGRA image
    struct BGRA {
        typedef TYPE type;
        typedef std::array<type, 4> value_type;
        //------------------------------------------------------------------------------

        RGBA& operator=(const value_type& val)
        {
            b = val[0];
            g = val[1];
            r = val[2];
            a = val[3];
            return *this;
        }
        type b;
        type g;
        type r;
        type a;
        static constexpr size_t elementSize{4};
    };
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
    ImageIteratorBase<IterationBase<std::int8_t>::RGBA> iter    = img->begin<IterationBase<std::int16_t>::RGBA>();
    const ImageIteratorBase<IterationBase<std::int8_t>::RGBA> iterEnd = img->end<IterationBase<std::int16_t>::RGBA>();

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
class ImageIteratorBase : public IterationBase<typename FORMAT::type>
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
    typedef typename std::conditional<isConstIterator,
                                      const typename FORMAT::type*,
                                      typename FORMAT::type*>::type pointer;

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
    /// Destructor
    ~ImageIteratorBase();

    ImageIteratorBase& operator=(const ImageIteratorBase& other);

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

    /// Value access operators
    value_type* operator->();

protected:

    /// allow to create a ConstIterator from an Iterator
    friend class ImageIteratorBase<FORMAT, true>;

    pointer m_pointer{nullptr};
    ::fwMemory::BufferObject::Lock m_lock;
    difference_type m_idx{0};
    difference_type m_elementSize{1};
    difference_type m_numberOfElements{0};
};

} // namespace iterator
} // namespace fwData

#include "fwData/iterator/ImageIterator.hxx"
