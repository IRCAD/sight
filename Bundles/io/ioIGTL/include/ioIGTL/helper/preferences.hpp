/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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
