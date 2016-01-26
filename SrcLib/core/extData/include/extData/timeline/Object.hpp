/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_TIMELINE_OBJECT_HPP__
#define __EXTDATA_TIMELINE_OBJECT_HPP__

#include <fwCore/mt/types.hpp>
#include <fwCore/HiResClock.hpp>
#include <fwData/Object.hpp>

#include "extData/config.hpp"

namespace extData
{

namespace timeline
{

/**
 * @class   Object
 * @brief   This class defines a TimeLine object. Such an object is intended to be managed by a Timeline implementation.
 *          Use Timeline::createObject() to allocate a new Object.
 */
class EXTDATA_CLASS_API Object
{
public:
    /// Constructor
    EXTDATA_API Object(::fwCore::HiResClock::HiResClockType timestamp = 0);

    /// Destructor
    EXTDATA_API virtual ~Object();

    /// Define deep copy
    EXTDATA_API virtual void deepCopy( const Object& _source );

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

} // namespace extData

#endif /* __EXTDATA_TIMELINE_OBJECT_HPP__ */
