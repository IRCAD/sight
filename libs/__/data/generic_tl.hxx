/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/exception.hpp>

namespace sight::data
{

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
generic_tl<BUFFER_TYPE>::generic_tl() :
    buffer_tl(),
    m_maxElementNum(~0U)
{
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void generic_tl<BUFFER_TYPE>::shallow_copy(const object::csptr&)
{
    SIGHT_FATAL("shallow_copy not implemented for : " + this->get_classname());
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void generic_tl<BUFFER_TYPE>::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const generic_tl<BUFFER_TYPE> >(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clearTimeline();
    this->initPoolSize(other->getMaxElementNum());

    for(const TimelineType::value_type& elt : other->m_timeline)
    {
        SPTR(BufferType) tlObj = this->createBuffer(elt.first);
        tlObj->deep_copy(*elt.second);
        m_timeline.insert(TimelineType::value_type(elt.first, tlObj));
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
CSPTR(typename generic_tl<BUFFER_TYPE>::BufferType)
generic_tl<BUFFER_TYPE>::getClosestBuffer(
    core::hires_clock::type timestamp,
    timeline::direction_t direction
) const
{
    CSPTR(data::timeline::object) buffer = this->getClosestObject(timestamp, direction);
    return std::dynamic_pointer_cast<const BufferType>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
CSPTR(typename generic_tl<BUFFER_TYPE>::BufferType)
generic_tl<BUFFER_TYPE>::get_buffer(core::hires_clock::type timestamp) const
{
    CSPTR(data::timeline::object) buffer = this->getObject(timestamp);
    return std::dynamic_pointer_cast<const BufferType>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void generic_tl<BUFFER_TYPE>::initPoolSize(unsigned int maxElementNum)
{
    m_maxElementNum = maxElementNum;
    this->allocPoolSize(sizeof(BUFFER_TYPE) * m_maxElementNum);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
SPTR(data::timeline::object)
generic_tl<BUFFER_TYPE>::createObject(core::hires_clock::type timestamp)
{
    return this->createBuffer(timestamp);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
SPTR(typename generic_tl<BUFFER_TYPE>::BufferType)
generic_tl<BUFFER_TYPE>::createBuffer(core::hires_clock::type timestamp)
{
    SPTR(BufferType) obj = std::make_shared<BufferType>(
        m_maxElementNum,
        timestamp,
        (data::timeline::buffer::BufferDataType) m_pool->malloc(),
        m_pool->get_requested_size(),
        [ObjectPtr = m_pool](auto&& PH1, auto&& ...){ObjectPtr->free(std::forward<decltype(PH1)>(PH1));});
    return obj;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::isObjectValid(const CSPTR(data::timeline::object)& obj) const
{
    CSPTR(BufferType) srcObj = std::dynamic_pointer_cast<const BufferType>(obj);
    return srcObj != nullptr;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
unsigned int generic_tl<BUFFER_TYPE>::getMaxElementNum() const
{
    return m_maxElementNum;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::operator==(const generic_tl& other) const noexcept
{
    if(m_maxElementNum != other.m_maxElementNum)
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::operator!=(const generic_tl& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
