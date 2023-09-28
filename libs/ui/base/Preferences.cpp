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

#include "ui/base/Preferences.hpp"

#include "ui/base/Application.hpp"
#include "ui/base/Cursor.hpp"
#include "ui/base/dialog/InputDialog.hpp"
#include "ui/base/dialog/MessageDialog.hpp"

#include <core/crypto/obfuscated_string.hpp>
#include <core/crypto/PasswordKeeper.hpp>
#include <core/crypto/SHA256.hpp>
#include <core/runtime/ExitException.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/tools/Os.hpp>

#include <io/zip/ArchiveReader.hpp>
#include <io/zip/ArchiveWriter.hpp>
#include <io/zip/exception/Read.hpp>

#include <boost/property_tree/json_parser.hpp>

namespace sight::ui::base
{

using core::crypto::secure_string;
using core::crypto::PasswordKeeper;

// The default preference file name
static constexpr auto s_preferences_file = "preferences.json";
static constexpr auto s_encrypted_file   = "preferences.sight";

// Keep a password
static std::unique_ptr<PasswordKeeper> s_password_keeper;

// Password policy to use
static PasswordKeeper::PasswordPolicy s_password_keeper_policy {PasswordKeeper::PasswordPolicy::NEVER};

// Encryption policy to use
static PasswordKeeper::EncryptionPolicy s_encryption_policy {PasswordKeeper::EncryptionPolicy::PASSWORD};

// If true, will exit on password error (default is false)
static bool s_exit_on_password_error {false};

// max number of retry
static std::size_t s_max_retry {3};

// The strings to use for the password dialog
static Preferences::PasswordDialogStrings s_password_strings {
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
    [](const sight::core::crypto::secure_string& password)
    {
        static const std::string message(
            "The password is too weak. "
            "Please, use at least 8 characters, with one special, one uppercase and one lowercase.\n\n"
            "Reason:\n"
        );

        if(password.length() < 8)
        {
            return std::make_pair(false, message + std::string("- Password is shorter than 8 characters long."));
        }

        bool has_special = false;
        bool has_upper   = false;
        bool has_lower   = false;

        for(const auto& c : password)
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
std::shared_mutex Preferences::s_preferences_mutex;

// Contains the preference tree
std::unique_ptr<boost::property_tree::ptree> Preferences::s_preferences;

// True if the preferences has been modified
bool Preferences::s_is_preferences_modified {false};

// Preferences can be disabled globally
bool Preferences::s_is_enabled {false};

bool Preferences::s_ignoreFilesystem {false};

//------------------------------------------------------------------------------

inline static bool mustEncrypt()
{
    return (s_password_keeper && !s_password_keeper->get_password().empty())
           || s_encryption_policy == PasswordKeeper::EncryptionPolicy::FORCED;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path computePreferencesFilepath()
{
    // Get the profile
    const auto& profile = core::runtime::getCurrentProfile();
    SIGHT_THROW_IF("No current profile set", !profile);

    // Get the current application name
    const auto& name = profile->getName();
    SIGHT_THROW_IF("Unable to determine application name", name.empty());

    // Get the application data directory
    const auto& config_directory = core::tools::os::getUserConfigDir(name);
    SIGHT_THROW_IF("Unable to define user data directory", config_directory.empty());

    // Build the preferences filepath
    const auto& preferences_filepath = config_directory / (mustEncrypt() ? s_encrypted_file : s_preferences_file);
    SIGHT_THROW_IF(
        "Preferences file '" + preferences_filepath.string() + "' already exists and is not a regular file",
        std::filesystem::exists(preferences_filepath)
        && !std::filesystem::is_regular_file(preferences_filepath)
    );

    return preferences_filepath;
}

//------------------------------------------------------------------------------

inline static secure_string computePassword()
{
    if(s_encryption_policy == PasswordKeeper::EncryptionPolicy::FORCED
       && (!s_password_keeper || s_password_keeper->get_password().empty()))
    {
        if constexpr(PasswordKeeper::has_default_password())
        {
            return PasswordKeeper::get_default_password();
        }
        else
        {
            // NOLINTNEXTLINE(readability-redundant-string-cstr)
            return PasswordKeeper::get_pseudo_password_hash(core::runtime::getCurrentProfile()->getName().c_str());
        }
    }
    else if(s_encryption_policy == PasswordKeeper::EncryptionPolicy::SALTED)
    {
        return PasswordKeeper::get_pseudo_password_hash(
            // NOLINTNEXTLINE(readability-redundant-string-cstr)
            s_password_keeper->get_password() + core::runtime::getCurrentProfile()->getName().c_str()
        );
    }
    else
    {
        return s_password_keeper->get_password();
    }
}

//------------------------------------------------------------------------------

inline static void setPasswordNolock(const core::crypto::secure_string& password)
{
    if(password.empty())
    {
        // No password, we disable encryption
        s_password_keeper.reset();
    }
    else
    {
        // If the PasswordKeeper is not there, create it
        if(!s_password_keeper)
        {
            s_password_keeper = std::make_unique<PasswordKeeper>();
        }

        // Store the password
        s_password_keeper->set_password(password);
    }
}

//------------------------------------------------------------------------------

Preferences::Preferences()
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
                const auto& preferences_filepath = computePreferencesFilepath();
                if(!std::filesystem::exists(preferences_filepath))
                {
                    // Create an empty preferences
                    s_preferences = std::make_unique<boost::property_tree::ptree>();

                    // Get the password to use
                    const secure_string& password =
                        s_password_keeper
                        ? s_password_keeper->get_password()
                        : PasswordKeeper::get_global_password();

                    // We may have to enter a new password
                    if(s_password_keeper_policy == PasswordKeeper::PasswordPolicy::ALWAYS
                       || (s_password_keeper_policy == PasswordKeeper::PasswordPolicy::GLOBAL && password.empty()))
                    {
                        secure_string new_password;
                        bool valid    = false;
                        bool accepted = true;

                        while(!valid && accepted)
                        {
                            // Get the new password from user
                            std::tie(new_password, accepted) = sight::ui::base::dialog::InputDialog::showInputDialog(
                                s_password_strings.new_title.value_or(s_password_strings.title.value_or("")),
                                s_password_strings.new_message.value_or(s_password_strings.message.value_or("")),
                                password.c_str(), // NOLINT(readability-redundant-string-cstr)
                                sight::ui::base::dialog::InputDialog::EchoMode::PASSWORD
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
                                sight::ui::base::dialog::MessageDialog::show(
                                    s_password_strings.weak_title.value_or(""),
                                    message,
                                    sight::ui::base::dialog::IMessageDialog::WARNING
                                );
                            }
                        }

                        if(accepted)
                        {
                            setPasswordNolock(new_password);
                            PasswordKeeper::set_global_password(new_password);
                        }
                        else if(s_exit_on_password_error)
                        {
                            SIGHT_INFO("User canceled. Exiting.");
                            sight::ui::base::BusyCursor cursor;
                            sight::ui::base::Application::New()->exit(0, false);
                        }
                        else
                        {
                            s_is_enabled = false;
                            SIGHT_WARN("Preferences has been disabled because no password was entered.");
                            sight::ui::base::dialog::MessageDialog::show(
                                s_password_strings.cancel_title.value_or(""),
                                s_password_strings.cancel_message.value_or(""),
                                sight::ui::base::dialog::IMessageDialog::WARNING
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
                if(mustEncrypt())
                {
                    // Open the archive that holds the property tree
                    auto archive = io::zip::ArchiveReader::get(preferences_filepath);

                    // Create the input stream, with a password, allowing decoding an encrypted file
                    auto istream = archive->openFile(
                        s_preferences_file,
                        computePassword()
                    );

                    // Read the property tree from the archive
                    boost::property_tree::read_json(*istream, *s_preferences);
                }
                else
                {
                    boost::property_tree::read_json(preferences_filepath.string(), *s_preferences);
                }

                // Preferences has been read from disk
                s_is_preferences_modified = false;
                s_is_enabled              = true;

                // Reset the retry counter if the password was correct
                password_retry = 0;
            }
            catch(const io::zip::exception::BadPassword& e)
            {
                s_preferences.reset();

                bool accepted = true;

                if(s_password_keeper_policy == PasswordKeeper::PasswordPolicy::NEVER)
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
                        sight::ui::base::dialog::MessageDialog::show(
                            s_password_strings.error_title.value_or(""),
                            s_password_strings.retry_message.value_or(""),
                            sight::ui::base::dialog::IMessageDialog::WARNING
                        );
                    }

                    // Initialize the new password with the previous
                    secure_string new_password =
                        s_password_keeper
                        ? s_password_keeper->get_password()
                        : PasswordKeeper::get_global_password();

                    // Get the new password from user
                    std::tie(new_password, accepted) = sight::ui::base::dialog::InputDialog::showInputDialog(
                        s_password_strings.title.value_or(""),
                        s_password_strings.message.value_or(""),
                        new_password.c_str(), // NOLINT(readability-redundant-string-cstr)
                        sight::ui::base::dialog::InputDialog::EchoMode::PASSWORD
                    );

                    // If user accept, we check the new password again
                    if(accepted)
                    {
                        // Save the new password
                        setPasswordNolock(new_password);
                        PasswordKeeper::set_global_password(new_password);

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
                        sight::ui::base::dialog::MessageDialog::show(
                            s_password_strings.error_title.value_or(""),
                            s_password_strings.fatal_message.value_or(""),
                            sight::ui::base::dialog::IMessageDialog::CRITICAL
                        );
                    }
                    else
                    {
                        SIGHT_INFO("The user canceled. Exiting.");
                    }

                    sight::ui::base::BusyCursor cursor;
                    sight::ui::base::Application::New()->exit(0, false);
                }
                else
                {
                    if(accepted)
                    {
                        SIGHT_WARN("Preferences has been disabled because the provided password is wrong.");
                        sight::ui::base::dialog::MessageDialog::show(
                            s_password_strings.error_title.value_or(""),
                            s_password_strings.error_message.value_or(""),
                            sight::ui::base::dialog::IMessageDialog::WARNING
                        );
                    }
                    else
                    {
                        SIGHT_INFO("Preferences has been disabled because the user canceled.");
                        sight::ui::base::dialog::MessageDialog::show(
                            s_password_strings.cancel_title.value_or(""),
                            s_password_strings.cancel_message.value_or(""),
                            sight::ui::base::dialog::IMessageDialog::WARNING
                        );
                    }

                    SIGHT_THROW_EXCEPTION(BadPassword(e.what()));
                }
            }
            catch(const core::runtime::ExitException&)
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

Preferences::~Preferences()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // Check if we must save the modifications
    if(s_is_enabled && s_is_preferences_modified && !s_ignoreFilesystem)
    {
        const auto& preferences_filepath = computePreferencesFilepath();

        if(mustEncrypt())
        {
            // Delete the preferences file, since minizip is unable to replace file.
            // Doing otherwise will result with many preferences.json inside the archive, which cannot be read back
            std::filesystem::remove_all(preferences_filepath);

            // Create the archive that will hold the property tree
            auto archive = io::zip::ArchiveWriter::get(preferences_filepath);

            // Create the output stream, with a password, resulting in an encrypted file
            auto ostream = archive->openFile(
                s_preferences_file,
                computePassword()
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

void Preferences::erase(const std::string& key)
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // If the preferences are disabled or not loaded, throw an exception to disallow loading
    throw_if_disabled();

    s_preferences->erase(key);

    // Mark the tree as modified so it will be saved when Preferences is deleted
    s_is_preferences_modified = true;
}

//------------------------------------------------------------------------------

void Preferences::clear()
{
    // Protect preferences for writing
    std::unique_lock guard(s_preferences_mutex);

    // If the preferences are disabled or not loaded, throw an exception to disallow loading
    throw_if_disabled();

    s_preferences->clear();

    // Mark the tree as modified so it will be saved when Preferences is deleted
    s_is_preferences_modified = true;
}

//------------------------------------------------------------------------------

void Preferences::set_enabled(bool enable)
{
    std::unique_lock guard(s_preferences_mutex);
    s_is_enabled = enable;

    if(!enable)
    {
        s_preferences.reset();
        s_is_preferences_modified = false;
    }
}

//------------------------------------------------------------------------------

void Preferences::ignoreFilesystem(bool ignore)
{
    std::unique_lock guard(s_preferences_mutex);
    s_ignoreFilesystem = ignore;
}

//------------------------------------------------------------------------------

void Preferences::throw_if_disabled()
{
    SIGHT_THROW_EXCEPTION_IF(
        PreferencesDisabled("Preferences are disabled"),
        !s_is_enabled || !s_preferences
    );
}

//------------------------------------------------------------------------------

void Preferences::set_password(const core::crypto::secure_string& password)
{
    std::unique_lock guard(s_preferences_mutex);
    setPasswordNolock(password);
}

//------------------------------------------------------------------------------

void Preferences::set_password_policy(const core::crypto::PasswordKeeper::PasswordPolicy policy)
{
    std::unique_lock guard(s_preferences_mutex);
    s_password_keeper_policy = policy;
}

//------------------------------------------------------------------------------

void Preferences::set_encryption_policy(const core::crypto::PasswordKeeper::EncryptionPolicy policy)
{
    std::unique_lock guard(s_preferences_mutex);
    s_encryption_policy = policy;
}

//------------------------------------------------------------------------------

void Preferences::exit_on_password_error(bool exit)
{
    std::unique_lock guard(s_preferences_mutex);
    s_exit_on_password_error = exit;
}

//------------------------------------------------------------------------------

void Preferences::set_password_dialog_strings(const PasswordDialogStrings& strings)
{
    std::unique_lock guard(s_preferences_mutex);

    if(strings.title)
    {
        s_password_strings.title = strings.title;
    }

    if(strings.message)
    {
        s_password_strings.message = strings.message;
    }

    if(strings.new_title)
    {
        s_password_strings.new_title = strings.new_title;
    }

    if(strings.new_message)
    {
        s_password_strings.new_message = strings.new_message;
    }

    if(strings.weak_title)
    {
        s_password_strings.weak_title = strings.weak_title;
    }

    if(strings.error_title)
    {
        s_password_strings.error_title = strings.error_title;
    }

    if(strings.error_message)
    {
        s_password_strings.error_message = strings.error_message;
    }

    if(strings.fatal_message)
    {
        s_password_strings.fatal_message = strings.fatal_message;
    }

    if(strings.retry_message)
    {
        s_password_strings.retry_message = strings.retry_message;
    }

    if(strings.cancel_title)
    {
        s_password_strings.cancel_title = strings.cancel_title;
    }

    if(strings.cancel_message)
    {
        s_password_strings.cancel_message = strings.cancel_message;
    }

    if(strings.cancel_fatal_message)
    {
        s_password_strings.cancel_fatal_message = strings.cancel_fatal_message;
    }
}

//------------------------------------------------------------------------------

void Preferences::set_password_validator(
    std::function<std::pair<bool, std::string>(const sight::core::crypto::secure_string&)> validator
)
{
    std::unique_lock guard(s_preferences_mutex);
    s_password_validator = std::move(validator);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
