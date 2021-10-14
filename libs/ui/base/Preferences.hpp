/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include <core/crypto/PasswordKeeper.hpp>
#include <core/Exception.hpp>
#include <core/exceptionmacros.hpp>
#include <core/macros.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>

#include <filesystem>
#include <mutex>
#include <shared_mutex>

namespace sight::ui::base
{

/// Simple exception class thrown when preferences have been disabled or because of they cannot be enabled due to errors
/// like corrupted preferences files
class UI_BASE_CLASS_API PreferencesDisabled : public core::Exception
{
public:

    inline PreferencesDisabled(const std::string& err) noexcept :
        core::Exception(err)
    {
    }

    virtual ~PreferencesDisabled() = default;
};

/// Subclass of PreferencesDisabled, thrown when trying to open an encrypted file with a wrong password
class UI_BASE_CLASS_API BadPassword : public PreferencesDisabled
{
public:

    inline BadPassword(const std::string& err) noexcept :
        PreferencesDisabled(err)
    {
    }

    virtual ~BadPassword() = default;
};

/**
 * @brief Application preferences
 *
 * @details Class that holds a global static property tree used to store and retrieve preferences accross all services.
 *
 * @note It uses Boost property tree as its backend, so it mimics its syntax.
 *
 * @section Usage Basic Usage
 * The class is thread safe and use RAII mechanism to load and store data from and to the preference file. In a service,
 * rhe basic usage would be:
 *
 * @code{.cpp}
    try
    {
        // Load
        Preferences preferences;
        std::string filename = preferences.get("debug.filename");
        int level = preferences.get<int>("debug.level");
        ...
        // Save
        Preferences preferences;
        preferences.put("debug.filename", "log.txt");
        preferences.put("debug.level", 2);
    }
    catch(const ui::base::PreferencesDisabled&)
    {
        // Nothing to do..
    }
   @endcode
 *
 * Which will be translated into:
 * @code{.json}
    debug:
        filename: "log.txt"
        level: 2
   @endcode
 *
 * @section Configuration Configuration
 *
 * the configuration is done by static functions:
 *
 * @subsection set_enabled set_enabled
 * Enable or disable preference management as a whoole. All functions, including constructor will throw
 * `PreferencesDisabled` exception if used while "disabled"
 *
 * @subsection set_password set_password
 * Set an harcoded password to be used. It enables defacto the encryption
 *
 * @subsection set_password_policy set_password_policy
 * Defines how and when a password is asked. @see sight::core::crypto::PasswordKeeper::PasswordPolicy for possible
 * values.
 *
 * @note `NEVER` will never display any message box, but if a password has been set, the resulting preference file will
 * still be encrypted. An `BadPassword` exception will be thrown instead of diplaying a message box, asking to renter
 * the password.
 *
 * @subsection set_encryption_policy set_encryption_policy
 * Define when the preferences file is encrypted: @see sight::core::crypto::PasswordKeeper::EncryptionPolicy for
 * possible values.
 *
 * @note `FORCE` will encrypt the file, even if no password is given. In this case a pseudo random password is used,
 * which can be guessed if someone has access to the code. Another option is to use an harcoded password AND
 * EncryptionPolicy::SALTED
 *
 *
 * @section Module  Module Configuration
 * All the above can be configured through the module ui_base parameters ( @see sight::module::ui::base::Plugin )
 *
 * The preferences are enabled by default, without encryption. An example of configuration would be:
 *
 * @code {.cmake}
    moduleParam(
        module_ui_base
        PARAM_LIST
            preferences_enabled
            preferences_password_policy
            preferences_encryption_policy
            preferences_password
        PARAM_VALUES
            true
            once
            salted
            a_bad_hardcoded_password
    )
 * @endcode
 *
 */
class UI_BASE_CLASS_API Preferences final
{
public:

    static constexpr auto s_DEFAULT_DELIMITER = "%";

    /// Constructor
    /// It will load the default preference file if not already done and throw 'PreferencesDisabled' exception in case
    /// of error or when disabled (or BadPassword, if we use encryption and the password is not the good one)
    UI_BASE_API Preferences();

    /// Destructor
    /// It will save the preferences, if they have been modified
    UI_BASE_API ~Preferences();

    /// Returns the preference associated with the given key/path
    /// @param key the key/path of the preference.
    template<typename T>
    inline T get(const std::string& key) const
    {
        // Protect preferences for reading
        std::shared_lock guard(s_preferences_mutex);

        // If the preferences are disabled or not loaded, throw an exception to disallow loading
        throw_if_disabled();

        return s_preferences->get<T>(key);
    }

    /// Returns an optional preference associated with the given key/path.
    /// @param key the key/path of the preference.
    template<typename T>
    inline boost::optional<T> get_optional(const std::string& key) const
    {
        // Protect preferences for reading
        std::shared_lock guard(s_preferences_mutex);

        // If the preferences are disabled or not loaded, throw an exception to disallow loading
        throw_if_disabled();

        return s_preferences->get_optional<T>(key);
    }

    /// Returns the preference associated with the given key/path. If the preference doesn't exist, the default value is
    /// returned
    /// @param key the key/path of the preference.
    /// @param default_value the default value used when the key/path is not found.
    template<typename T>
    inline T get(const std::string& key, const T& default_value) const noexcept
    {
        try
        {
            return get<T>(key);
        }
        catch(const boost::property_tree::ptree_error& e)
        {
            SIGHT_WARN(e.what());
            return default_value;
        }
        catch(...)
        {
            return default_value;
        }
    }

    /// Special "get" version mostly used in xml with a magic "%" delimiter. If there are no "%", it means we should
    /// return the key as a value, otherwise, we search in the preference
    /// @param key the key/path of the preference, that could be the preference if no delimiter are used.
    /// @param delimiter the magical delimiter.
    template<typename T>
    inline T delimited_get(const std::string& key, const std::string& delimiter = s_DEFAULT_DELIMITER) const
    {
        if(const auto& delimiter_start = key.find_first_of(delimiter); delimiter_start != std::string::npos)
        {
            if(const auto& delimiter_end = key.find_last_of(delimiter); delimiter_end != std::string::npos)
            {
                const auto& real_key = key.substr(delimiter_start + 1, delimiter_end - (delimiter_start + 1));
                return get<T>(real_key);
            }
        }

        return boost::lexical_cast<T>(key);
    }

    /// Special "get" version mostly used in xml with a magic "%" delimiter. If there are no "%", it means we should
    /// return the key as a value, otherwise, we search in the preference. If no preference is found, we return the
    /// default value
    /// @param key the key/path of the preference, that could be the preference if no delimiter are used.
    /// @param default_value the default value used when the key/path is not found.
    /// @param delimiter the magical delimiter.
    template<typename T>
    inline T delimited_get(
        const std::string& key,
        const T& default_value,
        const std::string& delimiter = s_DEFAULT_DELIMITER
    ) const noexcept
    {
        try
        {
            return delimited_get<T>(key, delimiter);
        }
        catch(const boost::property_tree::ptree_error& e)
        {
            SIGHT_WARN(e.what());
            return default_value;
        }
        catch(...)
        {
            return default_value;
        }
    }

    /// Set a preference associated with the given key/path
    /// @param key the key/path of the preference.
    /// @param value the value to set.
    template<typename T>
    inline void put(const std::string& key, const T& value)
    {
        // Protect preferences for writing
        std::unique_lock guard(s_preferences_mutex);

        // If the preferences are disabled or not loaded, throw an exception to disallow puting values
        throw_if_disabled();

        s_preferences->put(key, value);

        // Mark the tree as modified so it will be saved when Preferences is deleted
        s_is_preferences_modified = true;
    }

    /// Remove one value from the preferences
    UI_BASE_API void erase(const std::string& key);

    /// Remove all preferences
    UI_BASE_API void clear();

    /// Enable / disable the preferences system. All functions will throw a PreferencesDisabled, if disabled
    UI_BASE_API static void set_enabled(bool enable);

    /// Set a password and enable encryption
    UI_BASE_API static void set_password(const core::crypto::secure_string& password);

    /// Set the password policy
    /// @param policy @see sight::core::crypto::PasswordKeeper::PasswordPolicy
    UI_BASE_API static void set_password_policy(const core::crypto::PasswordKeeper::PasswordPolicy policy);

    /// Set the encryption policy
    /// @param policy @see sight::core::crypto::PasswordKeeper::EncryptionPolicy
    UI_BASE_API static void set_encryption_policy(const core::crypto::PasswordKeeper::EncryptionPolicy policy);

private:

    /// Throws a PreferencesDisabled if preferences management is disabled
    UI_BASE_API static void throw_if_disabled();

    /// Guard the preference tree
    UI_BASE_API static std::shared_mutex s_preferences_mutex;

    /// Contains the preference tree
    UI_BASE_API static std::unique_ptr<boost::property_tree::ptree> s_preferences;

    /// True if the preferences has been modified
    UI_BASE_API static bool s_is_preferences_modified;

    /// Preferences can be disabled globally
    UI_BASE_API static bool s_is_enabled;
};

} // namespace sight::ui::base
