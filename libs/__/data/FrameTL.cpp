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

#include "data/FrameTL.hpp"

#include <core/base.hpp>

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

SIGHT_REGISTER_DATA(sight::data::FrameTL);

namespace sight::data
{

//------------------------------------------------------------------------------

FrameTL::FrameTL()
{
    // Default to 100 frames since images used to eat a lot of memory...
    this->setMaximumSize(100);
}

//------------------------------------------------------------------------------

void FrameTL::shallow_copy(const Object::csptr& /*unused*/)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

void FrameTL::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const FrameTL>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clearTimeline();

    this->initPoolSize(other->m_width, other->m_height, other->m_type, other->m_pixelFormat);

    for(const TimelineType::value_type& elt : other->m_timeline)
    {
        SPTR(data::timeline::Buffer) tlObj = this->createBuffer(elt.first);
        tlObj->deep_copy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void FrameTL::initPoolSize(
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
        case data::FrameTL::PixelFormat::GRAY_SCALE:
            m_numberOfComponents = 1;
            break;

        case data::FrameTL::PixelFormat::BGR:
        case data::FrameTL::PixelFormat::RGB:
            m_numberOfComponents = 3;
            break;

        case data::FrameTL::PixelFormat::BGRA:
        case data::FrameTL::PixelFormat::RGBA:
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

void FrameTL::initPoolSize(unsigned int /*maxElementNum*/)
{
    SIGHT_ERROR("This function should not be called");
}

//------------------------------------------------------------------------------

bool FrameTL::operator==(const FrameTL& other) const noexcept
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

bool FrameTL::operator!=(const FrameTL& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
