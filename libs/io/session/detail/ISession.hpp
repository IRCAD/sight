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

#include "io/session/config.hpp"

#include <core/crypto/PasswordKeeper.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/location/ILocation.hpp>

#include <filesystem>

namespace sight::io::session
{

namespace detail
{

/// Interface to define a location that holds what is needed to de/serialize a session.
class IO_SESSION_CLASS_API ISession : public core::location::ILocation
{
public:

    SIGHT_DECLARE_CLASS(ISession, core::location::ILocation);

    constexpr static auto s_uuid {"uuid"};
    constexpr static auto s_children {"children"};
    constexpr static auto s_fields {"fields"};

    /// String serialization function
    inline std::string toString() const override;

    /// Return the default index file path inside the session archive
    inline std::filesystem::path getIndexFilePath() const;

    /// Salt the password, depending of the encryption level
    /// "PASSWORD" means encrypt if a password is provided, using the same key.
    /// "SALTED" means encrypt if a password is provided, but use a deterministically salted key, which will make hard
    ///   to open the archive session outside sight
    /// "FORCED" means always encrypt and use a salted key. The password will be deterministically guessed
    /// @param password original password
    /// @param salt salt used to change a bit the password
    /// @param level the way the password is changed
    /// @param policy the encryption policy: @see sight::io::session::PasswordKeeper::EncryptionPolicy
    static inline core::crypto::secure_string pickle(
        const core::crypto::secure_string& password,
        const core::crypto::secure_string& salt,
        const core::crypto::PasswordKeeper::EncryptionPolicy policy = core::crypto::PasswordKeeper::EncryptionPolicy::DEFAULT
    );

protected:

    /// Constructor
    IO_SESSION_API ISession() = default;

    /// Destructor
    IO_SESSION_API virtual ~ISession() = default;
};

//------------------------------------------------------------------------------

inline std::string ISession::toString() const
{
    return core::location::ILocation::toString();
}

//------------------------------------------------------------------------------

inline std::filesystem::path ISession::getIndexFilePath() const
{
    return "index.json";
}

//------------------------------------------------------------------------------

inline core::crypto::secure_string ISession::pickle(
    const core::crypto::secure_string& password,
    const core::crypto::secure_string& salt,
    const core::crypto::PasswordKeeper::EncryptionPolicy policy
)
{
    switch(policy)
    {
        case core::crypto::PasswordKeeper::EncryptionPolicy::SALTED:
            return password.empty() ? password : password + core::crypto::secure_string(salt);

        case core::crypto::PasswordKeeper::EncryptionPolicy::FORCED:
            return password + core::crypto::secure_string(salt);

        default:
            return password;
    }

    return password;
}

} // namespace detail

} // namespace sight::io::session
