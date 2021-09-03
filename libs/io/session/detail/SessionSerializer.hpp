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
#include "io/session/detail/ISession.hpp"
#include "io/session/PasswordKeeper.hpp"

#include <core/crypto/secure_string.hpp>

#include <data/Object.hpp>

#include <filesystem>

namespace sight::io::session
{

namespace detail
{

/// Class to define a location that holds what is needed to deserialize a session.
class SessionSerializer final : public ISession
{
public:

    SIGHT_DECLARE_CLASS(SessionSerializer, ISession);

    /// Delete default constructors and assignment operators
    SessionSerializer(const SessionSerializer&)            = delete;
    SessionSerializer(SessionSerializer&&)                 = delete;
    SessionSerializer& operator=(const SessionSerializer&) = delete;
    SessionSerializer& operator=(SessionSerializer&&)      = delete;

    /// Default constructor
    SessionSerializer() noexcept = default;

    /// Default destructor
    ~SessionSerializer() noexcept = default;

    /// Serializes a data::Object to archive
    /// @param archivePath archive file path
    /// @param object root object to serialize
    /// @param password password to use for optional encryption. Empty password means no encryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::PasswordKeeper::EncryptionPolicy
    void serialize(
        const std::filesystem::path& archivePath,
        sight::data::Object::csptr object,
        const core::crypto::secure_string& password             = "",
        const PasswordKeeper::EncryptionPolicy encryptionPolicy = PasswordKeeper::EncryptionPolicy::DEFAULT
    ) const;
};

} // namespace detail

} // namespace sight::io::session
