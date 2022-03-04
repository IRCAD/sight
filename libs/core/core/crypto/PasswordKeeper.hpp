/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "core/config.hpp"

#include <core/crypto/obfuscated_string.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/crypto/SHA256.hpp>
#include <core/macros.hpp>

namespace sight::core::crypto
{

/// Class designed to hold and check in a somewhat secure way a global password and "instance" passwords.
class CORE_CLASS_API PasswordKeeper final
{
public:

    SIGHT_DECLARE_CLASS(PasswordKeeper);

    /// Enum to define a password policy
    enum class PasswordPolicy : uint8_t
    {
        NEVER   = 0,     /// Never ask a password
        ONCE    = 1,     /// Ask for password once and reuse it later
        ALWAYS  = 2,     /// Always ask for a password
        DEFAULT = NEVER, /// Default behavior if nothing is set
        INVALID = 255    /// Used for error management
    };

    /// Enum to define an encryption policy
    enum class EncryptionPolicy : uint8_t
    {
        PASSWORD = 0,        /// Use the given password for encryption
        SALTED   = 1,        /// Use the given password with salt for encryption
        FORCED   = 2,        /// Force encryption with a pseudo random hidden password
        DEFAULT  = PASSWORD, /// Default behavior if nothing is set
        INVALID  = 255       /// Used for error management
    };

    /// Delete default copy constructors and assignment operators
    PasswordKeeper(const PasswordKeeper&)            = delete;
    PasswordKeeper(PasswordKeeper&&)                 = delete;
    PasswordKeeper& operator=(const PasswordKeeper&) = delete;
    PasswordKeeper& operator=(PasswordKeeper&&)      = delete;

    /// Default constructor
    CORE_API PasswordKeeper() noexcept;

    /// Default destructor
    CORE_API ~PasswordKeeper() noexcept;

    /// Gets the global password hash
    CORE_API static core::crypto::secure_string get_global_password_hash();

    /// Gets the global password
    CORE_API static core::crypto::secure_string get_global_password();

    /// Sets the global password
    /// @param password the new global password
    CORE_API static void set_global_password(const core::crypto::secure_string& password, bool restart_logger = true);

    /// Returns true if the password matches
    /// @param password the password to verify against global stored password
    CORE_API static bool check_global_password(const core::crypto::secure_string& password);

    /// Reset the global password
    CORE_API static void reset_global_password();

    /// Gets the password
    CORE_API core::crypto::secure_string get_password_hash() const;

    /// Gets the password
    CORE_API core::crypto::secure_string get_password() const;

    /// Reset the password
    CORE_API void reset_password();

    /// Sets the password
    /// @param password the new password
    CORE_API void set_password(const core::crypto::secure_string& password);

    /// Returns true if the password matches
    /// @param password the password to verify against stored password
    CORE_API bool check_password(const core::crypto::secure_string& password) const;

    /// Convenience function to convert from PasswordPolicy enum value to string
    constexpr static std::string_view password_policy_to_string(PasswordPolicy policy) noexcept
    {
        switch(policy)
        {
            case PasswordPolicy::NEVER:
                return "never";

            case PasswordPolicy::ONCE:
                return "once";

            case PasswordPolicy::ALWAYS:
                return "always";

            default:
                return "default";
        }
    }

    /// Convenience function to convert from string to PasswordPolicy enum value
    constexpr static PasswordPolicy string_to_password_policy(std::string_view policy) noexcept
    {
        if(constexpr auto NEVER = password_policy_to_string(PasswordPolicy::NEVER); policy == NEVER)
        {
            return PasswordPolicy::NEVER;
        }
        else if(constexpr auto ONCE = password_policy_to_string(PasswordPolicy::ONCE); policy == ONCE)
        {
            return PasswordPolicy::ONCE;
        }
        else if(constexpr auto ALWAYS = password_policy_to_string(PasswordPolicy::ALWAYS); policy == ALWAYS)
        {
            return PasswordPolicy::ALWAYS;
        }
        else if(policy.empty() || policy == "default")
        {
            return PasswordPolicy::DEFAULT;
        }
        else
        {
            // Error case
            return PasswordPolicy::INVALID;
        }
    }

    /// Convenience function to convert from EncryptionPolicy enum value to string
    constexpr static std::string_view encryption_policy_to_string(EncryptionPolicy policy) noexcept
    {
        switch(policy)
        {
            case EncryptionPolicy::PASSWORD:
                return "password";

            case EncryptionPolicy::SALTED:
                return "salted";

            case EncryptionPolicy::FORCED:
                return "forced";

            default:
                return "default";
        }
    }

    /// Convenience function to convert from string to EncryptionPolicy enum value
    constexpr static EncryptionPolicy string_to_encryption_policy(std::string_view policy) noexcept
    {
        if(constexpr auto PASSWORD = encryption_policy_to_string(EncryptionPolicy::PASSWORD); policy == PASSWORD)
        {
            return EncryptionPolicy::PASSWORD;
        }
        else if(constexpr auto SALTED = encryption_policy_to_string(EncryptionPolicy::SALTED); policy == SALTED)
        {
            return EncryptionPolicy::SALTED;
        }
        else if(constexpr auto FORCED = encryption_policy_to_string(EncryptionPolicy::FORCED); policy == FORCED)
        {
            return EncryptionPolicy::FORCED;
        }
        else if(policy.empty() || policy == "default")
        {
            return EncryptionPolicy::DEFAULT;
        }
        else
        {
            // Error case
            return EncryptionPolicy::INVALID;
        }
    }

    //------------------------------------------------------------------------------

    constexpr static bool has_default_password() noexcept
    {
#if defined(SIGHT_DEFAULT_PASSWORD)
        if constexpr(SIGHT_DEFAULT_PASSWORD[0] != '\0')
        {
            return true;
        }
        else
#endif
        {
            return false;
        }
    }

    /// Returns the compile-time hardcoded password
    FINLINE static secure_string get_default_password()
    {
#if defined(SIGHT_DEFAULT_PASSWORD)
        if constexpr(has_default_password())
        {
            constexpr auto obfuscated_password = OBFUSCATED_STR((SIGHT_DEFAULT_PASSWORD));
            return secure_string(obfuscated_password);
        }
        else
#endif
        {
            throw std::runtime_error("No default password");
        }
    }

    /// Returns a pseudo-random password
    CORE_API static secure_string get_pseudo_password_hash(const secure_string& salt) noexcept;

private:

    /// PImpl
    class PasswordKeeperImpl;
    std::unique_ptr<PasswordKeeperImpl> m_pimpl;
};

} // namespace sight::core::crypto
