/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
