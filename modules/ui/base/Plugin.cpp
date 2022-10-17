/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <core/tools/Os.hpp>

#include <service/base.hpp>

#include <ui/base/Preferences.hpp>

namespace sight::module::ui::base
{

using sight::core::crypto::secure_string;
using sight::core::crypto::PasswordKeeper;
using sight::ui::base::Preferences;

constexpr static auto s_PREFERENCES_ENABLED           = "preferences_enabled";
constexpr static auto s_PREFERENCES_PASSWORD_POLICY   = "preferences_password_policy";
constexpr static auto s_PREFERENCES_ENCRYPTION_POLICY = "preferences_encryption_policy";
constexpr static auto s_PREFERENCES_PASSWORD          = "preferences_password";

SIGHT_REGISTER_PLUGIN("sight::module::ui::base::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept =
    default;

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
        ? PasswordKeeper::PasswordPolicy::DEFAULT
        : PasswordKeeper::string_to_password_policy(module->getParameterValue(s_PREFERENCES_PASSWORD_POLICY));

    SIGHT_THROW_IF("Invalid password policy.", password_policy == PasswordKeeper::PasswordPolicy::INVALID);
    Preferences::set_password_policy(password_policy);

    // Set the encryption policy
    const PasswordKeeper::EncryptionPolicy encryption_policy =
        !module->hasParameter(s_PREFERENCES_ENCRYPTION_POLICY)
        ? PasswordKeeper::EncryptionPolicy::DEFAULT
        : PasswordKeeper::string_to_encryption_policy(module->getParameterValue(s_PREFERENCES_ENCRYPTION_POLICY));

    SIGHT_THROW_IF("Invalid encryption policy.", encryption_policy == PasswordKeeper::EncryptionPolicy::INVALID);
    Preferences::set_encryption_policy(encryption_policy);

    // Set an hardcoded password
    if(module->hasParameter(s_PREFERENCES_PASSWORD))
    {
        // NOLINTNEXTLINE(readability-redundant-string-cstr)
        const secure_string& password = module->getParameterValue(s_PREFERENCES_PASSWORD).c_str();
        Preferences::set_password(password);
    }
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    Preferences::set_enabled(false);
}

} // namespace sight::module::ui::base
