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

#pragma once

#include "io/session/config.hpp"

#include <core/crypto/obfuscated_string.hpp>
#include <core/crypto/password_keeper.hpp>
#include <core/crypto/sha256.hpp>
#include <core/exceptionmacros.hpp>
#include <core/location/base.hpp>

#include <filesystem>

namespace sight::io::session::detail
{

/// Interface to define a location that holds what is needed to de/serialize a session.
class IO_SESSION_CLASS_API session : public core::location::base
{
public:

    SIGHT_DECLARE_CLASS(session, core::location::base);

    constexpr static auto UUID {"uuid"};
    constexpr static auto CHILDREN {"children"};
    constexpr static auto FIELDS {"fields"};
    constexpr static auto DESCRIPTION {"description"};

    /// String serialization function
    [[nodiscard]] inline std::string to_string() const override;

    /// Return the default index file path inside the session archive
    static inline std::filesystem::path get_index_file_path();

    /// Salt the password, depending of the encryption level
    /// "PASSWORD" means encrypt if a password is provided, using the same key.
    /// "SALTED" means encrypt if a password is provided, but use a deterministically salted key, which will make hard
    ///   to open the archive session outside sight
    /// "FORCED" means always encrypt and use a salted key. The password will be deterministically generated if none is
    ///   provided
    /// @param password original password
    /// @param salt salt used to change a bit the password
    /// @param level the way the password is changed
    /// @param policy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    static inline core::crypto::secure_string pickle(
        const core::crypto::secure_string& _password,
        const core::crypto::secure_string& _salt,
        core::crypto::password_keeper::encryption_policy _policy = core::crypto::password_keeper::encryption_policy::password
    );

protected:

    /// Constructor
    IO_SESSION_API session() = default;

    /// Destructor
    IO_SESSION_API ~session() override = default;
};

//------------------------------------------------------------------------------

inline std::string session::to_string() const
{
    return core::location::base::to_string();
}

//------------------------------------------------------------------------------

inline std::filesystem::path session::get_index_file_path()
{
    return "index.json";
}

//------------------------------------------------------------------------------

inline core::crypto::secure_string session::pickle(
    const core::crypto::secure_string& _password,
    const core::crypto::secure_string& _salt,
    const core::crypto::password_keeper::encryption_policy _policy
)
{
    if(_password.empty() && _policy == core::crypto::password_keeper::encryption_policy::forced)
    {
        if constexpr(core::crypto::password_keeper::has_default_password())
        {
            return core::crypto::password_keeper::get_default_password();
        }
        else if(!_salt.empty())
        {
            return core::crypto::hash(_salt);
        }
        else
        {
            SIGHT_THROW("No password provided and no default password available");
        }
    }
    else if(_policy == core::crypto::password_keeper::encryption_policy::salted)
    {
        return core::crypto::hash(_password + _salt);
    }
    else
    {
        return _password;
    }
}

} // namespace sight::io::session::detail
