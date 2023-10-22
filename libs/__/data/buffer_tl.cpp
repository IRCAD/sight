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

const std::size_t buffer_tl::s_DEFAULT_TIMELINE_MAX_SIZE = 1000;

//------------------------------------------------------------------------------

buffer_tl::buffer_tl() :
    m_maximumSize(s_DEFAULT_TIMELINE_MAX_SIZE)
{
}

//------------------------------------------------------------------------------

buffer_tl::~buffer_tl()
{
    this->buffer_tl::clearTimeline();
}

//------------------------------------------------------------------------------

void buffer_tl::allocPoolSize(std::size_t _size)
{
    this->clearTimeline();

    SIGHT_ASSERT("Allocation size must be greater than 0", _size > 0);
    m_pool = std::make_shared<pool_t>(_size);
}

//------------------------------------------------------------------------------

void buffer_tl::pushObject(const SPTR(data::timeline::object)& _obj)
{
    // This check is important for inherited classes
    SIGHT_ASSERT("Trying to push not compatible Object in the buffer_tl.", isObjectValid(_obj));

    if(m_timeline.size() >= m_maximumSize)
    {
        auto begin = m_timeline.begin();
        m_timeline.erase(begin);
    }

    SPTR(data::timeline::buffer) src_obj = std::dynamic_pointer_cast<data::timeline::buffer>(_obj);
    m_timeline.insert(timeline_t::value_type(_obj->getTimestamp(), src_obj));
}

//------------------------------------------------------------------------------

SPTR(data::timeline::object) buffer_tl::popObject(timestamp_t _timestamp)
{
    const auto it_find = m_timeline.find(_timestamp);

    // Check if timestamp exists
    SIGHT_ASSERT("Trying to erase not existing timestamp", it_find != m_timeline.end());

    SPTR(data::timeline::object) object = it_find->second;

    m_timeline.erase(it_find);

    return object;
}

//------------------------------------------------------------------------------

void buffer_tl::modifyTime(timestamp_t _timestamp, timestamp_t _new_timestamp)
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

CSPTR(data::timeline::object) buffer_tl::getClosestObject(
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
        (_direction == timeline::PAST) ? m_timeline.upper_bound(_timestamp) : m_timeline.lower_bound(_timestamp);
    if(iter != m_timeline.begin())
    {
        if(iter == m_timeline.end())
        {
            if(_direction != timeline::FUTURE)
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
                case timeline::PAST:
                    result = (--iter)->second;
                    break;

                case timeline::BOTH:
                {
                    core::hires_clock::type previous_ts = (--iter)->first;
                    SPTR(data::timeline::buffer) previous_obj = iter->second;
                    result                                    =
                        ((next_ts - _timestamp) > (_timestamp - previous_ts)) ? previous_obj : next_obj;
                    break;
                }

                case timeline::FUTURE:
                    result = next_obj;
                    break;
            }
        }
    }
    else
    {
        if(_direction != timeline::PAST)
        {
            SPTR(data::timeline::buffer) next_obj = iter->second;
            result                                = next_obj;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::object) buffer_tl::getObject(core::hires_clock::type _timestamp) const
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

CSPTR(data::timeline::object) buffer_tl::getNewerObject() const
{
    SPTR(data::timeline::object) result;

    if(!m_timeline.empty())
    {
        result = m_timeline.rbegin()->second;
    }

    return result;
}

//------------------------------------------------------------------------------

core::hires_clock::type buffer_tl::getNewerTimestamp() const
{
    core::hires_clock::type result = 0;

    if(!m_timeline.empty())
    {
        result = m_timeline.rbegin()->first;
    }

    return result;
}

//------------------------------------------------------------------------------

void buffer_tl::clearTimeline()
{
    m_timeline.clear();

    auto sig = this->signal<timeline::signals::cleared_t>(timeline::signals::CLEARED);
    sig->async_emit();
}

//------------------------------------------------------------------------------

bool buffer_tl::operator==(const buffer_tl& _other) const noexcept
{
    if(m_maximumSize != _other.m_maximumSize
       || !core::tools::is_equal(m_timeline, _other.m_timeline))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool buffer_tl::operator!=(const buffer_tl& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
