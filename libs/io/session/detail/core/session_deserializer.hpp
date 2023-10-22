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
#include "io/session/SessionReader.hpp"

#include <core/crypto/password_keeper.hpp>

#include <data/object.hpp>

#include <io/zip/Archive.hpp>

#include <filesystem>

namespace sight::io::session::detail
{

/// Class to define a location that holds what is needed to deserialize a session.
class session_deserializer final : public session
{
public:

    SIGHT_DECLARE_CLASS(session_deserializer, session);

    /// Delete default constructors and assignment operators
    session_deserializer(const session_deserializer&)            = delete;
    session_deserializer(session_deserializer&&)                 = delete;
    session_deserializer& operator=(const session_deserializer&) = delete;
    session_deserializer& operator=(session_deserializer&&)      = delete;

    /// Default constructor
    session_deserializer() noexcept = default;

    /// Default destructor
    ~session_deserializer() noexcept override = default;

    /// Deserialize a data::object from archive
    /// @param archivePath archive file path
    /// @param archiveFormat how files are stored in the archive. @see sight::io::zip::Archive::ArchiveFormat
    /// @param password password to use for optional decryption. Empty password means no decryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    sight::data::object::sptr deserialize(
        const std::filesystem::path& _archive_path,
        io::zip::Archive::ArchiveFormat _archive_format                     = io::zip::Archive::ArchiveFormat::DEFAULT,
        const core::crypto::secure_string& _password                        = "",
        core::crypto::password_keeper::encryption_policy _encryption_policy = core::crypto::password_keeper::encryption_policy::PASSWORD
    ) const;

    /// Set a deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    void setCustomDeserializer(const std::string& _class_name, deserializer_t _deserializer = nullptr);

    /// Set a default deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    static void setDeserializer(const std::string& _class_name, deserializer_t _deserializer = nullptr);

    /// Return the registered deserialization function for an object
    /// @param className the name of the object to deserialize
    /// @return the function pointer to the deserialization function
    static deserializer_t deserializer(const std::string& _class_name);

private:

    /// Custom serializers that override default one
    std::unordered_map<std::string, deserializer_t> m_customDeserializers;

    /// Return a deserializer from a data object class name
    /// @param className the name of the object to find a deserializer
    deserializer_t findDeserializer(const std::string& _classname) const;

    /// Deserializes recursively an initialized archive to a data::object using an opened property tree
    /// @param cache object cache
    /// @param archive initialized archive
    /// @param tree property tree used to retrieve object index
    /// @param password password to use for optional encryption. Empty password means no encryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    data::object::sptr deepDeserialize(
        std::map<std::string, data::object::sptr>& _cache,
        zip::ArchiveReader& _archive,
        const boost::property_tree::ptree& _tree,
        const core::crypto::secure_string& _password,
        core::crypto::password_keeper::encryption_policy _encryption_policy
    ) const;
};

} // namespace sight::io::session::detail
