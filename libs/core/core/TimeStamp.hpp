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

#include "core/base.hpp"
#include "core/HiResClock.hpp"

namespace sight::core
{

/**
 * @brief Provides a timestamp and an expiry system.
 *
 */
class CORE_CLASS_API TimeStamp : public BaseObject
{
public:

    SIGHT_DECLARE_CLASS(TimeStamp, BaseObject, new TimeStamp);

    /**
     * @brief Type used in logical typestamp.
     */
    typedef core::HiResClock::HiResClockType TimeStampType;

    /**
     * @name Constructor/Destructor
     * @{ */

    CORE_API TimeStamp()
    {
        this->m_modifiedTime = 0;
        this->m_lifePeriod   = 0;
    }

    /**  @} */

    /**
     * @brief Update the timestamp to the current EPOCH time
     */
    CORE_API void modified();

    /**
     * @return TimeStamp's current value
     */
    CORE_API TimeStampType getTimeStamp() const
    {
        return this->m_modifiedTime;
    }

    /**
     * @brief Greater than operator for TimeStamp.
     *
     * @param ts TimeStamp to compare to
     */
    CORE_API bool operator>(const TimeStamp& ts) const
    {
        return this->m_modifiedTime > ts.m_modifiedTime;
    }

    /**
     * @brief Lesser than operator for TimeStamp.
     *
     * @param ts TimeStamp to compare to
     */
    CORE_API bool operator<(const TimeStamp& ts) const
    {
        return this->m_modifiedTime < ts.m_modifiedTime;
    }

    /**
     * @brief Cast operator for TimeStamp.
     */
    CORE_API operator TimeStampType() const
    {
        return this->m_modifiedTime;
    }

    /**
     * @brief Setter for the life period
     *
     * @param period Period in milliseconds
     */
    CORE_API void setLifePeriod(TimeStampType period)
    {
        m_lifePeriod = period;
    }

    /**
     * @brief Getter for the life period
     *
     * @return TimeStamp life period
     */
    CORE_API TimeStampType getLifePeriod() const
    {
        return m_lifePeriod;
    }

    /**
     * @brief Check TimeStamp expiry status
     *
     * @return True if more than m_lifePeriod milliseconds are elapsed from
     * TimeStamp value
     */
    CORE_API bool periodExpired() const
    {
        return (core::HiResClock::getTimeInMilliSec() - this->m_modifiedTime) > m_lifePeriod;
    }

private:

    /**
     * @brief TimeStamp value (milliseconds)
     */
    TimeStampType m_modifiedTime;

    /**
     * @brief life period (milliseconds)
     */
    TimeStampType m_lifePeriod;
};

} //namespace sight::core
