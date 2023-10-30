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

#include "modules/ui/__/plugin.hpp"

#include <core/crypto/password_keeper.hpp>
#include <core/runtime/path.hpp>
#include <core/tools/os.hpp>

#include <service/op.hpp>

#include <ui/__/preferences.hpp>

namespace sight::module::ui
{

using sight::core::crypto::secure_string;
using sight::core::crypto::password_keeper;
using sight::ui::preferences;

constexpr static auto PREFERENCES_ENABLED                     = "preferences_enabled";
constexpr static auto PREFERENCES_PASSWORD_POLICY             = "preferences_password_policy";
constexpr static auto PREFERENCES_ENCRYPTION_POLICY           = "preferences_encryption_policy";
constexpr static auto PREFERENCES_PASSWORD                    = "preferences_password";
constexpr static auto PREFERENCES_EXIT_ON_PASSWORD_ERROR      = "preferences_exit_on_password_error";
constexpr static auto PREFERENCES_PASSWORD_DIALOG_TITLE       = "preferences_password_dialog_title";
constexpr static auto PREFERENCES_PASSWORD_DIALOG_MESSAGE     = "preferences_password_dialog_message";
constexpr static auto PREFERENCES_PASSWORD_DIALOG_ICON        = "preferences_password_dialog_icon";
constexpr static auto PREFERENCES_NEW_PASSWORD_DIALOG_TITLE   = "preferences_new_password_dialog_title";
constexpr static auto PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE = "preferences_new_password_dialog_message";
constexpr static auto PREFERENCES_NEW_PASSWORD_DIALOG_ICON    = "preferences_new_password_dialog_icon";

SIGHT_REGISTER_PLUGIN("sight::module::ui::plugin");

//-----------------------------------------------------------------------------

void plugin::start()
{
    const auto module = get_module();

    // By default enable preferences
    const bool enabled = !module->has_parameter(PREFERENCES_ENABLED)
                         || module->get_parameter_value(PREFERENCES_ENABLED) != "false";

    preferences::set_enabled(enabled);

    // Set the password policy
    const password_keeper::password_policy password_policy =
        !module->has_parameter(PREFERENCES_PASSWORD_POLICY)
        ? password_keeper::password_policy::never
        : password_keeper::string_to_password_policy(module->get_parameter_value(PREFERENCES_PASSWORD_POLICY));

    SIGHT_THROW_IF("Invalid password policy.", password_policy == password_keeper::password_policy::invalid);
    preferences::set_password_policy(password_policy);

    // Set the encryption policy
    const password_keeper::encryption_policy encryption_policy =
        !module->has_parameter(PREFERENCES_ENCRYPTION_POLICY)
        ? password_keeper::encryption_policy::password
        : password_keeper::string_to_encryption_policy(module->get_parameter_value(PREFERENCES_ENCRYPTION_POLICY));

    SIGHT_THROW_IF("Invalid encryption policy.", encryption_policy == password_keeper::encryption_policy::invalid);
    preferences::set_encryption_policy(encryption_policy);

    // Set an hardcoded password
    if(module->has_parameter(PREFERENCES_PASSWORD))
    {
        // NOLINTNEXTLINE(readability-redundant-string-cstr)
        preferences::set_password(module->get_parameter_value(PREFERENCES_PASSWORD).c_str());
    }

    if(module->has_parameter(PREFERENCES_EXIT_ON_PASSWORD_ERROR))
    {
        preferences::exit_on_password_error(
            module->get_parameter_value(PREFERENCES_EXIT_ON_PASSWORD_ERROR)
            != "false"
        );
    }

    if(module->has_parameter(PREFERENCES_PASSWORD_DIALOG_TITLE))
    {
        preferences::set_password_dialog_strings(
            {.title = module->get_parameter_value(PREFERENCES_PASSWORD_DIALOG_TITLE)
            });
    }

    if(module->has_parameter(PREFERENCES_NEW_PASSWORD_DIALOG_TITLE))
    {
        preferences::set_password_dialog_strings(
            {.new_title = module->get_parameter_value(PREFERENCES_NEW_PASSWORD_DIALOG_TITLE)
            });
    }

    if(module->has_parameter(PREFERENCES_PASSWORD_DIALOG_ICON))
    {
        const auto& icon_path = core::runtime::get_module_resource_file_path(
            module->get_parameter_value(PREFERENCES_PASSWORD_DIALOG_ICON)
        );

        std::string message = "<img src='" + icon_path.string() + "' />";

        if(module->has_parameter(PREFERENCES_PASSWORD_DIALOG_MESSAGE))
        {
            message += "<br><strong>" + module->get_parameter_value(PREFERENCES_PASSWORD_DIALOG_MESSAGE)
                       + "</strong>";
        }

        preferences::set_password_dialog_strings({.message = message});
    }
    else if(module->has_parameter(PREFERENCES_PASSWORD_DIALOG_MESSAGE))
    {
        preferences::set_password_dialog_strings(
            {.message = module->get_parameter_value(PREFERENCES_PASSWORD_DIALOG_MESSAGE)
            });
    }

    if(module->has_parameter(PREFERENCES_NEW_PASSWORD_DIALOG_ICON))
    {
        const auto& icon_path = core::runtime::get_module_resource_file_path(
            module->get_parameter_value(PREFERENCES_NEW_PASSWORD_DIALOG_ICON)
        );

        std::string message = "<img src='" + icon_path.string() + "' />";

        if(module->has_parameter(PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE))
        {
            message += "<br><strong>"
                       + module->get_parameter_value(PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE)
                       + "</strong>";
        }

        preferences::set_password_dialog_strings({.new_message = message});
    }
    else if(module->has_parameter(PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE))
    {
        preferences::set_password_dialog_strings(
            {.new_message = module->get_parameter_value(PREFERENCES_NEW_PASSWORD_DIALOG_MESSAGE)
            });
    }
}

//-----------------------------------------------------------------------------

void plugin::stop() noexcept
{
    preferences::set_enabled(false);
}

} // namespace sight::module::ui
