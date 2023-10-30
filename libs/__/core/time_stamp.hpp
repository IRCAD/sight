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

#include "core/base.hpp"
#include "core/hires_clock.hpp"

namespace sight::core
{

/**
 * @brief Provides a timestamp and an expiry system.
 *
 */
class CORE_CLASS_API time_stamp : public base_object
{
public:

    SIGHT_DECLARE_CLASS(time_stamp, base_object);

    /**
     * @brief Type used in logical typestamp.
     */
    using time_stamp_type = core::hires_clock::type;

    /**
     * @brief Update the timestamp to the current EPOCH time
     */
    CORE_API void modified();

    /**
     * @return TimeStamp's current value
     */
    CORE_API time_stamp_type get_time_stamp() const
    {
        return this->m_modified_time;
    }

    /**
     * @brief Greater than operator for TimeStamp.
     *
     * @param ts TimeStamp to compare to
     */
    CORE_API bool operator>(const time_stamp& _ts) const
    {
        return this->m_modified_time > _ts.m_modified_time;
    }

    /**
     * @brief Lesser than operator for TimeStamp.
     *
     * @param ts TimeStamp to compare to
     */
    CORE_API bool operator<(const time_stamp& _ts) const
    {
        return this->m_modified_time < _ts.m_modified_time;
    }

    /**
     * @brief Cast operator for TimeStamp.
     */
    CORE_API operator time_stamp_type() const
    {
        return this->m_modified_time;
    }

    /**
     * @brief Setter for the life period
     *
     * @param period Period in milliseconds
     */
    CORE_API void set_life_period(time_stamp_type _period)
    {
        m_life_period = _period;
    }

    /**
     * @brief Getter for the life period
     *
     * @return TimeStamp life period
     */
    CORE_API time_stamp_type get_life_period() const
    {
        return m_life_period;
    }

    /**
     * @brief Check TimeStamp expiry status
     *
     * @return True if more than m_lifePeriod milliseconds are elapsed from
     * TimeStamp value
     */
    CORE_API bool period_expired() const
    {
        return (core::hires_clock::get_time_in_milli_sec() - this->m_modified_time) > m_life_period;
    }

private:

    /**
     * @brief TimeStamp value (milliseconds)
     */
    time_stamp_type m_modified_time {0};

    /**
     * @brief life period (milliseconds)
     */
    time_stamp_type m_life_period {0};
};

} //namespace sight::core
