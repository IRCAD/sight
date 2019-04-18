/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwPreferences/config.hpp"
#include "fwPreferences/IPreferences.hpp"

#include <fwData/Composite.hpp>

#include <boost/filesystem.hpp>

#include <string>

namespace fwPreferences
{

FWPREFERENCES_API extern const std::string s_PREFERENCES_KEY;

/// Returns preferences associated with specified key. If not found, returns an empty string.
FWPREFERENCES_API std::string getPreference(const std::string& preferenceKey);

/// Sets a value in preferences associated with specified key. If fail, returns false.
FWPREFERENCES_API bool setPreference(const std::string& key, const std::string& value);

/// Returns file used to store all preferences in current context.
FWPREFERENCES_API ::boost::filesystem::path getPreferencesFile();

/// Returns the Composite of preferences. Return nullptr if it does not exist.
FWPREFERENCES_API ::fwData::Composite::sptr getPreferences();

/**
 * @brief Gets the preferences service and call 'update', it should save the preferences file (if the service is
 * ::preferences::SPreferences).
 * @warning This method does nothing if the preferences service is not found or if the service is not started.
 */
FWPREFERENCES_API void savePreferences();

/**
 * @brief If the variable `var` is delimited by the character specified by `delimiter`,
 * it returns the associated value saved in the preferences (empty string if not found).
 * Otherwise, returns the variable directly.
 */
FWPREFERENCES_API std::string getValue(const std::string& var, const char delimiter = '%');

/**
 * @brief If the variable `var` is delimited by the character specified by `delimiter`,
 * it returns the associated value casted in <T> saved in the preferences.
 * Otherwise, returns the variable directly casted in <T>.
 */
template<typename T>
T getValue(const std::string& var, const char delimiter = '%')
{
    const std::string value = getValue(var, delimiter);
    return ::boost::lexical_cast< T >(value);
}

} // namespace fwPreferences
