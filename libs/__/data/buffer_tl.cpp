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

void buffer_tl::allocPoolSize(std::size_t size)
{
    this->clearTimeline();

    SIGHT_ASSERT("Allocation size must be greater than 0", size > 0);
    m_pool = std::make_shared<PoolType>(size);
}

//------------------------------------------------------------------------------

void buffer_tl::pushObject(const SPTR(data::timeline::object)& obj)
{
    // This check is important for inherited classes
    SIGHT_ASSERT("Trying to push not compatible Object in the buffer_tl.", isObjectValid(obj));

    if(m_timeline.size() >= m_maximumSize)
    {
        auto begin = m_timeline.begin();
        m_timeline.erase(begin);
    }

    SPTR(data::timeline::buffer) srcObj = std::dynamic_pointer_cast<data::timeline::buffer>(obj);
    m_timeline.insert(TimelineType::value_type(obj->getTimestamp(), srcObj));
}

//------------------------------------------------------------------------------

SPTR(data::timeline::object) buffer_tl::popObject(TimestampType timestamp)
{
    const auto itFind = m_timeline.find(timestamp);

    // Check if timestamp exists
    SIGHT_ASSERT("Trying to erase not existing timestamp", itFind != m_timeline.end());

    SPTR(data::timeline::object) object = itFind->second;

    m_timeline.erase(itFind);

    return object;
}

//------------------------------------------------------------------------------

void buffer_tl::modifyTime(TimestampType timestamp, TimestampType newTimestamp)
{
    const auto itFind = m_timeline.find(timestamp);

    // Check if timestamp exists
    SIGHT_ASSERT("Trying to swap at non-existing timestamp", itFind != m_timeline.end());

    // Check if newTimestamp is not already used
    SIGHT_ASSERT("New timestamp already used by an other object", m_timeline.find(newTimestamp) == m_timeline.end());

    m_timeline.insert(TimelineType::value_type(newTimestamp, itFind->second));
    m_timeline.erase(itFind);
}

//------------------------------------------------------------------------------

void buffer_tl::set_object(TimestampType timestamp, const SPTR(data::timeline::object)& obj)
{
    // Check if timestamp exists
    SIGHT_ASSERT("Trying to set an object at non-existing timestamp", m_timeline.find(timestamp) != m_timeline.end());

    SPTR(data::timeline::buffer) srcObj = std::dynamic_pointer_cast<data::timeline::buffer>(obj);
    m_timeline[timestamp]               = srcObj;
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::object) buffer_tl::getClosestObject(
    core::hires_clock::type timestamp,
    timeline::direction_t direction
) const
{
    SPTR(data::timeline::buffer) result;
    if(m_timeline.empty())
    {
        return result;
    }

    auto iter =
        (direction == timeline::PAST) ? m_timeline.upper_bound(timestamp) : m_timeline.lower_bound(timestamp);
    if(iter != m_timeline.begin())
    {
        if(iter == m_timeline.end())
        {
            if(direction != timeline::FUTURE)
            {
                SPTR(data::timeline::buffer) previousObj = (--iter)->second;
                result                                   = previousObj;
            }
        }
        else
        {
            core::hires_clock::type nextTS = iter->first;
            SPTR(data::timeline::buffer) nextObj = iter->second;

            switch(direction)
            {
                case timeline::PAST:
                    result = (--iter)->second;
                    break;

                case timeline::BOTH:
                {
                    core::hires_clock::type previousTS = (--iter)->first;
                    SPTR(data::timeline::buffer) previousObj = iter->second;
                    result                                   =
                        ((nextTS - timestamp) > (timestamp - previousTS)) ? previousObj : nextObj;
                    break;
                }

                case timeline::FUTURE:
                    result = nextObj;
                    break;
            }
        }
    }
    else
    {
        if(direction != timeline::PAST)
        {
            SPTR(data::timeline::buffer) nextObj = iter->second;
            result                               = nextObj;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

CSPTR(data::timeline::object) buffer_tl::getObject(core::hires_clock::type timestamp) const
{
    SPTR(data::timeline::buffer) result;
    auto iter = m_timeline.find(timestamp);

    if(iter != m_timeline.end())
    {
        result = iter->second;
    }

    SIGHT_WARN_IF(
        "There is no object in the timeline matching the timestamp: " << timestamp << ".",
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

bool buffer_tl::operator==(const buffer_tl& other) const noexcept
{
    if(m_maximumSize != other.m_maximumSize
       || !core::tools::is_equal(m_timeline, other.m_timeline))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool buffer_tl::operator!=(const buffer_tl& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
