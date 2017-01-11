/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPREFERENCES_HELPER_HPP__
#define __FWPREFERENCES_HELPER_HPP__

#include "fwPreferences/config.hpp"

#include <boost/filesystem.hpp>

#include <string>

namespace fwPreferences
{

/// Returns preferences associated with specified key. If not found, returns an empty string.
FWPREFERENCES_API std::string getPreference(const std::string& preferenceKey);

/// Sets a value in preferences associated with specified key. If fail, returns false.
FWPREFERENCES_API bool setPreference(const std::string& key, const std::string& value);

/// Returns file used to store all preferences in current context.
FWPREFERENCES_API ::boost::filesystem::path getPreferencesFile();

} // namespace fwPreferences

#endif //__FWPREFERENCES_HELPER_HPP__
