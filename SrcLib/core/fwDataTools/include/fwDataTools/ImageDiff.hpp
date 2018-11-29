/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __FWDATATOOLS_IMAGEDIFF_HPP__
#define __FWDATATOOLS_IMAGEDIFF_HPP__

#include "fwDataTools/config.hpp"
#include "fwDataTools/helper/Image.hpp"

#include <fwData/Image.hpp>

namespace fwDataTools
{

/**
 * @brief Class memorizing pixel changes in an image.
 */
class FWDATATOOLS_CLASS_API ImageDiff
{
public:

    struct ElementType
    {
        ::fwData::Image::IndexType m_index;
        ::fwData::Image::BufferType* m_oldValue;
        ::fwData::Image::BufferType* m_newValue;
    };

    /// Constructor.
    FWDATATOOLS_API ImageDiff(const size_t imageElementSize = 0, const size_t reservedElements = 0);

    /// Destructor
    FWDATATOOLS_API ~ImageDiff();

    /// Copy constructor
    FWDATATOOLS_API ImageDiff(const ImageDiff& other);

    /// Move constructor
    FWDATATOOLS_API ImageDiff(ImageDiff&& other);

    /// Copy assignement.
    FWDATATOOLS_API ImageDiff& operator= (const ImageDiff& other);

    /// Move assignement.
    FWDATATOOLS_API ImageDiff& operator= (ImageDiff&& other);

    /// Concatenate two diffs.
    FWDATATOOLS_API void addDiff(const ImageDiff& diff);

    /// Append a new pixel diff.
    FWDATATOOLS_API void addDiff(const ::fwData::Image::IndexType index, const ::fwData::Image::BufferType* oldValue,
                                 const ::fwData::Image::BufferType* newValue);

    /// Write the new values in the image.
    FWDATATOOLS_API void applyDiff(const ::fwData::Image::sptr& img) const;

    /// Write the old value back in the image.
    FWDATATOOLS_API void revertDiff(const ::fwData::Image::sptr& img) const;

    /// Return the amount of memory actually used by the elements.
    FWDATATOOLS_API size_t getSize() const;

    /// Returns the number of stored pixel diffs.
    FWDATATOOLS_API size_t getNumberOfElements() const;

    /// Set the number of elements to 0.
    FWDATATOOLS_API void clear();

    /// Reallocate the buffer to fit the actual size of the container.
    FWDATATOOLS_API void shrink();

    /// Returns the element at the given index
    FWDATATOOLS_API ElementType getElement(size_t index) const;

    /// Returns the image index from the element at the given index
    inline ::fwData::Image::IndexType getElementDiffIndex(size_t eltIndex) const;

private:

    /// Write the new value in the image from one element.
    void applyDiffElt(helper::Image& img, size_t eltIndex) const;

    /// Write the old value back in the image from one element.
    void revertDiffElt(helper::Image& img, size_t eltIndex) const;

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

fwData::Image::IndexType ImageDiff::getElementDiffIndex(size_t eltIndex) const
{
    std::uint8_t* eltPtr = m_buffer + eltIndex * m_eltSize;
    return *reinterpret_cast< ::fwData::Image::IndexType* >(eltPtr);
}

} // namespace fwDataTools

#endif // __FWDATATOOLS_IMAGEDIFF_HPP__
