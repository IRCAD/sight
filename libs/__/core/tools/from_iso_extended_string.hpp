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

#include <string>

namespace sight::core::tools
{

/**
 * @name Date and time in iso extended format
 * @{
 */

/**
 * @brief       Construct a boost ptime from a string in iso extended format (YYYY-MM-DDTHH:MM:SS)
 * @param[in]   _time_string time in the iso extended format YYYY-MM-DDTHH:MM:SS (example : 2007-02-25T00:00:00 )
 * @return      Time in the boost format
 */
SIGHT_CORE_API boost::posix_time::ptime from_iso_extended_string(const std::string& _time_string);

/**
 * @brief   Construct a boost ptime from a string in xsd format (YYYY-MM-DD)
 * @param   _date_string date in the xsd format YYYY-MM-DD
 * @return  Time in the boost format
 */
SIGHT_CORE_API boost::posix_time::ptime from_date_in_xsd_format(const std::string& _date_string);

/**
 * @brief       Construct a string in iso extended format from a boost ptime
 * @param[in]   _ptime time in the boost format
 * @return      Time in the iso extended format YYYY-MM-DDTHH:MM:SS (example : 2007-02-25T00:00:00 )
 */
SIGHT_CORE_API std::string to_iso_extended_string(boost::posix_time::ptime _ptime);

/**
 * @brief       Construct a string in the format YYYY-MM-DD from a boost ptime
 * @param[in]   _date_and_time time in the boost format
 * @return      Time in the format YYYY-MM-DD (example : 2007-02-25 )
 */
SIGHT_CORE_API std::string get_date_in_xsd_format(const boost::posix_time::ptime& _date_and_time);

///@}

} // namespace sight::core::tools
