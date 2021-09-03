/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "io/session/config.hpp"

#include <core/crypto/secure_string.hpp>
#include <core/macros.hpp>

namespace sight::io::session
{

/// Class designed to hold and check in a somewhat secure way a global password and "instance" passwords.
class IO_SESSION_CLASS_API PasswordKeeper final
{
public:

    SIGHT_DECLARE_CLASS(PasswordKeeper);

    /// Enum to define a password policy
    enum class PasswordPolicy : uint8_t
    {
        NEVER   = 0,     /// Never use a password
        ONCE    = 1,     /// Ask for password once and reuse it later
        ALWAYS  = 2,     /// Always ask for a password
        DEFAULT = NEVER, /// Default behavior if nothing is set
        INVALID = 255    /// Used for error management
    };

    /// Enum to define a encryption policy
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
    IO_SESSION_API PasswordKeeper() noexcept;

    /// Default destructor
    IO_SESSION_API ~PasswordKeeper() noexcept;

    /// Gets the global password hash
    IO_SESSION_API static core::crypto::secure_string getGlobalPasswordHash();

    /// Gets the global password
    IO_SESSION_API static core::crypto::secure_string getGlobalPassword();

    /// Sets the global password
    /// @param password the new global password
    IO_SESSION_API static void setGlobalPassword(const core::crypto::secure_string& password);

    /// Returns true if the password matches
    /// @param password the password to verify against global stored password
    IO_SESSION_API static bool checkGlobalPassword(const core::crypto::secure_string& password);

    /// Gets the password
    IO_SESSION_API core::crypto::secure_string getPasswordHash() const;

    /// Gets the password
    IO_SESSION_API core::crypto::secure_string getPassword() const;

    /// Sets the password
    /// @param password the new password
    IO_SESSION_API void setPassword(const core::crypto::secure_string& password);

    /// Returns true if the password matches
    /// @param password the password to verify against stored password
    IO_SESSION_API bool checkPassword(const core::crypto::secure_string& password) const;

    /// Convenience function to convert from PasswordPolicy enum value to string
    constexpr static std::string_view passwordPolicyToString(PasswordPolicy policy) noexcept
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
    constexpr static PasswordPolicy stringToPasswordPolicy(std::string_view policy) noexcept
    {
        if(constexpr auto NEVER = passwordPolicyToString(PasswordPolicy::NEVER); policy == NEVER)
        {
            return PasswordPolicy::NEVER;
        }
        else if(constexpr auto ONCE = passwordPolicyToString(PasswordPolicy::ONCE); policy == ONCE)
        {
            return PasswordPolicy::ONCE;
        }
        else if(constexpr auto ALWAYS = passwordPolicyToString(PasswordPolicy::ALWAYS); policy == ALWAYS)
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
    constexpr static std::string_view encryptionPolicyToString(EncryptionPolicy policy) noexcept
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
    constexpr static EncryptionPolicy stringToEncryptionPolicy(std::string_view policy) noexcept
    {
        if(constexpr auto PASSWORD = encryptionPolicyToString(EncryptionPolicy::PASSWORD); policy == PASSWORD)
        {
            return EncryptionPolicy::PASSWORD;
        }
        else if(constexpr auto SALTED = encryptionPolicyToString(EncryptionPolicy::SALTED); policy == SALTED)
        {
            return EncryptionPolicy::SALTED;
        }
        else if(constexpr auto FORCED = encryptionPolicyToString(EncryptionPolicy::FORCED); policy == FORCED)
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

private:

    /// PImpl
    class PasswordKeeperImpl;
    std::unique_ptr<PasswordKeeperImpl> m_pimpl;
};

} // namespace sight::io::session
