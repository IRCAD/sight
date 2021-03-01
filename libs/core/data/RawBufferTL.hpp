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

#pragma once

#include "data/BufferTL.hpp"
#include "data/config.hpp"
#include "data/timeline/RawBuffer.hpp"

#include <data/factory/new.hpp>

SIGHT_DECLARE_DATA_REFLECTION((sight)(data)(RawBufferTL));

namespace sight::data
{
/**
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class DATA_CLASS_API RawBufferTL : public BufferTL
{

public:
    SIGHT_DECLARE_CLASS(RawBufferTL, data::TimeLine, data::factory::New< RawBufferTL >)
    SIGHT_MAKE_FRIEND_REFLECTION((sight)(data)(RawBufferTL));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API RawBufferTL( data::Object::Key key );

    /// Destructor
    DATA_API virtual ~RawBufferTL();

    /// Defines deep copy
    DATA_API virtual void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Return the closest buffer to the given timestamp
     * @param timestamp timestamp used to find the closest buffer
     * @param direction direction to find the closest buffer (PAST, FUTURE, BOTH)
     */
    DATA_API virtual CSPTR(data::timeline::RawBuffer) getClosestBuffer(
        core::HiResClock::HiResClockType timestamp, DirectionType direction = BOTH) const;

    /// Return the buffer matching the specified timestamp, returns NULL if object is not found
    DATA_API virtual CSPTR(data::timeline::RawBuffer) getBuffer(core::HiResClock::HiResClockType timestamp)
    const;

    /// Initialize the size of the pool buffer.
    DATA_API void initPoolSize(std::size_t size);

    /**
     * @brief Return a new data::timeline::Object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    DATA_API SPTR(data::timeline::Object) createObject(core::HiResClock::HiResClockType timestamp) override;

    /**
     * @brief Return a new BufferType with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    DATA_API SPTR(data::timeline::RawBuffer) createBuffer(core::HiResClock::HiResClockType timestamp);

    /// Check if the type of an object is compatible with this timeline
    DATA_API virtual bool isObjectValid(const CSPTR(data::timeline::Object)& obj) const override;

}; // class RawBufferTL

} // namespace sight::data
