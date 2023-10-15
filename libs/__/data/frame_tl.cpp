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

#include "data/frame_tl.hpp"

#include <core/base.hpp>

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::frame_tl);

namespace sight::data
{

//------------------------------------------------------------------------------

frame_tl::frame_tl()
{
    // Default to 100 frames since images used to eat a lot of memory...
    this->setMaximumSize(100);
}

//------------------------------------------------------------------------------

void frame_tl::shallow_copy(const object::csptr& /*unused*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

void frame_tl::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const frame_tl>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clearTimeline();

    this->initPoolSize(other->m_width, other->m_height, other->m_type, other->m_pixelFormat);

    for(const TimelineType::value_type& elt : other->m_timeline)
    {
        SPTR(data::timeline::buffer) tlObj = this->createBuffer(elt.first);
        tlObj->deep_copy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void frame_tl::initPoolSize(
    std::size_t width,
    std::size_t height,
    const core::type& type,
    const PixelFormat format,
    unsigned int maxElementNum
)
{
    m_width       = width;
    m_height      = height;
    m_type        = type;
    m_pixelFormat = format;
    switch(format)
    {
        case data::frame_tl::PixelFormat::GRAY_SCALE:
            m_numberOfComponents = 1;
            break;

        case data::frame_tl::PixelFormat::BGR:
        case data::frame_tl::PixelFormat::RGB:
            m_numberOfComponents = 3;
            break;

        case data::frame_tl::PixelFormat::BGRA:
        case data::frame_tl::PixelFormat::RGBA:
            m_numberOfComponents = 4;
            break;

        default:
            m_numberOfComponents = 1;
    }

    std::size_t size = width * height * m_numberOfComponents * type.size();

    SIGHT_ASSERT("width or height or numberOfComponents is null", size != 0);

    m_maxElementNum = maxElementNum;
    this->allocPoolSize(size * m_maxElementNum);
}

//------------------------------------------------------------------------------

void frame_tl::initPoolSize(unsigned int /*maxElementNum*/)
{
    SIGHT_ERROR("This function should not be called");
}

//------------------------------------------------------------------------------

bool frame_tl::operator==(const frame_tl& other) const noexcept
{
    if(m_width != other.m_width
       || m_height != other.m_height
       || m_numberOfComponents != other.m_numberOfComponents
       || m_type != other.m_type
       || m_pixelFormat != other.m_pixelFormat)
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool frame_tl::operator!=(const frame_tl& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
