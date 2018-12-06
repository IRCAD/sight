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

#include "fwTools/fromIsoExtendedString.hpp"

#include <sstream>

namespace fwTools
{

//------------------------------------------------------------------------------

std::string getDateInXsdFormat( const boost::posix_time::ptime& dateAndTime )
{
    std::string dateAndTimeStr = boost::posix_time::to_iso_extended_string(dateAndTime);
    return dateAndTimeStr.substr(0, 10);
}

//------------------------------------------------------------------------------

::boost::posix_time::ptime fromIsoExtendedString(const std::string& time_string)
{
    std::string tmp(time_string);
    std::string::size_type i = 0;
    while ((i = tmp.find('T', i)) != (std::string::size_type)(-1))
    {
        tmp.replace(i++, 1, " ");
    }
    ::boost::posix_time::ptime t = boost::posix_time::time_from_string(tmp);
    return t;
}

//------------------------------------------------------------------------------

::boost::posix_time::ptime fromDateInXsdFormat(const std::string& date_string)
{
    std::stringstream ss;
    ss << date_string << " 00:00:00";

    ::boost::posix_time::ptime t = boost::posix_time::time_from_string(ss.str());
    return t;
}

//------------------------------------------------------------------------------

std::string toIsoExtendedString(boost::posix_time::ptime ptime)
{
    return boost::posix_time::to_iso_extended_string(ptime);
}

}//end namespace
