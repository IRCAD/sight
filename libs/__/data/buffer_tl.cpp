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

#include "data/buffer_tl.hpp"

#include <core/com/signal.hxx>

#include <cmath>

namespace sight::data
{

const std::size_t buffer_tl::DEFAULT_TIMELINE_MAX_SIZE = 1000;

//------------------------------------------------------------------------------

buffer_tl::buffer_tl() :
    m_maximum_size(DEFAULT_TIMELINE_MAX_SIZE)
{
}

//------------------------------------------------------------------------------

buffer_tl::~buffer_tl()
{
    this->buffer_tl::clear_timeline();
}

//------------------------------------------------------------------------------

void buffer_tl::alloc_pool_size(std::size_t _size)
{
    this->clear_timeline();

    SIGHT_ASSERT("Allocation size must be greater than 0", _size > 0);
    m_pool = std::make_shared<pool_t>(_size);
}

//------------------------------------------------------------------------------

void buffer_tl::push_object(const SPTR(data::timeline::object)& _obj)
{
    // This check is important for inherited classes
    SIGHT_ASSERT("Trying to push not compatible Object in the buffer_tl.", is_object_valid(_obj));

    if(m_timeline.size() >= m_maximum_size)
    {
        auto begin = m_timeline.begin();
        m_timeline.erase(begin);
    }

    SPTR(data::timeline::buffer) src_obj = std::dynamic_pointer_cast<data::timeline::buffer>(_obj);
    m_timeline.insert(timeline_t::value_type(_obj->get_timestamp(), src_obj));
}

//------------------------------------------------------------------------------

SPTR(data::timeline::object) buffer_tl::pop_object(timestamp_t _timestamp)
{
    const auto it_find = m_timeline.find(_timestamp);

    // Check if timestamp exists
    SIGHT_ASSERT("Trying to erase not existing timestamp", it_find != m_timeline.end());

    SPTR(data::timeline::object) object = it_find->second;

    m_timeline.erase(it_find);

    return object;
}

//------------------------------------------------------------------------------

void buffer_tl::modify_time(timestamp_t _timestamp, timestamp_t _new_timestamp)
{
    const auto it_find = m_timeline.find(_timestamp);

    // Check if timestamp exists
    SIGHT_ASSERT("Trying to swap at non-existing timestamp", it_find != m_timeline.end());

    // Check if newTimestamp is not already used
    SIGHT_ASSERT("New timestamp already used by an other object", m_timeline.find(_new_timestamp) == m_timeline.end());

    m_timeline.insert(timeline_t::value_type(_new_timestamp, it_find->second));
    m_timeline.erase(it_find);
}

//------------------------------------------------------------------------------

void buffer_tl::set_object(timestamp_t _timestamp, const SPTR(data::timeline::object)& _obj)
{
    // Check if timestamp exists
    SIGHT_ASSERT("Trying to set an object at non-existing timestamp", m_timeline.find(_timestamp) != m_timeline.end());

    SPTR(data::timeline::buffer) src_obj = std::dynamic_pointer_cast<data::timeline::buffer>(_obj);
    m_timeline[_timestamp]               = src_obj;
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::object) buffer_tl::get_closest_object(
    core::hires_clock::type _timestamp,
    timeline::direction_t _direction
) const
{
    SPTR(data::timeline::buffer) result;
    if(m_timeline.empty())
    {
        return result;
    }

    auto iter =
        (_direction == timeline::past) ? m_timeline.upper_bound(_timestamp) : m_timeline.lower_bound(_timestamp);
    if(iter != m_timeline.begin())
    {
        if(iter == m_timeline.end())
        {
            if(_direction != timeline::future)
            {
                SPTR(data::timeline::buffer) previous_obj = (--iter)->second;
                result                                    = previous_obj;
            }
        }
        else
        {
            core::hires_clock::type next_ts = iter->first;
            SPTR(data::timeline::buffer) next_obj = iter->second;

            switch(_direction)
            {
                case timeline::past:
                    result = (--iter)->second;
                    break;

                case timeline::both:
                {
                    core::hires_clock::type previous_ts = (--iter)->first;
                    SPTR(data::timeline::buffer) previous_obj = iter->second;
                    result                                    =
                        ((next_ts - _timestamp) > (_timestamp - previous_ts)) ? previous_obj : next_obj;
                    break;
                }

                case timeline::future:
                    result = next_obj;
                    break;
            }
        }
    }
    else
    {
        if(_direction != timeline::past)
        {
            SPTR(data::timeline::buffer) next_obj = iter->second;
            result                                = next_obj;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::object) buffer_tl::get_object(core::hires_clock::type _timestamp) const
{
    SPTR(data::timeline::buffer) result;
    auto iter = m_timeline.find(_timestamp);

    if(iter != m_timeline.end())
    {
        result = iter->second;
    }

    SIGHT_WARN_IF(
        "There is no object in the timeline matching the timestamp: " << _timestamp << ".",
        iter == m_timeline.end()
    );

    return result;
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::object) buffer_tl::get_newer_object() const
{
    SPTR(data::timeline::object) result;

    if(!m_timeline.empty())
    {
        result = m_timeline.rbegin()->second;
    }

    return result;
}

//------------------------------------------------------------------------------

core::hires_clock::type buffer_tl::get_newer_timestamp() const
{
    core::hires_clock::type result = 0;

    if(!m_timeline.empty())
    {
        result = m_timeline.rbegin()->first;
    }

    return result;
}

//------------------------------------------------------------------------------

void buffer_tl::clear_timeline()
{
    m_timeline.clear();

    auto sig = this->signal<timeline::signals::cleared_t>(timeline::signals::CLEARED);
    sig->async_emit();
}

//------------------------------------------------------------------------------

bool buffer_tl::operator==(const buffer_tl& _other) const noexcept
{
    if(m_maximum_size != _other.m_maximum_size
       || !core::tools::is_equal(m_timeline, _other.m_timeline))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool buffer_tl::operator!=(const buffer_tl& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
