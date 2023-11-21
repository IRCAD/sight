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

#include "data/config.hpp"
#include "data/object.hpp"
#include "data/timeline/object.hpp"

namespace sight::data::timeline
{

enum direction_t
{
    past   = -1,
    both   = 0,
    future = 1
};

struct signals
{
    using pushed_t  = core::com::signal<void (core::clock::type _timestamp)>;
    using removed_t = core::com::signal<void (core::clock::type _timestamp)>;
    using cleared_t = core::com::signal<void ()>;

    inline static const core::com::signals::key_t PUSHED  = "object_pushed";
    inline static const core::com::signals::key_t REMOVED = "objectRemoved";
    inline static const core::com::signals::key_t CLEARED = "objectCleared";
};

/**
 * @brief   This class defines the interface of base. Timeline is a collection of objects, each object being
 *          associated with a timestamp. It is intended to store lightweight objects.
 */

class DATA_CLASS_API base : public data::object
{
public:

    SIGHT_DECLARE_CLASS(base, data::object);

    DATA_API base();
    DATA_API ~base() override = default;

    /// Push an object to the base
    DATA_API virtual void push_object(const SPTR(timeline::object)& _obj) = 0;

    /// Removes an object from the base
    DATA_API virtual SPTR(timeline::object) pop_object(core::clock::type _timestamp) = 0;

    /// modify an object timestamp
    DATA_API virtual void modify_time(
        core::clock::type _timestamp,
        core::clock::type _new_timestamp
    ) = 0;

    /// Change an object to the specified timestamp
    DATA_API virtual void set_object(
        core::clock::type _timestamp,
        const SPTR(timeline::object)& _obj
    ) = 0;

    /**
     * @brief Return a new timeline::object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the base. You must call pushObject() to register it.
     */
    DATA_API virtual SPTR(timeline::object) create_object(core::clock::type _timestamp) = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param timestamp timestamp used to find the closest object
     * @param direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    DATA_API virtual CSPTR(timeline::object) get_closest_object(
        core::clock::type _timestamp,
        direction_t _direction = both
    ) const = 0;

    /// Return the object with the specified timestamp
    DATA_API virtual CSPTR(timeline::object) get_object(core::clock::type _timestamp) const = 0;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const base& _other) const noexcept;
    DATA_API bool operator!=(const base& _other) const noexcept;
    /// @}

protected:

    /// Signal to emit when an object is pushed in the base.
    signals::pushed_t::sptr m_sig_object_pushed;
    /// Signal to emit when an object is removed in the base.
    signals::pushed_t::sptr m_sig_object_removed;
}; // class base

} // namespace sight::data::timeline
