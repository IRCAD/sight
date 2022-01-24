/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/config.hpp"

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
 * @param[in]   dateStr date in the string format YYYYMMDD (ie : 20090722)
 * @return      Date in the boost format
 */
CORE_API boost::gregorian::date strToBoostDate(const std::string& dateStr);

//------------------------------------------------------------------------------

/**
 * @brief       Convert a string time to a boost time
 * @param[in]   timeStr time in the string format HHMMSS
 * @return      Time in the boost format
 */
CORE_API boost::posix_time::time_duration strToBoostTime(const std::string& timeStr);

//------------------------------------------------------------------------------

/**
 * @brief       Convert string data time to a boost time
 * @param[in]   dateStr date in the string format YYYYMMDD (ie : 20090722)
 * @param[in]   timeStr time in the string format HHMMSS (by default : "000000")
 * @return      Time in the boost format
 */
CORE_API boost::posix_time::ptime strToBoostDateAndTime(
    const std::string& dateStr,
    const std::string& timeStr = "000000"
);

//------------------------------------------------------------------------------

/**
 * @brief       Convert a boost time to a string date
 * @param[in]   dateAndTime time in the boost format
 * @return      Date in the string format YYYYMMDD
 */
CORE_API std::string getDate(const boost::posix_time::ptime& dateAndTime);

//------------------------------------------------------------------------------

// HHMMSS
/**
 * @brief       Convert a boost time to a string time
 * @param[in]   dateAndTime time in the boost format
 * @return      Time in the string format HHMMSS
 */
CORE_API std::string getTime(const boost::posix_time::ptime& dateAndTime);

//------------------------------------------------------------------------------

/**
 * @brief       Return the current clock
 * @return      Time in the string format YYYY-mmm-DD HH:MM:SS
 */
CORE_API std::string getCurrentTime();

///@}

}
