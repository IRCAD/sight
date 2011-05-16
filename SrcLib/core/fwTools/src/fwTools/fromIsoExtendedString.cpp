/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include "fwTools/fromIsoExtendedString.hpp"


namespace fwTools
{

//------------------------------------------------------------------------------

std::string getDateInXsdFormat( const boost::posix_time::ptime & dateAndTime )
{
    std::string dateAndTimeStr = boost::posix_time::to_iso_extended_string(dateAndTime);
    return dateAndTimeStr.substr(0,10);
}

//------------------------------------------------------------------------------

::boost::posix_time::ptime fromIsoExtendedString(const std::string & time_string)
{
    std::string tmp(time_string);
    std::string::size_type i = 0;
    while ((i = tmp.find('T', i)) != (std::string::size_type)(-1))
    tmp.replace(i++, 1, " ");
    ::boost::posix_time::ptime  t = boost::posix_time::time_from_string(tmp);
    return t;
}

//------------------------------------------------------------------------------

std::string toIsoExtendedString(boost::posix_time::ptime ptime)
{
    return boost::posix_time::to_iso_extended_string(ptime) ;
}

//------------------------------------------------------------------------------

std::string toStringWithoutAccent( const std::string & str )
{
    std::string newStr = str;

    std::string withAccent      ("ÀÁÂÃÄÅàáâãäåÒÓÔÕÖØòóôõöøÈÉÊËèéêëÇçÌÍÎÏìíîïÙÚÛÜùúûüÿÑñ");
    std::string withoutAccent   ("aaaaaaaaaaaaooooooooooooeeeeeeeecciiiiiiiiuuuuuuuuynn");


    std::string::iterator charWithAccent = withAccent.begin();
    std::string::iterator charWithoutAccent = withoutAccent.begin();

    while ( charWithAccent != withAccent.end() )
    {
        for (   std::string::iterator pCharacter = newStr.begin();
        pCharacter != newStr.end();
        ++pCharacter )
        {
            if( *pCharacter == *charWithAccent )
            {
                *pCharacter = *charWithoutAccent;
            }
        }
        charWithAccent++;
        charWithoutAccent++;
    }
    return newStr;
}

}//end namespace
