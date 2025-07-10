/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <sight/filter/image/config.hpp>

#include <data/image.hpp>

namespace sight::filter::image
{

/**
 * @brief Class memorizing pixel changes in an image.
 */
class SIGHT_FILTER_IMAGE_CLASS_API image_diff
{
public:

    struct element_t
    {
        data::image::index_t m_index;
        const data::image::buffer_t* m_old_value;
        const data::image::buffer_t* m_new_value;
    };

    /// Constructor.
    SIGHT_FILTER_IMAGE_API explicit image_diff(std::size_t _image_element_size = 0, std::size_t _reserved_elements = 0);

    /// Destructor
    SIGHT_FILTER_IMAGE_API ~image_diff() = default;

    /// Copy constructor
    SIGHT_FILTER_IMAGE_API image_diff(const image_diff& _other) = default;

    /// Move constructor
    SIGHT_FILTER_IMAGE_API image_diff(image_diff&& _other) noexcept = default;

    /// Copy assignement.
    SIGHT_FILTER_IMAGE_API image_diff& operator=(const image_diff& _other) = default;

    /// Move assignement.
    SIGHT_FILTER_IMAGE_API image_diff& operator=(image_diff&& _other) noexcept = default;

    /// Concatenate two diffs.
    SIGHT_FILTER_IMAGE_API void add_diff(const image_diff& _diff);

    /// Append a new pixel diff.
    SIGHT_FILTER_IMAGE_API void add_diff(
        data::image::index_t _index,
        const data::image::buffer_t* _old_value,
        const data::image::buffer_t* _new_value
    );

    /// Write the new values in the image.
    SIGHT_FILTER_IMAGE_API void apply_diff(const data::image::sptr& _img) const;

    /// Write the old value back in the image.
    SIGHT_FILTER_IMAGE_API void revert_diff(const data::image::sptr& _img) const;

    /// Return the amount of memory actually used by the elements.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API std::size_t size() const;

    /// Returns the number of stored pixel diffs.
    [[nodiscard]] SIGHT_FILTER_IMAGE_API std::size_t num_elements() const;

    /// Set the number of elements to 0.
    SIGHT_FILTER_IMAGE_API void clear();

    /// Reallocate the buffer to fit the actual size of the container.
    SIGHT_FILTER_IMAGE_API void shrink();

    /// Returns the element at the given index
    [[nodiscard]] SIGHT_FILTER_IMAGE_API element_t get_element(std::size_t _index) const;

    /// Returns the image index from the element at the given index
    [[nodiscard]] inline data::image::index_t get_element_diff_index(std::size_t _elt_index) const;

private:

    /// Write the new value in the image from one element.
    void apply_diff_elt(const data::image::sptr& _img, std::size_t _elt_index) const;

    /// Write the old value back in the image from one element.
    void revert_diff_elt(const data::image::sptr& _img, std::size_t _elt_index) const;

    /// The size of a single pixel diff.
    std::size_t m_img_elt_size;

    /// Size of an element (image index + old value + new value)
    std::size_t m_elt_size;

    /// The buffer holding the diff.
    std::vector<std::uint8_t> m_buffer;
};

//------------------------------------------------------------------------------

data::image::index_t image_diff::get_element_diff_index(std::size_t _elt_index) const
{
    return *reinterpret_cast<const data::image::index_t*>(&m_buffer[_elt_index * m_elt_size]);
}

} // namespace sight::filter::image
