/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOIGTL_HELPER_PREFERENCES_HPP__
#define __IOIGTL_HELPER_PREFERENCES_HPP__

#include "ioIGTL/config.hpp"

#include <fwPreferences/helper.hpp>

#include <boost/lexical_cast.hpp>

#include <string>

namespace ioIGTL
{
namespace helper
{

//-----------------------------------------------------------------------------

// return the preference value if found, otherwise, cast the string into value as it is
template<typename T>
T getPreferenceKey(const std::string& key)
{
    std::string keyResult(key);
    const size_t first = key.find('%');
    const size_t last  = key.rfind('%');
    if (first == 0 && last == key.size() - 1)
    {
        keyResult = ::fwPreferences::getPreference( key.substr(1, key.size() - 2) );
    }
    return ::boost::lexical_cast< T >(keyResult);
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace ioIGTL

#endif /*__IOIGTL_HELPER_PREFERENCES_HPP__*/
