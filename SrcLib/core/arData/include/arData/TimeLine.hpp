/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_TIMELINE_HPP__
#define __ARDATA_TIMELINE_HPP__

#include "arData/config.hpp"
#include "arData/timeline/Object.hpp"

#include <fwData/Object.hpp>

namespace arData
{
/**
 * @brief   This class defines the interface of TimeLine. Timeline is a collection of objects, each object being
 *          associated with a timestamp. It is intended to store lightweight objects.
 */

class ARDATA_CLASS_API TimeLine : public ::fwData::Object
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (TimeLine) (::fwData::Object) );

    typedef enum
    {
        PAST   = -1,
        BOTH   = 0,
        FUTURE = 1
    } DirectionType;

    typedef ::fwCom::Signal< void (::fwCore::HiResClock::HiResClockType timestamp) > ObjectPushedSignalType;
    typedef ::fwCom::Signal< void (::fwCore::HiResClock::HiResClockType timestamp) > ObjectRemovedSignalType;
    typedef ::fwCom::Signal< void () > ObjectClearedSignalType;

    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_OBJECT_PUSHED_SIG;
    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_OBJECT_REMOVED_SIG;
    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_CLEARED_SIG;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API TimeLine( ::fwData::Object::Key key );

    /// Destructor
    ARDATA_API virtual ~TimeLine();

    /// Push an object to the timeline
    ARDATA_API virtual void pushObject(const SPTR(::arData::timeline::Object)& obj) = 0;

    /// Removes an object from the timeline
    ARDATA_API virtual SPTR(::arData::timeline::Object) popObject(::fwCore::HiResClock::HiResClockType timestamp) = 0;

    /// modify an object timestamp
    ARDATA_API virtual void modifyTime(::fwCore::HiResClock::HiResClockType timestamp,
                                       ::fwCore::HiResClock::HiResClockType newTimestamp) = 0;

    /// Change an object to the specified timestamp
    ARDATA_API virtual void setObject(::fwCore::HiResClock::HiResClockType timestamp,
                                      const SPTR(::arData::timeline::Object)& obj) = 0;

    /**
     * @brief Return a new ::arData::timeline::Object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    ARDATA_API virtual SPTR(::arData::timeline::Object) createObject(::fwCore::HiResClock::HiResClockType timestamp)
        = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param timestamp timestamp used to find the closest object
     * @param direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    ARDATA_API virtual CSPTR(::arData::timeline::Object) getClosestObject(
        ::fwCore::HiResClock::HiResClockType timestamp, DirectionType direction = BOTH)  const = 0;

    /// Return the object with the specified timestamp
    ARDATA_API virtual CSPTR(::arData::timeline::Object) getObject(::fwCore::HiResClock::HiResClockType timestamp)
    const = 0;

protected:

    /// Signal to emit when an object is pushed in the timeline.
    ObjectPushedSignalType::sptr m_sigObjectPushed;
    /// Signal to emit when an object is removed in the timeline.
    ObjectPushedSignalType::sptr m_sigObjectRemoved;

}; // class TimeLine

} // namespace arData

#endif // __ARDATA_TIMELINE_HPP__
