/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/dateAndTime.hpp"

#include <fwCore/base.hpp>

// Basic Virutals Type (macro & integer)
#include <boost/cstdint.hpp>

#include <boost/lexical_cast.hpp>
#if defined(_USE_BOOST_REGEX) || defined(__MACOSX__) || defined(WIN32)
#include <boost/regex.hpp>
#endif

namespace fwTools
{

//------------------------------------------------------------------------------

boost::gregorian::date
strToBoostDate
( const std::string dateStr )
{
        if ( dateStr.size() < 8 )
        {
                OSLM_WARN("strToBoostDate : the string lenght is too short (<8) : " << dateStr << ". The string is initialized with \"19000101\".");
                return boost::gregorian::date( boost::gregorian::from_undelimited_string( "19000101" ) );
        }
        else if ( dateStr.size() > 8 )
        {
                OSLM_WARN("strToBoostDate : the string lenght is too short (>8) : " << dateStr << ".. The string is trunked to 8 characters.");
                return strToBoostDate( dateStr.substr(0,8) );
        }
        else
        {
#if defined(_USE_BOOST_REGEX) || defined(__MACOSX__) || defined(WIN32)
                boost::regex isNumber ("[0-9]+");

                if( boost::regex_match(dateStr, isNumber) )
                {
                        return boost::gregorian::date( boost::gregorian::from_undelimited_string( dateStr ) );
                }
                else
                {
#endif
                        OSLM_WARN("strToBoostDate : the string not contains 8 numbers : " << dateStr << ". The string is initialized with \"19000101\".");
                        return boost::gregorian::date( boost::gregorian::from_undelimited_string( "19000101" ) );
#if defined(_USE_BOOST_REGEX) || defined(__MACOSX__) || defined(WIN32)
                }
#endif
        }
}

//------------------------------------------------------------------------------

boost::posix_time::time_duration
strToBoostTime
( const std::string timeStr )
{
        using boost::posix_time::time_duration;
        using boost::posix_time::hours;
        using boost::posix_time::minutes;
        using boost::posix_time::seconds;

        if ( timeStr.size() < 6 )
        {
                OSLM_WARN("strToBoostTime : the string lenght is too short (<6) : " << timeStr << ".  The string is initialized with \"000000\".");
                time_duration td = hours(0) + minutes(0) + seconds(0);
                return td;
        }
        else if ( timeStr.size() > 6 )
        {
                OSLM_WARN("strToBoostTime : the string lenght is too short (>6) : " << timeStr << ". This string is trunked.");
                return strToBoostTime( timeStr.substr(0,6) );
        }
        else
        {
#if defined(_USE_BOOST_REGEX) || defined(__MACOSX__) || defined(WIN32)
                boost::regex isNumber ("[0-9]+");

                if( boost::regex_match(timeStr, isNumber) )
                {
                        boost::uint16_t  h = boost::lexical_cast< boost::uint16_t  > ( timeStr.substr(0,2) );
                        boost::uint16_t  m = boost::lexical_cast< boost::uint16_t  > ( timeStr.substr(2,2) );
                        boost::uint16_t  s = boost::lexical_cast< boost::uint16_t  > ( timeStr.substr(4,2) );

                        time_duration td = hours(h) + minutes(m) + seconds(s);
                        return td;
                }
                else
                {
#endif
                        OSLM_WARN("strToBoostTime : the string not contains 6 numbers : " << timeStr <<". The string is initialized with \"000000\".");
                        time_duration td = hours(0) + minutes(0) + seconds(0);
                        return td;
#if defined(_USE_BOOST_REGEX) || defined(__MACOSX__) || defined(WIN32)
                }
#endif


        }
}

//------------------------------------------------------------------------------

boost::posix_time::ptime
strToBoostDateAndTime
( const std::string dateStr, const std::string timeStr)
{
        return boost::posix_time::ptime(strToBoostDate(dateStr),strToBoostTime(timeStr));
}

//------------------------------------------------------------------------------
std::string
getDate
( const boost::posix_time::ptime & dateAndTime )
{
        std::string dateAndTimeStr = boost::posix_time::to_iso_string(dateAndTime);
        return dateAndTimeStr.substr(0,8);;
}

//------------------------------------------------------------------------------

std::string
getTime
( const boost::posix_time::ptime & dateAndTime )
{
        std::string dateAndTimeStr = boost::posix_time::to_iso_string(dateAndTime);
        return dateAndTimeStr.substr(9,6);
}

}
