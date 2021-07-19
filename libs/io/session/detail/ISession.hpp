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

    enum class EncryptionLevel
    {
        PASSWORD,
        SALTED,
        FORCED
    };

    constexpr static auto s_uuid {"uuid"};
    constexpr static auto s_children {"children"};
    constexpr static auto s_fields {"fields"};

    /// String serialization function
    inline std::string toString() const override;

    /// Set the encryption level
    /// @param level the encryption mode
    inline void setEncryptionLevel(const EncryptionLevel level);

    /// Salt the password, depending of the encryption level
    /// "PASSWORD" means encrypt if a password is provided, using the same key.
    /// "SALTED" means encrypt if a password is provided, but use a deterministically salted key, which will make hard
    ///   to open the archive session outside sight
    /// "FORCED" means always encrypt and use a salted key. The password will be deterministically guessed
    /// @param password original password
    /// @param salt salt used to change a bit the password
    /// @param level the way the password is changed
    static inline core::crypto::secure_string pickle(
        const core::crypto::secure_string& password,
        const core::crypto::secure_string& salt,
        const ISession::EncryptionLevel level = ISession::EncryptionLevel::SALTED
    );

protected:

    /// Constructor
    IO_SESSION_API ISession() = default;

    /// Destructor
    IO_SESSION_API virtual ~ISession() = default;

    /// Return the default index file path used to store the objects tree
    inline virtual std::filesystem::path getIndexFilePath() const;

    /// Return the default encryption level
    inline EncryptionLevel getEncryptionLevel() const;

private:

    EncryptionLevel m_encryptionLevel {EncryptionLevel::SALTED};
};

//------------------------------------------------------------------------------

inline std::string ISession::toString() const
{
    return core::location::ILocation::toString();
}

//------------------------------------------------------------------------------

inline std::filesystem::path ISession::getIndexFilePath() const
{
    return "/index.json";
}

//------------------------------------------------------------------------------

inline ISession::EncryptionLevel ISession::getEncryptionLevel() const
{
    return m_encryptionLevel;
}

//------------------------------------------------------------------------------

inline void ISession::setEncryptionLevel(const EncryptionLevel level)
{
    m_encryptionLevel = level;
}

//------------------------------------------------------------------------------

inline core::crypto::secure_string ISession::pickle(
    const core::crypto::secure_string& password,
    const core::crypto::secure_string& salt,
    const ISession::EncryptionLevel level
)
{
    switch(level)
    {
        case ISession::EncryptionLevel::PASSWORD:
            return password;

        case ISession::EncryptionLevel::SALTED:
            return password.empty() ? password : password + core::crypto::secure_string(salt);

        case ISession::EncryptionLevel::FORCED:
            return password + core::crypto::secure_string(salt);
    }
}

} // namespace detail

} // namespace sight::io::session
