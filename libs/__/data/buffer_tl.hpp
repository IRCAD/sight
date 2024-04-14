/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

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
class SIGHT_DATA_CLASS_API buffer_tl : public timeline::base
{
public:

    SIGHT_DECLARE_CLASS(buffer_tl, timeline::base);

    using timestamp_t   = core::clock::type;
    using timeline_t    = std::map<timestamp_t, std::shared_ptr<timeline::buffer> >;
    using buffer_pair_t = std::pair<timestamp_t, std::shared_ptr<timeline::buffer> >;
    using pool_t        = boost::pool<>;

    SIGHT_DATA_API buffer_tl();
    SIGHT_DATA_API ~buffer_tl() override;

    /// Check if the type of an object is compatible with this timeline
    SIGHT_DATA_API virtual bool is_object_valid(const CSPTR(timeline::object)& _obj) const = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param _timestamp timestamp used to find the closest object
     * @param _direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    SIGHT_DATA_API CSPTR(timeline::object) get_closest_object(
        core::clock::type _timestamp,
        timeline::direction_t _direction = timeline::both
    ) const override;

    /// Return the object matching the specified timestamp, returns NULL if object is not found
    SIGHT_DATA_API CSPTR(timeline::object) get_object(core::clock::type _timestamp)
    const override;

    /// Clear the timeline
    SIGHT_DATA_API virtual void clear_timeline();

    /// Push a buffer to the timeline
    SIGHT_DATA_API void push_object(const SPTR(timeline::object)& _obj) override;

    /// Remove a buffer to the timeline
    SIGHT_DATA_API SPTR(timeline::object) pop_object(timestamp_t _timestamp) override;

    /// Change a buffer timestamp to the timeline
    SIGHT_DATA_API void modify_time(timestamp_t _timestamp, timestamp_t _new_timestamp) override;

    /// Change a buffer object to the specified timestamp
    SIGHT_DATA_API void set_object(timestamp_t _timestamp, const SPTR(timeline::object)& _obj) override;

    /// Return the last object in the timeline
    SIGHT_DATA_API CSPTR(timeline::object) get_newer_object() const;

    /// Return the last timestamp in the timeline
    SIGHT_DATA_API core::clock::type get_newer_timestamp() const;

    /// Change the maximum size of the timeline
    void set_maximum_size(std::size_t _maximum_size)
    {
        m_maximum_size = _maximum_size;
    }

    /// Default Timeline Size
    SIGHT_DATA_API static const std::size_t DEFAULT_TIMELINE_MAX_SIZE;

    /// Return true if the pool is allocated
    bool is_allocated() const
    {
        return m_pool != nullptr;
    }

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const buffer_tl& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const buffer_tl& _other) const noexcept;
    /// @}

protected:

    /// Allocate the pool buffer.
    SIGHT_DATA_API void alloc_pool_size(std::size_t _size);

    ///Timeline
    timeline_t m_timeline;

    /// Pool of buffer
    SPTR(pool_t) m_pool;

    /// maximum size
    std::size_t m_maximum_size;
}; // class buffer_tl

} // namespace sight::data
