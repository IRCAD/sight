/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "filter/image/config.hpp"

#include <data/image.hpp>

namespace sight::filter::image
{

/**
 * @brief Class memorizing pixel changes in an image.
 */
class FILTER_IMAGE_CLASS_API image_diff
{
public:

    struct ElementType
    {
        data::image::IndexType m_index;
        const data::image::BufferType* m_oldValue;
        const data::image::BufferType* m_newValue;
    };

    /// Constructor.
    FILTER_IMAGE_API explicit image_diff(std::size_t imageElementSize = 0, std::size_t reservedElements = 0);

    /// Destructor
    FILTER_IMAGE_API ~image_diff() = default;

    /// Copy constructor
    FILTER_IMAGE_API image_diff(const image_diff& other) = default;

    /// Move constructor
    FILTER_IMAGE_API image_diff(image_diff&& other) noexcept = default;

    /// Copy assignement.
    FILTER_IMAGE_API image_diff& operator=(const image_diff& other) = default;

    /// Move assignement.
    FILTER_IMAGE_API image_diff& operator=(image_diff&& other) noexcept = default;

    /// Concatenate two diffs.
    FILTER_IMAGE_API void addDiff(const image_diff& diff);

    /// Append a new pixel diff.
    FILTER_IMAGE_API void addDiff(
        data::image::IndexType index,
        const data::image::BufferType* oldValue,
        const data::image::BufferType* newValue
    );

    /// Write the new values in the image.
    FILTER_IMAGE_API void applyDiff(const data::image::sptr& img) const;

    /// Write the old value back in the image.
    FILTER_IMAGE_API void revertDiff(const data::image::sptr& img) const;

    /// Return the amount of memory actually used by the elements.
    [[nodiscard]] FILTER_IMAGE_API std::size_t size() const;

    /// Returns the number of stored pixel diffs.
    [[nodiscard]] FILTER_IMAGE_API std::size_t numElements() const;

    /// Set the number of elements to 0.
    FILTER_IMAGE_API void clear();

    /// Reallocate the buffer to fit the actual size of the container.
    FILTER_IMAGE_API void shrink();

    /// Returns the element at the given index
    [[nodiscard]] FILTER_IMAGE_API ElementType getElement(std::size_t index) const;

    /// Returns the image index from the element at the given index
    [[nodiscard]] inline data::image::IndexType getElementDiffIndex(std::size_t eltIndex) const;

private:

    /// Write the new value in the image from one element.
    void applyDiffElt(const data::image::sptr& img, std::size_t eltIndex) const;

    /// Write the old value back in the image from one element.
    void revertDiffElt(const data::image::sptr& img, std::size_t eltIndex) const;

    /// The size of a single pixel diff.
    std::size_t m_imgEltSize;

    /// Size of an element (image index + old value + new value)
    std::size_t m_eltSize;

    /// The buffer holding the diff.
    std::vector<std::uint8_t> m_buffer;
};

//------------------------------------------------------------------------------

data::image::IndexType image_diff::getElementDiffIndex(std::size_t eltIndex) const
{
    return *reinterpret_cast<const data::image::IndexType*>(&m_buffer[eltIndex * m_eltSize]);
}

} // namespace sight::filter::image
