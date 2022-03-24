/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "data/GenericTL.hpp"
#include "data/GenericTL.hxx"
#include "data/timeline/GenericObject.hpp"
#include "data/timeline/GenericObject.hxx"

#include <core/Type.hpp>

#include <data/factory/new.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a timeline of images.
 */
class DATA_CLASS_API FrameTL : public GenericTL<uint8_t>
{
public:

    SIGHT_DECLARE_CLASS(FrameTL, TimeLine, factory::New<FrameTL>);

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
    DATA_API FrameTL(Object::Key key);

    /// Destructor
    DATA_API virtual ~FrameTL();
    /// Initializes the size of the pool buffer.
    DATA_API void initPoolSize(
        std::size_t width,
        std::size_t height,
        const core::Type& type,
        const PixelFormat format,
        unsigned int maxElementNum = 1
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
    core::Type getType() const
    {
        return m_type;
    }

    /// Returns the frame pixel format
    PixelFormat getPixelFormat() const;

    /// Set the frame pixel format
    void setPixelFormat(PixelFormat format);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const FrameTL& other) const noexcept;
    DATA_API bool operator!=(const FrameTL& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    /// Forbid the use of this inherited method.
    DATA_API void initPoolSize(unsigned int maxElementNum) override;

    /// frame width
    std::size_t m_width;

    /// frame height
    std::size_t m_height;

    /// number of components
    std::size_t m_numberOfComponents;

    /// type of frame pixel
    core::Type m_type;

    /// Frame format
    PixelFormat m_pixelFormat {PixelFormat::UNDEFINED};
}; // class FrameTL

//-----------------------------------------------------------------------------

inline void FrameTL::setPixelFormat(PixelFormat format)
{
    m_pixelFormat = format;
}

//-----------------------------------------------------------------------------

inline FrameTL::PixelFormat FrameTL::getPixelFormat() const
{
    return m_pixelFormat;
}

} // namespace sight::data
