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

#include "io/session/config.hpp"
#include "io/session/detail/ISession.hpp"
#include "io/session/SessionReader.hpp"

#include <core/crypto/PasswordKeeper.hpp>

#include <data/Object.hpp>

#include <io/zip/Archive.hpp>

#include <filesystem>

namespace sight::io::session::detail
{

/// Class to define a location that holds what is needed to deserialize a session.
class SessionDeserializer final : public ISession
{
public:

    SIGHT_DECLARE_CLASS(SessionDeserializer, ISession);

    /// Delete default constructors and assignment operators
    SessionDeserializer(const SessionDeserializer&)            = delete;
    SessionDeserializer(SessionDeserializer&&)                 = delete;
    SessionDeserializer& operator=(const SessionDeserializer&) = delete;
    SessionDeserializer& operator=(SessionDeserializer&&)      = delete;

    /// Default constructor
    SessionDeserializer() noexcept = default;

    /// Default destructor
    ~SessionDeserializer() noexcept override = default;

    /// Deserialize a data::Object from archive
    /// @param archivePath archive file path
    /// @param archiveFormat how files are stored in the archive. @see sight::io::zip::Archive::ArchiveFormat
    /// @param password password to use for optional decryption. Empty password means no decryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::PasswordKeeper::EncryptionPolicy
    sight::data::Object::sptr deserialize(
        const std::filesystem::path& archive_path,
        io::zip::Archive::ArchiveFormat archiveFormat                   = io::zip::Archive::ArchiveFormat::DEFAULT,
        const core::crypto::secure_string& password                     = "",
        core::crypto::PasswordKeeper::EncryptionPolicy encryptionPolicy = core::crypto::PasswordKeeper::EncryptionPolicy::DEFAULT
    ) const;

    /// Set a deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    void setCustomDeserializer(const std::string& className, deserializer_t deserializer = nullptr);

    /// Set a default deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    static void setDeserializer(const std::string& className, deserializer_t deserializer = nullptr);

private:

    /// Custom serializers that override default one
    std::unordered_map<std::string, deserializer_t> m_customDeserializers;

    /// Return a deserializer from a data object class name
    /// @param className the name of the object to find a deserializer
    deserializer_t findDeserializer(const std::string& classname) const;

    /// Deserializes recursively an initialized archive to a data::Object using an opened property tree
    /// @param cache object cache
    /// @param archive initialized archive
    /// @param tree property tree used to retrieve object index
    /// @param password password to use for optional encryption. Empty password means no encryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::PasswordKeeper::EncryptionPolicy
    data::Object::sptr deepDeserialize(
        std::map<std::string, data::Object::sptr>& cache,
        zip::ArchiveReader& archive,
        const boost::property_tree::ptree& tree,
        const core::crypto::secure_string& password,
        core::crypto::PasswordKeeper::EncryptionPolicy encryptionPolicy
    ) const;
};

} // namespace sight::io::session::detail
