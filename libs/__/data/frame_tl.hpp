/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/factory/new.hpp"
#include "data/generic_tl.hpp"
#include "data/generic_tl.hxx"
#include "data/timeline/generic_object.hpp"
#include "data/timeline/generic_object.hxx"

#include <core/type.hpp>

#include <data/factory/new.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a timeline of images.
 */
class SIGHT_DATA_CLASS_API frame_tl final : public generic_tl<uint8_t>
{
public:

    SIGHT_DECLARE_CLASS(frame_tl, generic_tl<uint8_t>);

    /// Frame format
    enum class pixel_format
    {
        undefined = 0, ///< Undefined pixel format
        rgb,           ///< Frame with 3 component RGB.
        rgba,          ///< Frame with 4 component RGBA.
        bgr,           ///< Frame with 3 component BGR.
        bgra,          ///< Frame with 4 component BGRA.
        gray_scale     ///< Frame with 1 component.
    };

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API frame_tl();

    /// Destructor
    SIGHT_DATA_API ~frame_tl() noexcept override = default;

    /// Initializes the size of the pool buffer.
    SIGHT_DATA_API void init_pool_size(
        std::size_t _width,
        std::size_t _height,
        const core::type& _type,
        pixel_format _format,
        unsigned int _max_element_num = 1
    );

    /// Returns the width of an image in the timeline
    std::size_t get_width() const
    {
        return m_width;
    }

    /// Returns the height of an image in the timeline
    std::size_t get_height() const
    {
        return m_height;
    }

    /// Returns the number of components of an image in the timeline
    std::size_t num_components() const
    {
        return m_number_of_components;
    }

    /// Returns the type of the frame pixel
    core::type type() const
    {
        return m_type;
    }

    /// Returns the frame pixel format
    pixel_format pixel_format() const;

    /// Set the frame pixel format
    void set_pixel_format(enum pixel_format _format);

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const frame_tl& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const frame_tl& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    /// Forbid the use of this inherited method.
    SIGHT_DATA_API void init_pool_size(unsigned int _max_element_num) override;

    /// frame width
    std::size_t m_width {0};

    /// frame height
    std::size_t m_height {0};

    /// number of components
    std::size_t m_number_of_components {3};

    /// type of frame pixel
    core::type m_type;

    /// Frame format
    enum pixel_format m_pixel_format {pixel_format::undefined};
}; // class frame_tl

//-----------------------------------------------------------------------------

inline void frame_tl::set_pixel_format(enum pixel_format _format)
{
    m_pixel_format = _format;
}

//-----------------------------------------------------------------------------

inline enum frame_tl::pixel_format frame_tl::pixel_format() const
{
    return m_pixel_format;
}

} // namespace sight::data
