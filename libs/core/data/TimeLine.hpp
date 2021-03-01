/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "data/timeline/Object.hpp"

#include <data/Object.hpp>

namespace sight::data
{
/**
 * @brief   This class defines the interface of TimeLine. Timeline is a collection of objects, each object being
 *          associated with a timestamp. It is intended to store lightweight objects.
 */

class DATA_CLASS_API TimeLine : public data::Object
{

public:
    SIGHT_DECLARE_CLASS(TimeLine, data::Object)

    typedef enum
    {
        PAST   = -1,
        BOTH   = 0,
        FUTURE = 1
    } DirectionType;

    typedef core::com::Signal< void (core::HiResClock::HiResClockType timestamp) > ObjectPushedSignalType;
    typedef core::com::Signal< void (core::HiResClock::HiResClockType timestamp) > ObjectRemovedSignalType;
    typedef core::com::Signal< void () > ObjectClearedSignalType;

    DATA_API static const core::com::Signals::SignalKeyType s_OBJECT_PUSHED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_OBJECT_REMOVED_SIG;
    DATA_API static const core::com::Signals::SignalKeyType s_CLEARED_SIG;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API TimeLine( data::Object::Key key );

    /// Destructor
    DATA_API virtual ~TimeLine();

    /// Push an object to the timeline
    DATA_API virtual void pushObject(const SPTR(data::timeline::Object)& obj) = 0;

    /// Removes an object from the timeline
    DATA_API virtual SPTR(data::timeline::Object) popObject(core::HiResClock::HiResClockType timestamp) = 0;

    /// modify an object timestamp
    DATA_API virtual void modifyTime(core::HiResClock::HiResClockType timestamp,
                                     core::HiResClock::HiResClockType newTimestamp) = 0;

    /// Change an object to the specified timestamp
    DATA_API virtual void setObject(core::HiResClock::HiResClockType timestamp,
                                    const SPTR(data::timeline::Object)& obj) = 0;

    /**
     * @brief Return a new data::timeline::Object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    DATA_API virtual SPTR(data::timeline::Object) createObject(core::HiResClock::HiResClockType timestamp)
        = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param timestamp timestamp used to find the closest object
     * @param direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    DATA_API virtual CSPTR(data::timeline::Object) getClosestObject(
        core::HiResClock::HiResClockType timestamp, DirectionType direction = BOTH)  const = 0;

    /// Return the object with the specified timestamp
    DATA_API virtual CSPTR(data::timeline::Object) getObject(core::HiResClock::HiResClockType timestamp)
    const = 0;

protected:

    /// Signal to emit when an object is pushed in the timeline.
    ObjectPushedSignalType::sptr m_sigObjectPushed;
    /// Signal to emit when an object is removed in the timeline.
    ObjectPushedSignalType::sptr m_sigObjectRemoved;

}; // class TimeLine

} // namespace sight::data
