/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <sight/core/config.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace sight::core::tools
{

/**
 * @name Date and time
 * @{
 */

//------------------------------------------------------------------------------
/**
 * @brief       Convert a string date to a boost date
 * @param[in]   _date_str date in the string format YYYYMMDD (ie : 20090722)
 * @return      Date in the boost format
 */
SIGHT_CORE_API boost::gregorian::date str_to_boost_date(const std::string& _date_str);

//------------------------------------------------------------------------------

/**
 * @brief       Convert a string time to a boost time
 * @param[in]   _time_str time in the string format HHMMSS
 * @return      Time in the boost format
 */
SIGHT_CORE_API boost::posix_time::time_duration str_to_boost_time(const std::string& _time_str);

//------------------------------------------------------------------------------

/**
 * @brief       Convert string data time to a boost time
 * @param[in]   _date_str date in the string format YYYYMMDD (ie : 20090722)
 * @param[in]   _time_str time in the string format HHMMSS (by default : "000000")
 * @return      Time in the boost format
 */
SIGHT_CORE_API boost::posix_time::ptime str_to_boost_date_and_time(
    const std::string& _date_str,
    const std::string& _time_str = "000000"
);

//------------------------------------------------------------------------------

/**
 * @brief       Convert a boost time to a string date
 * @param[in]   _date_and_time time in the boost format
 * @return      Date in the string format YYYYMMDD
 */
SIGHT_CORE_API std::string get_date(const boost::posix_time::ptime& _date_and_time);

//------------------------------------------------------------------------------

// HHMMSS
/**
 * @brief       Convert a boost time to a string time
 * @param[in]   _date_and_time time in the boost format
 * @return      Time in the string format HHMMSS
 */
SIGHT_CORE_API std::string get_time(const boost::posix_time::ptime& _date_and_time);

//------------------------------------------------------------------------------

/**
 * @brief       Return the current clock
 * @return      Time in the string format YYYY-mmm-DD HH:MM:SS
 */
SIGHT_CORE_API std::string get_current_time();

///@}

} // namespace sight::core::tools
