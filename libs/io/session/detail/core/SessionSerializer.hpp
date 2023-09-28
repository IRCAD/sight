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
#include "io/session/detail/session.hpp"
#include "io/session/SessionWriter.hpp"

#include <core/crypto/password_keeper.hpp>

#include <io/zip/Archive.hpp>

#include <filesystem>

namespace sight::io::session::detail
{

/// Class to define a location that holds what is needed to deserialize a session.
class SessionSerializer final : public session
{
public:

    SIGHT_DECLARE_CLASS(SessionSerializer, session);

    /// Delete default constructors and assignment operators
    SessionSerializer(const SessionSerializer&)            = delete;
    SessionSerializer(SessionSerializer&&)                 = delete;
    SessionSerializer& operator=(const SessionSerializer&) = delete;
    SessionSerializer& operator=(SessionSerializer&&)      = delete;

    /// Default constructor
    SessionSerializer() noexcept = default;

    /// Default destructor
    ~SessionSerializer() noexcept override = default;

    /// Serializes a data::Object to archive
    /// @param archivePath archive file path
    /// @param object root object to serialize
    /// @param archiveFormat how files are stored in the archive @see sight::io::zip::Archive::ArchiveFormat
    /// @param password password to use for optional encryption. Empty password means no encryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    void serialize(
        const std::filesystem::path& archivePath,
        sight::data::Object::csptr object,
        io::zip::Archive::ArchiveFormat archiveFormat                     = io::zip::Archive::ArchiveFormat::DEFAULT,
        const core::crypto::secure_string& password                       = "",
        core::crypto::password_keeper::encryption_policy encryptionPolicy = core::crypto::password_keeper::encryption_policy::PASSWORD
    ) const;

    /// Set a serialization function for an object
    /// @param className the name of the object to serialize
    /// @param serializer the function pointer to the serialization function
    void setCustomSerializer(const std::string& className, serializer_t serializer = nullptr);

    /// Set a default serialization function for an object
    /// @param className the name of the object to serialize
    /// @param serializer the function pointer to the serialization function
    static void setSerializer(const std::string& className, serializer_t serializer = nullptr);

    /// Return the registered serialization function for an object
    /// @param className the name of the object to serialize
    /// @return the function pointer to the serialization function
    static serializer_t serializer(const std::string& className);

private:

    /// Custom serializers that override default one
    std::unordered_map<std::string, serializer_t> m_customSerializers;

    /// Return a serializer from a data object class name
    /// @param className the name of the object to find a serializer
    serializer_t findSerializer(const std::string& classname) const;

    /// Serializes recursively a data::Object to an opened archive using an initialized property tree
    /// @param cache ptree cache
    /// @param archive opened archive
    /// @param tree property tree used to store object index
    /// @param object root object to serialize
    /// @param password password to use for optional encryption. Empty password means no encryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    void deepSerialize(
        std::set<std::string>& cache,
        zip::ArchiveWriter& archive,
        boost::property_tree::ptree& tree,
        data::Object::csptr object,
        const core::crypto::secure_string& password,
        core::crypto::password_keeper::encryption_policy encryptionPolicy
    ) const;
};

} // namespace sight::io::session::detail
