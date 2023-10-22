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

#include "data/buffer_tl.hpp"
#include "data/config.hpp"
#include "data/timeline/generic_object.hpp"

namespace sight::data
{

/**
 * @brief   This class defines a timeline of template objects. All objects have the same maximum number of elements,
 *          nevertheless it is not mandatory to provide all elements.
 */
template<class BUFFER_TYPE>
class generic_tl : public buffer_tl
{
public:

    SIGHT_DECLARE_CLASS(generic_tl<BUFFER_TYPE>, buffer_tl);

    typedef timeline::generic_object<BUFFER_TYPE> buffer_t;

    generic_tl();
    ~generic_tl() override = default;

    /**
     * @brief Returns the closest buffer to the given timestamp
     * @param timestamp timestamp used to find the closest buffer
     * @param direction direction to find the closest buffer (PAST, FUTURE, BOTH)
     */
    CSPTR(buffer_t) getClosestBuffer(
        core::hires_clock::type _timestamp,
        timeline::direction_t _direction = timeline::BOTH
    ) const;

    /// Returns the buffer matching the specified timestamp, returns NULL if object is not found
    CSPTR(buffer_t) get_buffer(core::hires_clock::type _timestamp) const;

    /// Initializes the size of the pool buffer.
    virtual void initPoolSize(unsigned int _max_element_num);

    /**
     * @brief Returns a new timeline::object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    SPTR(timeline::object) createObject(core::hires_clock::type _timestamp) override;

    /**
     * @brief Returns a new buffer_t with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    SPTR(buffer_t) createBuffer(core::hires_clock::type _timestamp);

    /// Check if the type of an object is compatible with this timeline
    bool isObjectValid(const CSPTR(timeline::object)& _obj) const override;

    /// Get/set the maximum number of objects inside a single buffer
    unsigned int getMaxElementNum() const;

    /// Equality comparison operators
    /// @{
    bool operator==(const generic_tl& _other) const noexcept;
    bool operator!=(const generic_tl& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// maximum number of elements inside a single buffer
    unsigned int m_maxElementNum;
}; // class generic_tl

} // namespace sight::data
