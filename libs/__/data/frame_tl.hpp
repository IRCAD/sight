/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
class DATA_CLASS_API frame_tl final : public generic_tl<uint8_t>
{
public:

    SIGHT_DECLARE_CLASS(frame_tl, generic_tl<uint8_t>);

    /// Frame format
    enum class PixelFormat
    {
        UNDEFINED = 0, ///< Undefined pixel format
        RGB,           ///< Frame with 3 component RGB.
        RGBA,          ///< Frame with 4 component RGBA.
        BGR,           ///< Frame with 3 component BGR.
        BGRA,          ///< Frame with 4 component BGRA.
        GRAY_SCALE     ///< Frame with 1 component.
    };

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API frame_tl();

    /// Destructor
    DATA_API ~frame_tl() noexcept override = default;

    /// Initializes the size of the pool buffer.
    DATA_API void initPoolSize(
        std::size_t _width,
        std::size_t _height,
        const core::type& _type,
        PixelFormat _format,
        unsigned int _max_element_num = 1
    );

    /// Returns the width of an image in the timeline
    std::size_t getWidth() const
    {
        return m_width;
    }

    /// Returns the height of an image in the timeline
    std::size_t getHeight() const
    {
        return m_height;
    }

    /// Returns the number of components of an image in the timeline
    std::size_t numComponents() const
    {
        return m_numberOfComponents;
    }

    /// Returns the type of the frame pixel
    core::type getType() const
    {
        return m_type;
    }

    /// Returns the frame pixel format
    PixelFormat getPixelFormat() const;

    /// Set the frame pixel format
    void setPixelFormat(PixelFormat _format);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const frame_tl& _other) const noexcept;
    DATA_API bool operator!=(const frame_tl& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    /// Forbid the use of this inherited method.
    DATA_API void initPoolSize(unsigned int _max_element_num) override;

    /// frame width
    std::size_t m_width {0};

    /// frame height
    std::size_t m_height {0};

    /// number of components
    std::size_t m_numberOfComponents {3};

    /// type of frame pixel
    core::type m_type;

    /// Frame format
    PixelFormat m_pixelFormat {PixelFormat::UNDEFINED};
}; // class frame_tl

//-----------------------------------------------------------------------------

inline void frame_tl::setPixelFormat(PixelFormat _format)
{
    m_pixelFormat = _format;
}

//-----------------------------------------------------------------------------

inline frame_tl::PixelFormat frame_tl::getPixelFormat() const
{
    return m_pixelFormat;
}

} // namespace sight::data
