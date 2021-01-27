/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "data/config.hpp"

#include <data/Image.hpp>

namespace sight::data::tools
{

/**
 * @brief Class memorizing pixel changes in an image.
 */
class DATA_CLASS_API ImageDiff
{
public:

    struct ElementType
    {
        data::Image::IndexType m_index;
        data::Image::BufferType* m_oldValue;
        data::Image::BufferType* m_newValue;
    };

    /// Constructor.
    DATA_API ImageDiff(const size_t imageElementSize = 0, const size_t reservedElements = 0);

    /// Destructor
    DATA_API ~ImageDiff();

    /// Copy constructor
    DATA_API ImageDiff(const ImageDiff& other);

    /// Move constructor
    DATA_API ImageDiff(ImageDiff&& other);

    /// Copy assignement.
    DATA_API ImageDiff& operator= (const ImageDiff& other);

    /// Move assignement.
    DATA_API ImageDiff& operator= (ImageDiff&& other);

    /// Concatenate two diffs.
    DATA_API void addDiff(const ImageDiff& diff);

    /// Append a new pixel diff.
    DATA_API void addDiff(const data::Image::IndexType index, const data::Image::BufferType* oldValue,
                          const data::Image::BufferType* newValue);

    /// Write the new values in the image.
    DATA_API void applyDiff(const data::Image::sptr& img) const;

    /// Write the old value back in the image.
    DATA_API void revertDiff(const data::Image::sptr& img) const;

    /// Return the amount of memory actually used by the elements.
    DATA_API size_t getSize() const;

    /// Returns the number of stored pixel diffs.
    DATA_API size_t getNumberOfElements() const;

    /// Set the number of elements to 0.
    DATA_API void clear();

    /// Reallocate the buffer to fit the actual size of the container.
    DATA_API void shrink();

    /// Returns the element at the given index
    DATA_API ElementType getElement(size_t index) const;

    /// Returns the image index from the element at the given index
    inline data::Image::IndexType getElementDiffIndex(size_t eltIndex) const;

private:

    /// Write the new value in the image from one element.
    void applyDiffElt(const data::Image::sptr& img, size_t eltIndex) const;

    /// Write the old value back in the image from one element.
    void revertDiffElt(const data::Image::sptr& img, size_t eltIndex) const;

    /// The size of a single pixel diff.
    size_t m_imgEltSize;

    /// Size of an element (image index + old value + new value)
    size_t m_eltSize;

    /// Number of pixel diffs.
    size_t m_nbElts;

    /// The allocated buffer size.
    size_t m_reservedSize;

    /// The buffer holding the diff.
    std::uint8_t* m_buffer;
};

//------------------------------------------------------------------------------

data::Image::IndexType ImageDiff::getElementDiffIndex(size_t eltIndex) const
{
    std::uint8_t* eltPtr = m_buffer + eltIndex * m_eltSize;
    return *reinterpret_cast< data::Image::IndexType* >(eltPtr);
}

} // namespace sight::data::tools
