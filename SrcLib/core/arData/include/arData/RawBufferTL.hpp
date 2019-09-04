/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "arData/BufferTL.hpp"
#include "arData/config.hpp"
#include "arData/timeline/RawBuffer.hpp"

#include <fwData/factory/new.hpp>

fwCampAutoDeclareDataMacro((arData)(RawBufferTL), ARDATA_API);

namespace arData
{
/**
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class ARDATA_CLASS_API RawBufferTL : public BufferTL
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (RawBufferTL)(::arData::TimeLine),
                                            ::fwData::factory::New< RawBufferTL >);
    fwCampMakeFriendDataMacro((arData)(RawBufferTL));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API RawBufferTL( ::fwData::Object::Key key );

    /// Destructor
    ARDATA_API virtual ~RawBufferTL();

    /// Defines deep copy
    ARDATA_API virtual void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Return the closest buffer to the given timestamp
     * @param timestamp timestamp used to find the closest buffer
     * @param direction direction to find the closest buffer (PAST, FUTURE, BOTH)
     */
    ARDATA_API virtual CSPTR(::arData::timeline::RawBuffer) getClosestBuffer(
        ::fwCore::HiResClock::HiResClockType timestamp, DirectionType direction = BOTH) const;

    /// Return the buffer matching the specified timestamp, returns NULL if object is not found
    ARDATA_API virtual CSPTR(::arData::timeline::RawBuffer) getBuffer(::fwCore::HiResClock::HiResClockType timestamp)
    const;

    /// Initialize the size of the pool buffer.
    ARDATA_API void initPoolSize(std::size_t size);

    /**
     * @brief Return a new ::arData::timeline::Object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    ARDATA_API SPTR(::arData::timeline::Object) createObject(::fwCore::HiResClock::HiResClockType timestamp) override;

    /**
     * @brief Return a new BufferType with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    ARDATA_API SPTR(::arData::timeline::RawBuffer) createBuffer(::fwCore::HiResClock::HiResClockType timestamp);

    /// Check if the type of an object is compatible with this timeline
    ARDATA_API virtual bool isObjectValid(const CSPTR(::arData::timeline::Object)& obj) const override;

}; // class RawBufferTL

} // namespace arData
