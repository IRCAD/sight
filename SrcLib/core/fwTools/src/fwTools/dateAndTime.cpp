/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwTools/dateAndTime.hpp"

#include <fwCore/base.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

namespace fwTools
{

//------------------------------------------------------------------------------

::boost::gregorian::date strToBoostDate( const std::string& dateStr )
{
    ::boost::gregorian::date resDate( ::boost::gregorian::from_undelimited_string( "19000101" ) );
    if ( dateStr.size() < 8 )
    {
        OSLM_WARN(
            "The string length is too short (<8) : " << dateStr <<
            ". The string is initialized with \"19000101\".");
    }
    else if ( dateStr.size() > 8 )
    {
        OSLM_WARN("The string length is too long (>8) : " << dateStr << ".. The string is trunked to 8 characters.");
        resDate = strToBoostDate( dateStr.substr(0, 8) );
    }
    else
    {
        ::boost::regex isNumber("[0-9]+");

        if( ::boost::regex_match(dateStr, isNumber) )
        {
            resDate = ::boost::gregorian::date( ::boost::gregorian::from_undelimited_string( dateStr ) );
        }
        else
        {
            OSLM_WARN(
                "The string not contains 8 numbers : " << dateStr <<
                ". The string is initialized with \"19000101\".");
        }
    }
    return resDate;
}

//------------------------------------------------------------------------------

::boost::posix_time::time_duration strToBoostTime( const std::string& timeStr )
{
    using ::boost::posix_time::time_duration;
    using ::boost::posix_time::hours;
    using ::boost::posix_time::minutes;
    using ::boost::posix_time::seconds;

    time_duration td;
    if ( timeStr.size() < 6 )
    {
        OSLM_WARN("The string length is too short (<6) : " << timeStr <<
                  ".  The string is initialized with \"000000\".");
        td = hours(0) + minutes(0) + seconds(0);
    }
    else if ( timeStr.size() > 6 )
    {
        OSLM_WARN("The string length is too short (>6) : " << timeStr << ". This string is trunked.");
        td = strToBoostTime( timeStr.substr(0, 6) );
    }
    else
    {
        ::boost::regex isNumber("[0-9]+");
        if( ::boost::regex_match(timeStr, isNumber) )
        {
            std::uint16_t h = ::boost::lexical_cast< std::uint16_t > ( timeStr.substr(0, 2) );
            std::uint16_t m = ::boost::lexical_cast< std::uint16_t > ( timeStr.substr(2, 2) );
            std::uint16_t s = ::boost::lexical_cast< std::uint16_t > ( timeStr.substr(4, 2) );
            td = hours(h) + minutes(m) + seconds(s);
        }
        else
        {
            OSLM_WARN("The string not contains 6 numbers : " << timeStr <<
                      ". The string is initialized with \"000000\".");
            td = hours(0) + minutes(0) + seconds(0);
        }
    }
    return td;
}

//------------------------------------------------------------------------------

::boost::posix_time::ptime strToBoostDateAndTime( const std::string& dateStr, const std::string& timeStr)
{
    return ::boost::posix_time::ptime(strToBoostDate(dateStr), strToBoostTime(timeStr));
}

//------------------------------------------------------------------------------

std::string getDate( const ::boost::posix_time::ptime& dateAndTime )
{
    std::string dateAndTimeStr = ::boost::posix_time::to_iso_string(dateAndTime);
    return dateAndTimeStr.substr(0, 8);
}

//------------------------------------------------------------------------------

std::string getTime( const ::boost::posix_time::ptime& dateAndTime )
{
    std::string dateAndTimeStr = ::boost::posix_time::to_iso_string(dateAndTime);
    return dateAndTimeStr.substr(9, 6);
}

//------------------------------------------------------------------------------

std::string getCurrentTime()
{
    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    std::string nowStr = ::boost::posix_time::to_simple_string(now);
    return nowStr.substr(0, 21);
}

}
