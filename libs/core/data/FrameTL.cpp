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

#include "data/FrameTL.hpp"

#include <core/base.hpp>

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

#include <boost/pool/pool.hpp>

SIGHT_REGISTER_DATA(sight::data::FrameTL);

namespace sight::data
{

//------------------------------------------------------------------------------

FrameTL::FrameTL(data::Object::Key key) :
    GenericTL<uint8_t>(key),
    m_width(0),
    m_height(0),
    m_numberOfComponents(3),
    m_type()
{
    // Default to 100 frames since images used to eat a lot of memory...
    this->setMaximumSize(100);
}

//------------------------------------------------------------------------------

FrameTL::~FrameTL()
{
}

//------------------------------------------------------------------------------

void FrameTL::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType&)
{
    FrameTL::csptr other = FrameTL::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source);

    this->clearTimeline();

    this->initPoolSize(other->m_width, other->m_height, other->m_type, other->m_pixelFormat);

    for(TimelineType::value_type elt : other->m_timeline)
    {
        SPTR(data::timeline::Buffer) tlObj = this->createBuffer(elt.first);
        tlObj->deepCopy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }
}

//------------------------------------------------------------------------------

void FrameTL::initPoolSize(
    std::size_t width,
    std::size_t height,
    const core::tools::Type& type,
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

    std::size_t size = width * height * m_numberOfComponents * type.sizeOf();

    SIGHT_ASSERT("width or height or numberOfComponents is null", size != 0);

    m_maxElementNum = maxElementNum;
    this->allocPoolSize(size * m_maxElementNum);
}

//------------------------------------------------------------------------------

void FrameTL::initPoolSize(unsigned int)
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
    return GenericTL<uint8_t>::operator==(other);
}

//------------------------------------------------------------------------------

bool FrameTL::operator!=(const FrameTL& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
