/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include "modules/ui/base/Plugin.hpp"

#include <core/crypto/PasswordKeeper.hpp>
#include <core/runtime/path.hpp>
#include <core/tools/Os.hpp>

#include <service/base.hpp>

#include <ui/base/Preferences.hpp>

namespace sight::module::ui::base
{

using sight::core::crypto::secure_string;
using sight::core::crypto::PasswordKeeper;
using sight::ui::base::Preferences;

constexpr static auto s_PREFERENCES_ENABLED                     = "preferences_enabled";
constexpr static auto s_PREFERENCES_PASSWORD_POLICY             = "preferences_password_policy";
constexpr static auto s_PREFERENCES_ENCRYPTION_POLICY           = "preferences_encryption_policy";
constexpr static auto s_PREFERENCES_PASSWORD                    = "preferences_password";
constexpr static auto s_PREFERENCES_EXIT_ON_PASSWORD_ERROR      = "preferences_exit_on_password_error";
constexpr static auto S_PREFERENCES_PASSWORD_DIALOG_TITLE       = "preferences_password_dialog_title";
constexpr static auto S_PREFERENCES_PASSWORD_DIALOG_MESSAGE     = "preferences_password_dialog_message";
constexpr static auto S_PREFERENCES_PASSWORD_DIALOG_ICON        = "preferences_password_dialog_icon";
constexpr static auto S_PREFERENCES_NEW_PASSWORD_DIALOG_TITLE   = "preferences_new_password_dialog_title";
constexpr static auto S_PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE = "preferences_new_password_dialog_message";
constexpr static auto S_PREFERENCES_NEW_PASSWORD_DIALOG_ICON    = "preferences_new_password_dialog_icon";

SIGHT_REGISTER_PLUGIN("sight::module::ui::base::Plugin");

//-----------------------------------------------------------------------------

void Plugin::start()
{
    const auto module = getModule();

    // By default enable preferences
    const bool enabled = !module->hasParameter(s_PREFERENCES_ENABLED)
                         || module->getParameterValue(s_PREFERENCES_ENABLED) != "false";

    Preferences::set_enabled(enabled);

    // Set the password policy
    const PasswordKeeper::PasswordPolicy password_policy =
        !module->hasParameter(s_PREFERENCES_PASSWORD_POLICY)
        ? PasswordKeeper::PasswordPolicy::NEVER
        : PasswordKeeper::string_to_password_policy(module->getParameterValue(s_PREFERENCES_PASSWORD_POLICY));

    SIGHT_THROW_IF("Invalid password policy.", password_policy == PasswordKeeper::PasswordPolicy::INVALID);
    Preferences::set_password_policy(password_policy);

    // Set the encryption policy
    const PasswordKeeper::EncryptionPolicy encryption_policy =
        !module->hasParameter(s_PREFERENCES_ENCRYPTION_POLICY)
        ? PasswordKeeper::EncryptionPolicy::PASSWORD
        : PasswordKeeper::string_to_encryption_policy(module->getParameterValue(s_PREFERENCES_ENCRYPTION_POLICY));

    SIGHT_THROW_IF("Invalid encryption policy.", encryption_policy == PasswordKeeper::EncryptionPolicy::INVALID);
    Preferences::set_encryption_policy(encryption_policy);

    // Set an hardcoded password
    if(module->hasParameter(s_PREFERENCES_PASSWORD))
    {
        // NOLINTNEXTLINE(readability-redundant-string-cstr)
        Preferences::set_password(module->getParameterValue(s_PREFERENCES_PASSWORD).c_str());
    }

    if(module->hasParameter(s_PREFERENCES_EXIT_ON_PASSWORD_ERROR))
    {
        Preferences::exit_on_password_error(module->getParameterValue(s_PREFERENCES_EXIT_ON_PASSWORD_ERROR) != "false");
    }

    if(module->hasParameter(S_PREFERENCES_PASSWORD_DIALOG_TITLE))
    {
        Preferences::set_password_dialog_strings(
            {.title = module->getParameterValue(S_PREFERENCES_PASSWORD_DIALOG_TITLE)
            });
    }

    if(module->hasParameter(S_PREFERENCES_NEW_PASSWORD_DIALOG_TITLE))
    {
        Preferences::set_password_dialog_strings(
            {.new_title = module->getParameterValue(S_PREFERENCES_NEW_PASSWORD_DIALOG_TITLE)
            });
    }

    if(module->hasParameter(S_PREFERENCES_PASSWORD_DIALOG_ICON))
    {
        const auto& icon_path = core::runtime::getModuleResourceFilePath(
            module->getParameterValue(S_PREFERENCES_PASSWORD_DIALOG_ICON)
        );

        std::string message = "<img src='" + icon_path.string() + "' />";

        if(module->hasParameter(S_PREFERENCES_PASSWORD_DIALOG_MESSAGE))
        {
            message += "<br><strong>" + module->getParameterValue(S_PREFERENCES_PASSWORD_DIALOG_MESSAGE) + "</strong>";
        }

        Preferences::set_password_dialog_strings({.message = message});
    }
    else if(module->hasParameter(S_PREFERENCES_PASSWORD_DIALOG_MESSAGE))
    {
        Preferences::set_password_dialog_strings(
            {.message = module->getParameterValue(S_PREFERENCES_PASSWORD_DIALOG_MESSAGE)
            });
    }

    if(module->hasParameter(S_PREFERENCES_NEW_PASSWORD_DIALOG_ICON))
    {
        const auto& icon_path = core::runtime::getModuleResourceFilePath(
            module->getParameterValue(S_PREFERENCES_NEW_PASSWORD_DIALOG_ICON)
        );

        std::string message = "<img src='" + icon_path.string() + "' />";

        if(module->hasParameter(S_PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE))
        {
            message += "<br><strong>"
                       + module->getParameterValue(S_PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE)
                       + "</strong>";
        }

        Preferences::set_password_dialog_strings({.new_message = message});
    }
    else if(module->hasParameter(S_PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE))
    {
        Preferences::set_password_dialog_strings(
            {.new_message = module->getParameterValue(S_PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE)
            });
    }
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    Preferences::set_enabled(false);
}

} // namespace sight::module::ui::base
