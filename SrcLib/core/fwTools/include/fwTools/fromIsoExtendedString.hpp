/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwTools/config.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

namespace fwTools
{

/**
 * @name Date and time in iso extended format
 * @{
 */

/**
 * @brief       Construct a boost ptime from a string in iso extended format (YYYY-MM-DDTHH:MM:SS)
 * @param[in]   time_string time in the iso extended format YYYY-MM-DDTHH:MM:SS (example : 2007-02-25T00:00:00 )
 * @return      Time in the boost format
 */
FWTOOLS_API boost::posix_time::ptime fromIsoExtendedString(const std::string& time_string );

/**
 * @brief   Construct a boost ptime from a string in xsd format (YYYY-MM-DD)
 * @param   date_string date in the xsd format YYYY-MM-DD
 * @return  Time in the boost format
 */
FWTOOLS_API ::boost::posix_time::ptime fromDateInXsdFormat(const std::string& date_string);

/**
 * @brief       Construct a string in iso extended format from a boost ptime
 * @param[in]   ptime time in the boost format
 * @return      Time in the iso extended format YYYY-MM-DDTHH:MM:SS (example : 2007-02-25T00:00:00 )
 */
FWTOOLS_API std::string toIsoExtendedString(boost::posix_time::ptime ptime);

/**
 * @brief       Construct a string in the format YYYY-MM-DD from a boost ptime
 * @param[in]   dateAndTime time in the boost format
 * @return      Time in the format YYYY-MM-DD (example : 2007-02-25 )
 */
FWTOOLS_API std::string getDateInXsdFormat( const boost::posix_time::ptime& dateAndTime );

///@}

}//end namespace
