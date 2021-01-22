/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "arData/BufferTL.hpp"

#include <core/com/Signal.hxx>

#include <boost/pool/pool.hpp>

#include <cmath>

namespace arData
{

const size_t BufferTL::s_DEFAULT_TIMELINE_MAX_SIZE = 1000;

//------------------------------------------------------------------------------

BufferTL::BufferTL ( ::fwData::Object::Key key ) :
    TimeLine(key),
    m_maximumSize(s_DEFAULT_TIMELINE_MAX_SIZE)
{
}

//------------------------------------------------------------------------------

BufferTL::~BufferTL ()
{
    this->clearTimeline();
}

//------------------------------------------------------------------------------

void BufferTL::allocPoolSize(std::size_t size)
{
    this->clearTimeline();
    core::mt::WriteLock lock(m_tlMutex);

    SLM_ASSERT( "Allocation size must be greater than 0", size > 0 );
    m_pool = std::make_shared< PoolType >(size);
}

//------------------------------------------------------------------------------

void BufferTL::pushObject(const SPTR(::arData::timeline::Object)& obj)
{
    // This check is important for inherited classes
    SLM_ASSERT("Trying to push not compatible Object in the BufferTL.", isObjectValid(obj));

    core::mt::WriteLock writeLock(m_tlMutex);
    if(m_timeline.size() >= m_maximumSize)
    {
        TimelineType::iterator begin = m_timeline.begin();
        m_timeline.erase(begin);
    }

    SPTR(::arData::timeline::Buffer) srcObj = std::dynamic_pointer_cast< ::arData::timeline::Buffer >(obj);
    m_timeline.insert(TimelineType::value_type(obj->getTimestamp(), srcObj));
}

//------------------------------------------------------------------------------

SPTR(::arData::timeline::Object) BufferTL::popObject(TimestampType timestamp)
{
    const auto itFind = m_timeline.find(timestamp);

    // Check if timestamp exists
    SLM_ASSERT("Trying to erase not existing timestamp", itFind != m_timeline.end());

    SPTR(::arData::timeline::Object) object = itFind->second;

    core::mt::WriteLock writeLock(m_tlMutex);

    m_timeline.erase(itFind);

    return object;
}

//------------------------------------------------------------------------------

void BufferTL::modifyTime(TimestampType timestamp, TimestampType newTimestamp)
{
    const auto itFind = m_timeline.find(timestamp);

    // Check if timestamp exists
    SLM_ASSERT("Trying to swap at non-existing timestamp", itFind != m_timeline.end());

    // Check if newTimestamp is not already used
    SLM_ASSERT("New timestamp already used by an other object", m_timeline.find(newTimestamp) == m_timeline.end());

    core::mt::WriteLock writeLock(m_tlMutex);

    m_timeline.insert(TimelineType::value_type(newTimestamp, itFind->second));
    m_timeline.erase(itFind);
}

//------------------------------------------------------------------------------

void BufferTL::setObject(TimestampType timestamp, const SPTR(::arData::timeline::Object)& obj)
{
    // Check if timestamp exists
    SLM_ASSERT("Trying to set an object at non-existing timestamp", m_timeline.find(timestamp) != m_timeline.end());

    core::mt::WriteLock writeLock(m_tlMutex);

    SPTR(::arData::timeline::Buffer) srcObj = std::dynamic_pointer_cast< ::arData::timeline::Buffer >(obj);
    m_timeline[timestamp]                   = srcObj;
}

//------------------------------------------------------------------------------

CSPTR(::arData::timeline::Object) BufferTL::getClosestObject(core::HiResClock::HiResClockType timestamp,
                                                             DirectionType direction) const
{
    core::mt::ReadLock readLock(m_tlMutex);
    SPTR(::arData::timeline::Buffer) result;
    if(m_timeline.empty())
    {
        return result;
    }

    TimelineType::const_iterator iter =
        (direction == PAST) ? m_timeline.upper_bound(timestamp) : m_timeline.lower_bound(timestamp);
    if (iter != m_timeline.begin())
    {
        if( iter == m_timeline.end() )
        {
            if (direction != FUTURE)
            {
                SPTR(::arData::timeline::Buffer) previousObj = (--iter)->second;
                result                                       = previousObj;
            }
        }
        else
        {
            core::HiResClock::HiResClockType nextTS = iter->first;
            SPTR(::arData::timeline::Buffer) nextObj = iter->second;

            switch ( direction )
            {
                case PAST:
                    result = (--iter)->second;
                    break;
                case BOTH:
                {
                    core::HiResClock::HiResClockType previousTS = (--iter)->first;
                    SPTR(::arData::timeline::Buffer) previousObj = iter->second;
                    result                                       =
                        ((nextTS - timestamp) > (timestamp - previousTS)) ? previousObj : nextObj;
                }
                break;
                case FUTURE:
                    result = nextObj;
                    break;
            }
        }
    }
    else
    {
        if (direction != PAST)
        {
            SPTR(::arData::timeline::Buffer) nextObj = iter->second;
            result                                   = nextObj;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

CSPTR(::arData::timeline::Object) BufferTL::getObject(core::HiResClock::HiResClockType timestamp) const
{
    core::mt::ReadLock readLock(m_tlMutex);
    SPTR(::arData::timeline::Buffer) result;
    TimelineType::const_iterator iter = m_timeline.find(timestamp);

    if(iter != m_timeline.end())
    {
        result = iter->second;
    }

    SLM_WARN_IF("There is no object in the timeline matching the timestamp: " << timestamp << ".",
                iter == m_timeline.end());

    return result;
}

//------------------------------------------------------------------------------

CSPTR(::arData::timeline::Object) BufferTL::getNewerObject() const
{
    SPTR(::arData::timeline::Object) result;

    core::mt::ReadLock readLock(m_tlMutex);
    if (!m_timeline.empty())
    {
        result = m_timeline.rbegin()->second;
    }
    return result;
}

//------------------------------------------------------------------------------

core::HiResClock::HiResClockType BufferTL::getNewerTimestamp() const
{
    core::HiResClock::HiResClockType result = 0;

    core::mt::ReadLock readLock(m_tlMutex);
    if (!m_timeline.empty())
    {
        result = m_timeline.rbegin()->first;
    }
    return result;
}

//------------------------------------------------------------------------------

void BufferTL::clearTimeline()
{
    core::mt::WriteLock writeLock(m_tlMutex);
    m_timeline.clear();

    auto sig = this->signal<ObjectClearedSignalType>(s_CLEARED_SIG);
    sig->asyncEmit();
}

} // namespace arData
