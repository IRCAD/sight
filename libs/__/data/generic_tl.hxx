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
void generic_tl<BUFFER_TYPE>::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const generic_tl<BUFFER_TYPE> >(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    this->clearTimeline();
    this->initPoolSize(other->getMaxElementNum());

    for(const timeline_t::value_type& elt : other->m_timeline)
    {
        SPTR(buffer_t) tl_obj = this->createBuffer(elt.first);
        tl_obj->deep_copy(*elt.second);
        m_timeline.insert(timeline_t::value_type(elt.first, tl_obj));
    }

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
CSPTR(typename generic_tl<BUFFER_TYPE>::buffer_t)
generic_tl<BUFFER_TYPE>::getClosestBuffer(
    core::hires_clock::type _timestamp,
    timeline::direction_t _direction
) const
{
    CSPTR(data::timeline::object) buffer = this->getClosestObject(_timestamp, _direction);
    return std::dynamic_pointer_cast<const buffer_t>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
CSPTR(typename generic_tl<BUFFER_TYPE>::buffer_t)
generic_tl<BUFFER_TYPE>::get_buffer(core::hires_clock::type _timestamp) const
{
    CSPTR(data::timeline::object) buffer = this->getObject(_timestamp);
    return std::dynamic_pointer_cast<const buffer_t>(buffer);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
void generic_tl<BUFFER_TYPE>::initPoolSize(unsigned int _max_element_num)
{
    m_maxElementNum = _max_element_num;
    this->allocPoolSize(sizeof(BUFFER_TYPE) * m_maxElementNum);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
SPTR(data::timeline::object)
generic_tl<BUFFER_TYPE>::createObject(core::hires_clock::type _timestamp)
{
    return this->createBuffer(_timestamp);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
SPTR(typename generic_tl<BUFFER_TYPE>::buffer_t)
generic_tl<BUFFER_TYPE>::createBuffer(core::hires_clock::type _timestamp)
{
    SPTR(buffer_t) obj = std::make_shared<buffer_t>(
        m_maxElementNum,
        _timestamp,
        (data::timeline::buffer::buffer_data_t) m_pool->malloc(),
        m_pool->get_requested_size(),
        [object_ptr = m_pool](auto&& _p_h1, auto&& ...){object_ptr->free(std::forward<decltype(_p_h1)>(_p_h1));});
    return obj;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::isObjectValid(const CSPTR(data::timeline::object)& _obj) const
{
    CSPTR(buffer_t) src_obj = std::dynamic_pointer_cast<const buffer_t>(_obj);
    return src_obj != nullptr;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
unsigned int generic_tl<BUFFER_TYPE>::getMaxElementNum() const
{
    return m_maxElementNum;
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::operator==(const generic_tl& _other) const noexcept
{
    if(m_maxElementNum != _other.m_maxElementNum)
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

template<class BUFFER_TYPE>
bool generic_tl<BUFFER_TYPE>::operator!=(const generic_tl& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
