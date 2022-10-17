/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/RawBufferTL.hpp"

#include "data/timeline/RawBuffer.hpp"

#include <data/Exception.hpp>
#include <data/registry/macros.hpp>

#include <functional>

SIGHT_REGISTER_DATA(sight::data::RawBufferTL);

namespace sight::data
{

//------------------------------------------------------------------------------

RawBufferTL::RawBufferTL(data::Object::Key key) :
    BufferTL(key)
{
}

//------------------------------------------------------------------------------

void RawBufferTL::shallowCopy(const Object::csptr& /*source*/)
{
    SIGHT_FATAL("shallowCopy not implemented for : " + this->getClassname());
}

//------------------------------------------------------------------------------

void RawBufferTL::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    this->clearTimeline();
    this->allocPoolSize(other->m_pool->get_requested_size());

    for(const TimelineType::value_type& elt : other->m_timeline)
    {
        SPTR(data::timeline::RawBuffer) tlObj = this->createBuffer(elt.first);
        tlObj->deepCopy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::RawBuffer) RawBufferTL::getClosestBuffer(
    core::HiResClock::HiResClockType timestamp,
    DirectionType direction
) const
{
    CSPTR(data::timeline::Object) buffer = this->getClosestObject(timestamp, direction);
    return std::dynamic_pointer_cast<const data::timeline::RawBuffer>(buffer);
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::RawBuffer) RawBufferTL::getBuffer(core::HiResClock::HiResClockType timestamp) const
{
    CSPTR(data::timeline::Object) buffer = this->getObject(timestamp);
    return std::dynamic_pointer_cast<const data::timeline::RawBuffer>(buffer);
}

//------------------------------------------------------------------------------

void RawBufferTL::initPoolSize(std::size_t size)
{
    this->allocPoolSize(size);
}

//------------------------------------------------------------------------------

SPTR(data::timeline::Object) RawBufferTL::createObject(core::HiResClock::HiResClockType timestamp)
{
    return this->createBuffer(timestamp);
}

//------------------------------------------------------------------------------

SPTR(data::timeline::RawBuffer) RawBufferTL::createBuffer(core::HiResClock::HiResClockType timestamp)
{
    return std::make_shared<data::timeline::RawBuffer>(
        timestamp,
        reinterpret_cast<data::timeline::Buffer::BufferDataType>(m_pool->malloc()),
        m_pool->get_requested_size(),
        [ObjectPtr = m_pool](auto&& PH1, auto&& ...){ObjectPtr->free(std::forward<decltype(PH1)>(PH1));});
}

//------------------------------------------------------------------------------

bool RawBufferTL::isObjectValid(const CSPTR(data::timeline::Object)& obj) const
{
    CSPTR(data::timeline::RawBuffer) srcObj =
        std::dynamic_pointer_cast<const data::timeline::RawBuffer>(obj);
    return srcObj != nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::data
