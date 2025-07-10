/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include <core/crypto/obfuscated_string.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/crypto/sha256.hpp>
#include <core/macros.hpp>

namespace sight::core::crypto
{

/// Class designed to hold and check in a somewhat secure way a global password and "instance" passwords.
class SIGHT_CORE_CLASS_API password_keeper final
{
public:

    SIGHT_DECLARE_CLASS(password_keeper);

    /// Enum to define a password policy
    enum class password_policy : uint8_t
    {
        never   = 0,  /// Never ask a password
        global  = 1,  /// Use global password if set, Otherwise ask for password
        always  = 2,  /// Always ask for a password
        invalid = 255 /// Used for error management
    };

    /// Enum to define an encryption policy
    enum class encryption_policy : uint8_t
    {
        password = 0,  /// Use the given password for encryption
        salted   = 1,  /// Use the given password with salt for encryption
        forced   = 2,  /// Force encryption with a pseudo random hidden password
        invalid  = 255 /// Used for error management
    };

    /// Delete default copy constructors and assignment operators
    password_keeper(const password_keeper&)            = delete;
    password_keeper(password_keeper&&)                 = delete;
    password_keeper& operator=(const password_keeper&) = delete;
    password_keeper& operator=(password_keeper&&)      = delete;

    /// Default constructor
    SIGHT_CORE_API password_keeper() noexcept;

    /// Default destructor
    SIGHT_CORE_API ~password_keeper() noexcept;

    /// Gets the global password hash
    SIGHT_CORE_API static core::crypto::secure_string get_global_password_hash();

    /// Gets the global password
    SIGHT_CORE_API static core::crypto::secure_string get_global_password();

    /// Sets the global password
    /// @param _password the new global password
    SIGHT_CORE_API static void set_global_password(
        const core::crypto::secure_string& _password,
        bool _restart_logger = true
    );

    /// Returns true if the password matches
    /// @param _password the password to verify against global stored password
    SIGHT_CORE_API static bool check_global_password(const core::crypto::secure_string& _password);

    /// Reset the global password
    SIGHT_CORE_API static void reset_global_password();

    /// Gets the password
    [[nodiscard]] SIGHT_CORE_API core::crypto::secure_string get_password_hash() const;

    /// Gets the password
    [[nodiscard]] SIGHT_CORE_API core::crypto::secure_string get_password() const;

    /// Reset the password
    SIGHT_CORE_API void reset_password();

    /// Sets the password
    /// @param _password the new password
    SIGHT_CORE_API void set_password(const core::crypto::secure_string& _password);

    /// Returns true if the password matches
    /// @param _password the password to verify against stored password
    [[nodiscard]] SIGHT_CORE_API bool check_password(const core::crypto::secure_string& _password) const;

    /// Convenience function to convert from password_policy enum value to string
    constexpr static std::string_view password_policy_to_string(password_policy _policy) noexcept
    {
        switch(_policy)
        {
            case password_policy::global:
                return "global";

            case password_policy::always:
                return "always";

            default:
                return "never";
        }
    }

    /// Convenience function to convert from string to password_policy enum value
    constexpr static password_policy string_to_password_policy(std::string_view _policy) noexcept
    {
        if(constexpr auto never = password_policy_to_string(password_policy::never);
           _policy == never || _policy.empty() || _policy == "default")
        {
            return password_policy::never;
        }

        if(constexpr auto global = password_policy_to_string(password_policy::global);
           _policy == global || _policy == "once")
        {
            return password_policy::global;
        }

        if(constexpr auto always = password_policy_to_string(password_policy::always); _policy == always)
        {
            return password_policy::always;
        }

        // Error case
        return password_policy::invalid;
    }

    /// Convenience function to convert from encryption_policy enum value to string
    constexpr static std::string_view encryption_policy_to_string(encryption_policy _policy) noexcept
    {
        switch(_policy)
        {
            case encryption_policy::salted:
                return "salted";

            case encryption_policy::forced:
                return "forced";

            default:
                return "password";
        }
    }

    /// Convenience function to convert from string to encryption_policy enum value
    constexpr static encryption_policy string_to_encryption_policy(std::string_view _policy) noexcept
    {
        if(constexpr auto password = encryption_policy_to_string(encryption_policy::password);
           _policy == password || _policy.empty() || _policy == "default")
        {
            return encryption_policy::password;
        }

        if(constexpr auto salted = encryption_policy_to_string(encryption_policy::salted); _policy == salted)
        {
            return encryption_policy::salted;
        }

        if(constexpr auto forced = encryption_policy_to_string(encryption_policy::forced); _policy == forced)
        {
            return encryption_policy::forced;
        }

        // Error case
        return encryption_policy::invalid;
    }

    /// Returns a pseudo-random password
    SIGHT_CORE_API static secure_string get_pseudo_password_hash(const secure_string& _salt) noexcept;

private:

    /// PImpl
    class password_keeper_impl;
    std::unique_ptr<password_keeper_impl> m_pimpl;
};

} // namespace sight::core::crypto
