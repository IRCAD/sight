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

#pragma once

#include "data/config.hpp"
#include "data/TimeLine.hpp"
#include "data/timeline/Buffer.hpp"

#include <boost/array.hpp>
#include <boost/pool/poolfwd.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class DATA_CLASS_API BufferTL : public TimeLine
{
public:

    SIGHT_DECLARE_CLASS(BufferTL, Object);

    typedef core::HiResClock::HiResClockType TimestampType;
    typedef std::map<TimestampType, SPTR(timeline::Buffer)> TimelineType;
    typedef std::pair<TimestampType, SPTR(timeline::Buffer)> BufferPairType;
    typedef boost::pool<> PoolType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API BufferTL(Object::Key key);

    /// Destructor
    DATA_API virtual ~BufferTL();

    /// Check if the type of an object is compatible with this timeline
    DATA_API virtual bool isObjectValid(const CSPTR(timeline::Object)& obj) const = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param timestamp timestamp used to find the closest object
     * @param direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    DATA_API virtual CSPTR(timeline::Object) getClosestObject(
        core::HiResClock::HiResClockType timestamp,
        DirectionType direction = BOTH
    ) const override;

    /// Return the object matching the specified timestamp, returns NULL if object is not found
    DATA_API virtual CSPTR(timeline::Object) getObject(core::HiResClock::HiResClockType timestamp)
    const override;

    /// Clear the timeline
    DATA_API virtual void clearTimeline();

    /// Push a buffer to the timeline
    DATA_API void pushObject(const SPTR(timeline::Object)& obj) override;

    /// Remove a buffer to the timeline
    DATA_API SPTR(timeline::Object) popObject(TimestampType timestamp) override;

    /// Change a buffer timestamp to the timeline
    DATA_API void modifyTime(TimestampType timestamp, TimestampType newTimestamp) override;

    /// Change a buffer object to the specified timestamp
    DATA_API void setObject(TimestampType timestamp, const SPTR(timeline::Object)& obj) override;

    /// Return the last object in the timeline
    DATA_API CSPTR(timeline::Object) getNewerObject() const;

    /// Return the last timestamp in the timeline
    DATA_API core::HiResClock::HiResClockType getNewerTimestamp() const;

    /// Change the maximum size of the timeline
    void setMaximumSize(std::size_t maximumSize)
    {
        m_maximumSize = maximumSize;
    }

    /// Default Timeline Size
    DATA_API static const std::size_t s_DEFAULT_TIMELINE_MAX_SIZE;

    /// Return true if the pool is allocated
    bool isAllocated() const
    {
        return m_pool != nullptr;
    }

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const BufferTL& other) const noexcept;
    DATA_API bool operator!=(const BufferTL& other) const noexcept;
    /// @}

protected:

    /// Allocate the pool buffer.
    DATA_API void allocPoolSize(std::size_t size);

    ///Timeline
    TimelineType m_timeline;

    /// Pool of buffer
    SPTR(PoolType) m_pool;

    /// maximum size
    std::size_t m_maximumSize;
}; // class BufferTL

} // namespace sight::data
