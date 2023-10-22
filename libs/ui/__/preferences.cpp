/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

// cspell:ignore NOLINT NOLINTNEXTLINE

#include "ui/__/preferences.hpp"

#include "ui/__/application.hpp"
#include "ui/__/cursor.hpp"
#include "ui/__/dialog/input.hpp"
#include "ui/__/dialog/message.hpp"

#include <core/crypto/obfuscated_string.hpp>
#include <core/crypto/password_keeper.hpp>
#include <core/crypto/sha256.hpp>
#include <core/runtime/exit_exception.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/tools/os.hpp>

#include <io/zip/ArchiveReader.hpp>
#include <io/zip/ArchiveWriter.hpp>
#include <io/zip/exception/Read.hpp>

#include <boost/property_tree/json_parser.hpp>

namespace sight::ui
{

using core::crypto::secure_string;
using core::crypto::password_keeper;

// The default preference file name
static constexpr auto s_preferences_file = "preferences.json";
static constexpr auto s_encrypted_file   = "preferences.sight";

// Keep a password
static std::unique_ptr<password_keeper> s_password_keeper;

// Password policy to use
static password_keeper::password_policy s_password_keeper_policy {password_keeper::password_policy::NEVER};

// Encryption policy to use
static password_keeper::encryption_policy s_encryption_policy {password_keeper::encryption_policy::PASSWORD};

// If true, will exit on password error (default is false)
static bool s_exit_on_password_error {false};

// max number of retry
static std::size_t s_max_retry {3};

// The strings to use for the password dialog
static preferences::PasswordDialogStrings s_password_strings {
    .title         = std::string("Enter Password"),
    .message       = std::string("Password:"),
    .new_title     = std::string("Enter a new password"),
    .new_message   = std::string("New password:"),
    .weak_title    = std::string("Password is too weak"),
    .error_title   = std::string("Wrong password"),
    .error_message = std::string(
        "The provided password is wrong.\n\n"
        "The number of tries is exceeded. Some functionalities will be disabled."
                     ),
    .fatal_message = std::string(
        "The provided password is wrong.\n\n"
        "The number of tries is exceeded. The application will close."
                     ),
    .retry_message = std::string(
        "The provided password is wrong.\n\n"
        "You may retry with a different password."
                     ),
    .cancel_title         = std::string("Canceled"),
    .cancel_message       = std::string("User canceled. Some functionalities will be disabled."),
    .cancel_fatal_message = std::string("User canceled. The application will close.")
};

/// Password validator with default implementation
static std::function<std::pair<bool, std::string>(const sight::core::crypto::secure_string&)> s_password_validator {
    [](const sight::core::crypto::secure_string& _password)
    {
        static const std::string message(
            "The password is too weak. "
            "Please, use at least 8 characters, with one special, one uppercase and one lowercase.\n\n"
            "Reason:\n"
        );

        if(_password.length() < 8)
        {
            return std::make_pair(false, message + std::string("- Password is shorter than 8 characters long."));
        }

        bool has_special = false;
        bool has_upper   = false;
        bool has_lower   = false;

        for(const auto& c : _password)
        {
            // Convert to unsigned char to avoid undefined behavior with std::isupper/lower/isalnum
            // @see https://en.cppreference.com/w/cpp/string/byte/islower
            const auto uc = static_cast<unsigned char>(c);

            if(!has_upper && std::isupper(uc) != 0)
            {
                has_upper = true;
            }

            if(!has_lower && std::islower(uc) != 0)
            {
                has_lower = true;
            }

            if(!has_special && std::isalnum(uc) == 0)
            {
                has_special = true;
            }

            if(has_special && has_upper && has_lower)
            {
                return std::make_pair(true, std::string());
            }
        }

        std::string reason;

        if(!has_upper)
        {
            reason += " - Password is missing one uppercase character\n";
        }

        if(!has_lower)
        {
            reason += " - Password is missing one lowercase character\n";
        }

        if(!has_special)
        {
            reason += " - Password is missing one special character\n";
        }

        return std::make_pair(false, message + reason);
    }
};

/// @}

// Guard the preference tree
std::shared_mutex preferences::s_preferences_mutex;

// Contains the preference tree
std::unique_ptr<boost::property_tree::ptree> preferences::s_preferences;

// True if the preferences has been modified
bool preferences::s_is_preferences_modified {false};

// preferences can be disabled globally
bool preferences::s_is_enabled {false};

bool preferences::s_ignoreFilesystem {false};

//------------------------------------------------------------------------------

inline static bool must_encrypt()
{
    return (s_password_keeper && !s_password_keeper->get_password().empty())
           || s_encryption_policy == password_keeper::encryption_policy::FORCED;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path compute_preferences_filepath()
{
    // Get the profile
    const auto& profile = core::runtime::get_current_profile();
    SIGHT_THROW_IF("No current profile set", !profile);

    // Get the current application name
    const auto& name = profile->name();
    SIGHT_THROW_IF("Unable to determine application name", name.empty());

    // Get the application data directory
    const auto& config_directory = core::tools::os::get_user_config_dir(name);
    SIGHT_THROW_IF("Unable to define user data directory", config_directory.empty());

    // Build the preferences filepath
    const auto& preferences_filepath = config_directory / (must_encrypt() ? s_encrypted_file : s_preferences_file);
    SIGHT_THROW_IF(
        "preferences file '" + preferences_filepath.string() + "' already exists and is not a regular file",
        std::filesystem::exists(preferences_filepath)
        && !std::filesystem::is_regular_file(preferences_filepath)
    );

    return preferences_filepath;
}

//------------------------------------------------------------------------------

inline static secure_string compute_password()
{
    if(s_encryption_policy == password_keeper::encryption_policy::FORCED
       && (!s_password_keeper || s_password_keeper->get_password().empty()))
    {
        if constexpr(password_keeper::has_default_password())
        {
            return password_keeper::get_default_password();
        }
        else
        {
            // NOLINTNEXTLINE(readability-redundant-string-cstr)
            return password_keeper::get_pseudo_password_hash(core::runtime::get_current_profile()->name().c_str());
        }
    }
    else if(s_encryption_policy == password_keeper::encryption_policy::SALTED)
    {
        return password_keeper::get_pseudo_password_hash(
            // NOLINTNEXTLINE(readability-redundant-string-cstr)
            s_password_keeper->get_password() + core::runtime::get_current_profile()->name().c_str()
        );
    }
    else
    {
        return s_password_keeper->get_password();
    }
}

//------------------------------------------------------------------------------

inline static void set_password_nolock(const core::crypto::secure_string& _password)
{
    if(_password.empty())
    {
        // No password, we disable encryption
        s_password_keeper.reset();
    }
    else
    {
        // If the password_keeper is not there, create it
        if(!s_password_keeper)
        {
            s_password_keeper = std::make_unique<password_keeper>();
        }

        // Store the password
        s_password_keeper->set_password(_password);
    }
}

//------------------------------------------------------------------------------

preferences::preferences()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // current number of retry
    std::size_t password_retry {0};

    const std::function<void(void)> load =
        [&]
        {
            // Not enabled, simply return
            if(!s_is_enabled)
            {
                return;
            }

            // Already loaded, simply return
            if(s_preferences)
            {
                return;
            }

            // Don't read a preference file, simply create an empty preferences
            if(s_ignoreFilesystem)
            {
                s_preferences = std::make_unique<boost::property_tree::ptree>();
                return;
            }

            try
            {
                // Check if we can open the preferences file
                const auto& preferences_filepath = compute_preferences_filepath();
                if(!std::filesystem::exists(preferences_filepath))
                {
                    // Create an empty preferences
                    s_preferences = std::make_unique<boost::property_tree::ptree>();

                    // Get the password to use
                    const secure_string& password =
                        s_password_keeper
                        ? s_password_keeper->get_password()
                        : password_keeper::get_global_password();

                    // We may have to enter a new password
                    if(s_password_keeper_policy == password_keeper::password_policy::ALWAYS
                       || (s_password_keeper_policy == password_keeper::password_policy::GLOBAL && password.empty()))
                    {
                        secure_string new_password;
                        bool valid    = false;
                        bool accepted = true;

                        while(!valid && accepted)
                        {
                            // Get the new password from user
                            std::tie(new_password, accepted) = sight::ui::dialog::input::showInputDialog(
                                s_password_strings.new_title.value_or(s_password_strings.title.value_or("")),
                                s_password_strings.new_message.value_or(s_password_strings.message.value_or("")),
                                password.c_str(), // NOLINT(readability-redundant-string-cstr)
                                sight::ui::dialog::input::EchoMode::PASSWORD
                            );

                            // If user didn't accept, we stop here
                            if(!accepted)
                            {
                                break;
                            }

                            // Test validity of the password
                            SIGHT_ASSERT("No password validator defined", s_password_validator);
                            std::string message;
                            std::tie(valid, message) = s_password_validator(new_password);

                            if(!valid)
                            {
                                sight::ui::dialog::message::show(
                                    s_password_strings.weak_title.value_or(""),
                                    message,
                                    sight::ui::dialog::message::WARNING
                                );
                            }
                        }

                        if(accepted)
                        {
                            set_password_nolock(new_password);
                            password_keeper::set_global_password(new_password);
                        }
                        else if(s_exit_on_password_error)
                        {
                            SIGHT_INFO("User canceled. Exiting.");
                            sight::ui::BusyCursor cursor;
                            sight::ui::application::get()->exit(0, false);
                        }
                        else
                        {
                            s_is_enabled = false;
                            SIGHT_WARN("preferences has been disabled because no password was entered.");
                            sight::ui::dialog::message::show(
                                s_password_strings.cancel_title.value_or(""),
                                s_password_strings.cancel_message.value_or(""),
                                sight::ui::dialog::message::WARNING
                            );
                            return;
                        }
                    }

                    s_is_preferences_modified = true;
                    return;
                }

                // Try to open the preferences file.
                s_preferences = std::make_unique<boost::property_tree::ptree>();

                // If a password has been set or if the encryption is "forced" open as an encrypted file
                if(must_encrypt())
                {
                    // Open the archive that holds the property tree
                    auto archive = io::zip::ArchiveReader::get(preferences_filepath);

                    // Create the input stream, with a password, allowing decoding an encrypted file
                    auto istream = archive->openFile(
                        s_preferences_file,
                        compute_password()
                    );

                    // Read the property tree from the archive
                    boost::property_tree::read_json(*istream, *s_preferences);
                }
                else
                {
                    boost::property_tree::read_json(preferences_filepath.string(), *s_preferences);
                }

                // preferences has been read from disk
                s_is_preferences_modified = false;
                s_is_enabled              = true;

                // Reset the retry counter if the password was correct
                password_retry = 0;
            }
            catch(const io::zip::exception::BadPassword& e)
            {
                s_preferences.reset();

                bool accepted = true;

                if(s_password_keeper_policy == password_keeper::password_policy::NEVER)
                {
                    // Give a chance to retry with a different password
                    SIGHT_THROW_EXCEPTION(BadPassword(e.what()));
                }

                // Check the number of try (0 means first try)
                if(password_retry <= s_max_retry)
                {
                    // Only display a warning the second time
                    if(password_retry++ > 0)
                    {
                        sight::ui::dialog::message::show(
                            s_password_strings.error_title.value_or(""),
                            s_password_strings.retry_message.value_or(""),
                            sight::ui::dialog::message::WARNING
                        );
                    }

                    // Initialize the new password with the previous
                    secure_string new_password =
                        s_password_keeper
                        ? s_password_keeper->get_password()
                        : password_keeper::get_global_password();

                    // Get the new password from user
                    std::tie(new_password, accepted) = sight::ui::dialog::input::showInputDialog(
                        s_password_strings.title.value_or(""),
                        s_password_strings.message.value_or(""),
                        new_password.c_str(), // NOLINT(readability-redundant-string-cstr)
                        sight::ui::dialog::input::EchoMode::PASSWORD
                    );

                    // If user accept, we check the new password again
                    if(accepted)
                    {
                        // Save the new password
                        set_password_nolock(new_password);
                        password_keeper::set_global_password(new_password);

                        // Retry...
                        load();
                        return;
                    }
                }

                // Too much retries or canceled
                s_is_enabled = false;

                if(s_exit_on_password_error)
                {
                    if(accepted)
                    {
                        SIGHT_WARN("The provided password is wrong. Exiting.");
                        sight::ui::dialog::message::show(
                            s_password_strings.error_title.value_or(""),
                            s_password_strings.fatal_message.value_or(""),
                            sight::ui::dialog::message::CRITICAL
                        );
                    }
                    else
                    {
                        SIGHT_INFO("The user canceled. Exiting.");
                    }

                    sight::ui::BusyCursor cursor;
                    sight::ui::application::get()->exit(0, false);
                }
                else
                {
                    if(accepted)
                    {
                        SIGHT_WARN("preferences has been disabled because the provided password is wrong.");
                        sight::ui::dialog::message::show(
                            s_password_strings.error_title.value_or(""),
                            s_password_strings.error_message.value_or(""),
                            sight::ui::dialog::message::WARNING
                        );
                    }
                    else
                    {
                        SIGHT_INFO("preferences has been disabled because the user canceled.");
                        sight::ui::dialog::message::show(
                            s_password_strings.cancel_title.value_or(""),
                            s_password_strings.cancel_message.value_or(""),
                            sight::ui::dialog::message::WARNING
                        );
                    }

                    SIGHT_THROW_EXCEPTION(BadPassword(e.what()));
                }
            }
            catch(const core::runtime::exit_exception&)
            {
                // Propagate the exception
                s_is_enabled = false;
                throw;
            }
            catch(const std::exception& e)
            {
                // We simply print an error and disable preferences management
                s_is_enabled = false;
                SIGHT_ERROR(e.what());
            }
            catch(...)
            {
                // We disable preferences management
                s_is_enabled = false;
            }
        };

    load();

    throw_if_disabled();
}

preferences::~preferences()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // Check if we must save the modifications
    if(s_is_enabled && s_is_preferences_modified && !s_ignoreFilesystem)
    {
        const auto& preferences_filepath = compute_preferences_filepath();

        if(must_encrypt())
        {
            // Delete the preferences file, since minizip is unable to replace file.
            // Doing otherwise will result with many preferences.json inside the archive, which cannot be read back
            std::filesystem::remove_all(preferences_filepath);

            // Create the archive that will hold the property tree
            auto archive = io::zip::ArchiveWriter::get(preferences_filepath);

            // Create the output stream, with a password, resulting in an encrypted file
            auto ostream = archive->openFile(
                s_preferences_file,
                compute_password()
            );

            // Read the property tree from the archive
            boost::property_tree::write_json(*ostream, *s_preferences, false);
        }
        else
        {
            boost::property_tree::write_json(preferences_filepath.string(), *s_preferences);
        }

        s_is_preferences_modified = false;
    }
}

//------------------------------------------------------------------------------

void preferences::erase(const std::string& _key)
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // If the preferences are disabled or not loaded, throw an exception to disallow loading
    throw_if_disabled();

    s_preferences->erase(_key);

    // Mark the tree as modified so it will be saved when preferences is deleted
    s_is_preferences_modified = true;
}

//------------------------------------------------------------------------------

void preferences::clear()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // If the preferences are disabled or not loaded, throw an exception to disallow loading
    throw_if_disabled();

    s_preferences->clear();

    // Mark the tree as modified so it will be saved when preferences is deleted
    s_is_preferences_modified = true;
}

//------------------------------------------------------------------------------

void preferences::set_enabled(bool _enable)
{
    std::unique_lock guard(s_preferences_mutex);
    s_is_enabled = _enable;

    if(!_enable)
    {
        s_preferences.reset();
        s_is_preferences_modified = false;
    }
}

//------------------------------------------------------------------------------

void preferences::ignoreFilesystem(bool _ignore)
{
    std::unique_lock guard(s_preferences_mutex);
    s_ignoreFilesystem = _ignore;
}

//------------------------------------------------------------------------------

void preferences::throw_if_disabled()
{
    SIGHT_THROW_EXCEPTION_IF(
        preferences_disabled("preferences are disabled"),
        !s_is_enabled || !s_preferences
    );
}

//------------------------------------------------------------------------------

void preferences::set_password(const core::crypto::secure_string& _password)
{
    std::unique_lock guard(s_preferences_mutex);
    set_password_nolock(_password);
}

//------------------------------------------------------------------------------

void preferences::set_password_policy(const core::crypto::password_keeper::password_policy _policy)
{
    std::unique_lock guard(s_preferences_mutex);
    s_password_keeper_policy = _policy;
}

//------------------------------------------------------------------------------

void preferences::set_encryption_policy(const core::crypto::password_keeper::encryption_policy _policy)
{
    std::unique_lock guard(s_preferences_mutex);
    s_encryption_policy = _policy;
}

//------------------------------------------------------------------------------

void preferences::exit_on_password_error(bool _exit)
{
    std::unique_lock guard(s_preferences_mutex);
    s_exit_on_password_error = _exit;
}

//------------------------------------------------------------------------------

void preferences::set_password_dialog_strings(const PasswordDialogStrings& _strings)
{
    std::unique_lock guard(s_preferences_mutex);

    if(_strings.title)
    {
        s_password_strings.title = _strings.title;
    }

    if(_strings.message)
    {
        s_password_strings.message = _strings.message;
    }

    if(_strings.new_title)
    {
        s_password_strings.new_title = _strings.new_title;
    }

    if(_strings.new_message)
    {
        s_password_strings.new_message = _strings.new_message;
    }

    if(_strings.weak_title)
    {
        s_password_strings.weak_title = _strings.weak_title;
    }

    if(_strings.error_title)
    {
        s_password_strings.error_title = _strings.error_title;
    }

    if(_strings.error_message)
    {
        s_password_strings.error_message = _strings.error_message;
    }

    if(_strings.fatal_message)
    {
        s_password_strings.fatal_message = _strings.fatal_message;
    }

    if(_strings.retry_message)
    {
        s_password_strings.retry_message = _strings.retry_message;
    }

    if(_strings.cancel_title)
    {
        s_password_strings.cancel_title = _strings.cancel_title;
    }

    if(_strings.cancel_message)
    {
        s_password_strings.cancel_message = _strings.cancel_message;
    }

    if(_strings.cancel_fatal_message)
    {
        s_password_strings.cancel_fatal_message = _strings.cancel_fatal_message;
    }
}

//------------------------------------------------------------------------------

void preferences::set_password_validator(
    std::function<std::pair<bool, std::string>(const sight::core::crypto::secure_string&)> _validator
)
{
    std::unique_lock guard(s_preferences_mutex);
    s_password_validator = std::move(_validator);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
