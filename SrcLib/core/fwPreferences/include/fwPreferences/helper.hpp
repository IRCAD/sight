/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwPreferences/config.hpp"

#include <fwData/Composite.hpp>

#include <boost/filesystem.hpp>

#include <string>

namespace fwPreferences
{

FWPREFERENCES_API static const std::string s_PREFERENCES_KEY = "preferences";

/// Returns preferences associated with specified key. If not found, returns an empty string.
FWPREFERENCES_API std::string getPreference(const std::string& preferenceKey);

/// Sets a value in preferences associated with specified key. If fail, returns false.
FWPREFERENCES_API bool setPreference(const std::string& key, const std::string& value);

/// Returns file used to store all preferences in current context.
FWPREFERENCES_API ::boost::filesystem::path getPreferencesFile();

/// Returns the Composite of preferences. Return nullptr if it does not exist.
FWPREFERENCES_API ::fwData::Composite::sptr getPreferences();

} // namespace fwPreferences
