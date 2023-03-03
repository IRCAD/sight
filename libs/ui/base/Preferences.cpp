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

// current number of retry
static std::size_t s_password_retry {0};

/// Password dialog configuration
/// @{
static std::string s_password_title {"Enter Password"};
static std::string s_password_message {"Password:"};
static std::string s_password_error_title {"Invalid password"};
static std::string s_password_error_message {
    "Invalid password. The number of tries is exceeded and the application will close."
};
static std::string s_password_retry_message {
    "The provided password is wrong.\n\n"
    "Retry with a different password ?"
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

    const std::function<void(void)> load =
        [&]
        {
            // Create and load the preferences file if not already done
            if(s_is_enabled && !s_preferences)
            {
                if(s_ignoreFilesystem)
                {
                    // Don't read a preference file, simply create an empty preferences
                    s_preferences = std::make_unique<boost::property_tree::ptree>();
                    return;
                }

                // Set the password to use
                // NEVER policy means we never ask for a password and only rely on manually set
                if(s_password_keeper_policy != PasswordKeeper::PasswordPolicy::NEVER)
                {
                    const secure_string& password =
                        s_password_keeper
                        ? s_password_keeper->get_password()
                        : PasswordKeeper::get_global_password();

                    if(s_password_retry != 0
                       || (s_password_keeper_policy == PasswordKeeper::PasswordPolicy::ALWAYS)
                       || (s_password_keeper_policy == PasswordKeeper::PasswordPolicy::GLOBAL
                           && password.empty()))
                    {
                        const auto& [new_password, ok] = sight::ui::base::dialog::InputDialog::showInputDialog(
                            s_password_title,
                            s_password_message,
                            password.c_str(), // NOLINT(readability-redundant-string-cstr)
                            sight::ui::base::dialog::InputDialog::EchoMode::PASSWORD
                        );

                        if(ok)
                        {
                            const secure_string secure_password(new_password);
                            setPasswordNolock(secure_password);
                            PasswordKeeper::set_global_password(secure_password);
                        }
                        else if(s_exit_on_password_error)
                        {
                            SIGHT_WARN("User canceled. Exiting.");
                            sight::ui::base::BusyCursor cursor;
                            sight::ui::base::Application::New()->exit(0);
                        }
                        else
                        {
                            s_is_enabled = false;
                            SIGHT_WARN("Preferences has been disabled because no password was entered.");
                            return;
                        }
                    }
                    else if(!s_password_keeper)
                    {
                        setPasswordNolock(password);
                    }
                }

                try
                {
                    s_preferences = std::make_unique<boost::property_tree::ptree>();

                    const auto& preferences_filepath = computePreferencesFilepath();

                    if(std::filesystem::exists(preferences_filepath))
                    {
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
                    }
                    else
                    {
                        // Create an empty preferences
                        s_is_preferences_modified = true;
                    }

                    // Reset the retry counter if the password was correct
                    s_password_retry = 0;
                }
                catch(const io::zip::exception::BadPassword& e)
                {
                    s_preferences.reset();
                    ++s_password_retry;

                    if(s_password_retry > s_max_retry)
                    {
                        if(s_password_keeper_policy != PasswordKeeper::PasswordPolicy::NEVER)
                        {
                            sight::ui::base::dialog::MessageDialog messageDialog;
                            messageDialog.setTitle(s_password_error_title);
                            messageDialog.setMessage(s_password_error_message);
                            messageDialog.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
                            messageDialog.addButton(ui::base::dialog::IMessageDialog::OK);
                            messageDialog.show();
                        }
                    }
                    else
                    {
                        if(s_password_keeper_policy != PasswordKeeper::PasswordPolicy::NEVER)
                        {
                            sight::ui::base::dialog::MessageDialog messageDialog;
                            messageDialog.setTitle(s_password_error_title);
                            messageDialog.setMessage(s_password_retry_message);
                            messageDialog.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
                            messageDialog.addButton(ui::base::dialog::IMessageDialog::RETRY);
                            messageDialog.addButton(ui::base::dialog::IMessageDialog::CANCEL);

                            if(messageDialog.show() == sight::ui::base::dialog::IMessageDialog::RETRY)
                            {
                                // Retry...
                                load();
                                return;
                            }

                            s_password_retry = 0;
                        }
                        else
                        {
                            // Give a chance to retry with a different password
                            SIGHT_THROW_EXCEPTION(BadPassword(e.what()));
                        }
                    }

                    // Too much retries or canceled
                    s_is_enabled = false;

                    if(s_exit_on_password_error)
                    {
                        SIGHT_WARN("The provided password is wrong. Exiting.");
                        sight::ui::base::BusyCursor cursor;
                        sight::ui::base::Application::New()->exit(0);
                    }
                    else
                    {
                        SIGHT_WARN("Preferences has been disabled because the password is wrong.");
                        SIGHT_THROW_EXCEPTION(BadPassword(e.what()));
                    }
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
            }
        };

    // Call the lambda !
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

void Preferences::set_password_dialog_title(const std::string& title)
{
    std::unique_lock guard(s_preferences_mutex);
    s_password_title = title;
}

//------------------------------------------------------------------------------

void Preferences::set_password_dialog_message(const std::string& message)
{
    std::unique_lock guard(s_preferences_mutex);
    s_password_message = message;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
