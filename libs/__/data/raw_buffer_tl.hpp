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

#include "data/buffer_tl.hpp"
#include "data/timeline/raw_buffer.hpp"

#include <data/factory/new.hpp>

namespace sight::data
{

/**
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class SIGHT_DATA_CLASS_API raw_buffer_tl : public buffer_tl
{
public:

    SIGHT_DECLARE_CLASS(raw_buffer_tl, timeline::base);

    /// Destructor
    inline ~raw_buffer_tl() noexcept override = default;

    /**
     * @brief Return the closest buffer to the given timestamp
     * @param _timestamp timestamp used to find the closest buffer
     * @param _direction direction to find the closest buffer (PAST, FUTURE, BOTH)
     */
    SIGHT_DATA_API virtual CSPTR(timeline::raw_buffer) get_closest_buffer(
        core::clock::type _timestamp,
        timeline::direction_t _direction = timeline::both
    ) const;

    /// Return the buffer matching the specified timestamp, returns NULL if object is not found
    SIGHT_DATA_API virtual CSPTR(timeline::raw_buffer) get_buffer(core::clock::type _timestamp)
    const;

    /// Initialize the size of the pool buffer.
    SIGHT_DATA_API void init_pool_size(std::size_t _size);

    /**
     * @brief Return a new timeline::object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    SIGHT_DATA_API SPTR(timeline::object) create_object(core::clock::type _timestamp) override;

    /**
     * @brief Return a new buffer_t with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    SIGHT_DATA_API SPTR(timeline::raw_buffer) create_buffer(core::clock::type _timestamp);

    /// Check if the type of an object is compatible with this timeline
    SIGHT_DATA_API bool is_object_valid(const CSPTR(timeline::object)& _obj) const override;

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;
}; // class raw_buffer_tl

} // namespace sight::data
