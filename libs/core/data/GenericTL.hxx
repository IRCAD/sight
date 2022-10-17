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

#pragma once

#include <data/Exception.hpp>

namespace sight::data
{

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
GenericTL<BUFFER_TYPE>::GenericTL(data::Object::Key key) :
    BufferTL(key),
    m_maxElementNum(~0U)
{
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void GenericTL<BUFFER_TYPE>::shallowCopy(const Object::csptr&)
{
    SIGHT_FATAL("shallowCopy not implemented for : " + this->getClassname());
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void GenericTL<BUFFER_TYPE>::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
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
    this->initPoolSize(other->getMaxElementNum());

    for(const TimelineType::value_type& elt : other->m_timeline)
    {
        SPTR(BufferType) tlObj = this->createBuffer(elt.first);
        tlObj->deepCopy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
CSPTR(typename GenericTL<BUFFER_TYPE>::BufferType)
GenericTL<BUFFER_TYPE>::getClosestBuffer(
    core::HiResClock::HiResClockType timestamp,
    DirectionType direction
) const
{
    CSPTR(data::timeline::Object) buffer = this->getClosestObject(timestamp, direction);
    return std::dynamic_pointer_cast<const BufferType>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
CSPTR(typename GenericTL<BUFFER_TYPE>::BufferType)
GenericTL<BUFFER_TYPE>::getBuffer(core::HiResClock::HiResClockType timestamp) const
{
    CSPTR(data::timeline::Object) buffer = this->getObject(timestamp);
    return std::dynamic_pointer_cast<const BufferType>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void GenericTL<BUFFER_TYPE>::initPoolSize(unsigned int maxElementNum)
{
    m_maxElementNum = maxElementNum;
    this->allocPoolSize(sizeof(BUFFER_TYPE) * m_maxElementNum);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
SPTR(data::timeline::Object)
GenericTL<BUFFER_TYPE>::createObject(core::HiResClock::HiResClockType timestamp)
{
    return this->createBuffer(timestamp);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
SPTR(typename GenericTL<BUFFER_TYPE>::BufferType)
GenericTL<BUFFER_TYPE>::createBuffer(core::HiResClock::HiResClockType timestamp)
{
    SPTR(BufferType) obj = std::make_shared<BufferType>(
        m_maxElementNum,
        timestamp,
        (data::timeline::Buffer::BufferDataType) m_pool->malloc(),
        m_pool->get_requested_size(),
        [ObjectPtr = m_pool](auto&& PH1, auto&& ...){ObjectPtr->free(std::forward<decltype(PH1)>(PH1));});
    return obj;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool GenericTL<BUFFER_TYPE>::isObjectValid(const CSPTR(data::timeline::Object)& obj) const
{
    CSPTR(BufferType) srcObj = std::dynamic_pointer_cast<const BufferType>(obj);
    return srcObj != nullptr;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
unsigned int GenericTL<BUFFER_TYPE>::getMaxElementNum() const
{
    return m_maxElementNum;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool GenericTL<BUFFER_TYPE>::operator==(const GenericTL& other) const noexcept
{
    if(m_maxElementNum != other.m_maxElementNum)
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool GenericTL<BUFFER_TYPE>::operator!=(const GenericTL& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
