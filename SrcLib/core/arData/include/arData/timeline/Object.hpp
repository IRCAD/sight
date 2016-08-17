/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_TIMELINE_OBJECT_HPP__
#define __ARDATA_TIMELINE_OBJECT_HPP__

#include <fwCore/mt/types.hpp>
#include <fwCore/HiResClock.hpp>
#include <fwData/Object.hpp>

#include "arData/config.hpp"

namespace arData
{

namespace timeline
{

/**
 * @brief   This class defines a TimeLine object. Such an object is intended to be managed by a Timeline implementation.
 *          Use Timeline::createObject() to allocate a new Object.
 */
class ARDATA_CLASS_API Object
{
public:
    /// Constructor
    ARDATA_API Object(::fwCore::HiResClock::HiResClockType timestamp = 0);

    /// Destructor
    ARDATA_API virtual ~Object();

    /// Define deep copy
    ARDATA_API virtual void deepCopy( const Object& _source );

    /// Return timestamp
    ::fwCore::HiResClock::HiResClockType getTimestamp() const
    {
        return m_timestamp;
    }

protected:

    /// Timestamp
    ::fwCore::HiResClock::HiResClockType m_timestamp;
};

} // namespace timeline

} // namespace arData

#endif /* __ARDATA_TIMELINE_OBJECT_HPP__ */
