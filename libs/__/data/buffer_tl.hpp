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

#pragma once

#include "data/config.hpp"
#include "data/timeline/base.hpp"
#include "data/timeline/buffer.hpp"

#include <boost/array.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/poolfwd.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class DATA_CLASS_API buffer_tl : public timeline::base
{
public:

    SIGHT_DECLARE_CLASS(buffer_tl, timeline::base);

    typedef core::hires_clock::type timestamp_t;
    typedef std::map<timestamp_t, SPTR(timeline::buffer)> timeline_t;
    typedef std::pair<timestamp_t, SPTR(timeline::buffer)> buffer_pair_t;
    typedef boost::pool<> pool_t;

    DATA_API buffer_tl();
    DATA_API ~buffer_tl() override;

    /// Check if the type of an object is compatible with this timeline
    DATA_API virtual bool isObjectValid(const CSPTR(timeline::object)& _obj) const = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param timestamp timestamp used to find the closest object
     * @param direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    DATA_API CSPTR(timeline::object) getClosestObject(
        core::hires_clock::type _timestamp,
        timeline::direction_t _direction = timeline::BOTH
    ) const override;

    /// Return the object matching the specified timestamp, returns NULL if object is not found
    DATA_API CSPTR(timeline::object) getObject(core::hires_clock::type _timestamp)
    const override;

    /// Clear the timeline
    DATA_API virtual void clearTimeline();

    /// Push a buffer to the timeline
    DATA_API void pushObject(const SPTR(timeline::object)& _obj) override;

    /// Remove a buffer to the timeline
    DATA_API SPTR(timeline::object) popObject(timestamp_t _timestamp) override;

    /// Change a buffer timestamp to the timeline
    DATA_API void modifyTime(timestamp_t _timestamp, timestamp_t _new_timestamp) override;

    /// Change a buffer object to the specified timestamp
    DATA_API void set_object(timestamp_t _timestamp, const SPTR(timeline::object)& _obj) override;

    /// Return the last object in the timeline
    DATA_API CSPTR(timeline::object) getNewerObject() const;

    /// Return the last timestamp in the timeline
    DATA_API core::hires_clock::type getNewerTimestamp() const;

    /// Change the maximum size of the timeline
    void setMaximumSize(std::size_t _maximum_size)
    {
        m_maximumSize = _maximum_size;
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
    DATA_API bool operator==(const buffer_tl& _other) const noexcept;
    DATA_API bool operator!=(const buffer_tl& _other) const noexcept;
    /// @}

protected:

    /// Allocate the pool buffer.
    DATA_API void allocPoolSize(std::size_t _size);

    ///Timeline
    timeline_t m_timeline;

    /// Pool of buffer
    SPTR(pool_t) m_pool;

    /// maximum size
    std::size_t m_maximumSize;
}; // class buffer_tl

} // namespace sight::data
