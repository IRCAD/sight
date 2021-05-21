/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "ui/base/config.hpp"
#include "ui/base/preferences/IPreferences.hpp"

#include <core/crypto/secure_string.hpp>

#include <data/Composite.hpp>

#include <boost/lexical_cast.hpp>

#include <filesystem>
#include <string>

namespace sight::ui::base::preferences
{

UI_BASE_API extern const std::string s_PREFERENCES_KEY;

/// Returns preferences associated with specified key. If not found, returns an empty string.
UI_BASE_API std::string getPreference(const std::string& preferenceKey);

/// Sets a value in preferences associated with specified key. If fail, returns false.
UI_BASE_API bool setPreference(const std::string& key, const std::string& value);

/// Returns file used to store all preferences in current context.
UI_BASE_API std::filesystem::path getPreferencesFile();

/// Returns the Composite of preferences. Return nullptr if it does not exist.
UI_BASE_API data::Composite::sptr getPreferences();

/**
 * @brief Gets the preferences service and call 'update', it should save the preferences file (if the service is
 * module::ui::base::preferences::SPreferences).
 * @warning This method does nothing if the preferences service is not found or if the service is not started.
 */
UI_BASE_API void savePreferences();

/**
 * @brief Returns associated value in preferences
 *
 * If the variable `var` is delimited by the character specified by `delimiter`,
 * it returns the associated value saved in the preferences (empty string if not found).
 * Otherwise, returns the variable directly.
 */
UI_BASE_API std::string getValue(const std::string& var, const char delimiter = '%');

/**
 * @brief Returns casted associated value in preferences
 *
 * If the variable `var` is delimited by the character specified by `delimiter`,
 * it returns the associated value casted in <T> saved in the preferences.
 * Otherwise, returns the variable directly casted in <T>.
 */
template<typename T>
T getValue(const std::string& var, const char delimiter = '%')
{
    const std::string value = getValue(var, delimiter);
    return ::boost::lexical_cast<T>(value);
}

/// Sets the password to use in the application. If null or empty, erase it.
UI_BASE_API void setPassword(const core::crypto::secure_string& password);

/// @Returns the password used in the application.
UI_BASE_API const core::crypto::secure_string getPassword();

/// Checks if the given password match the hash stored in the preferences
UI_BASE_API bool checkPassword(const core::crypto::secure_string& password);

/// Checks if a password hash is stored in the preferences
UI_BASE_API bool hasPasswordHash();

} // namespace sight::ui::base::preferences
