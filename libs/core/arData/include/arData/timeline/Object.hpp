/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
