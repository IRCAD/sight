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
#include "io/session/session_writer.hpp"

#include <core/crypto/password_keeper.hpp>

#include <io/zip/archive.hpp>

#include <filesystem>

namespace sight::io::session::detail
{

/// Class to define a location that holds what is needed to deserialize a session.
class session_serializer final : public session
{
public:

    SIGHT_DECLARE_CLASS(session_serializer, session);

    /// Delete default constructors and assignment operators
    session_serializer(const session_serializer&)            = delete;
    session_serializer(session_serializer&&)                 = delete;
    session_serializer& operator=(const session_serializer&) = delete;
    session_serializer& operator=(session_serializer&&)      = delete;

    /// Default constructor
    session_serializer() noexcept = default;

    /// Default destructor
    ~session_serializer() noexcept override = default;

    /// Serializes a data::object to archive
    /// @param archivePath archive file path
    /// @param object root object to serialize
    /// @param archiveFormat how files are stored in the archive @see sight::io::zip::archive::archiveFormat
    /// @param password password to use for optional encryption. Empty password means no encryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    void serialize(
        const std::filesystem::path& _archive_path,
        sight::data::object::csptr _object,
        io::zip::archive::archive_format _archive_format                    = io::zip::archive::archive_format::DEFAULT,
        const core::crypto::secure_string& _password                        = "",
        core::crypto::password_keeper::encryption_policy _encryption_policy = core::crypto::password_keeper::encryption_policy::password
    ) const;

    /// Set a serialization function for an object
    /// @param className the name of the object to serialize
    /// @param serializer the function pointer to the serialization function
    void set_custom_serializer(const std::string& _class_name, serializer_t _serializer = nullptr);

    /// Set a default serialization function for an object
    /// @param className the name of the object to serialize
    /// @param serializer the function pointer to the serialization function
    static void set_serializer(const std::string& _class_name, serializer_t _serializer = nullptr);

    /// Return the registered serialization function for an object
    /// @param className the name of the object to serialize
    /// @return the function pointer to the serialization function
    static serializer_t serializer(const std::string& _class_name);

private:

    /// Custom serializers that override default one
    std::unordered_map<std::string, serializer_t> m_custom_serializers;

    /// Return a serializer from a data object class name
    /// @param className the name of the object to find a serializer
    serializer_t find_serializer(const std::string& _classname) const;

    /// Serializes recursively a data::object to an opened archive using an initialized property tree
    /// @param cache ptree cache
    /// @param archive opened archive
    /// @param tree property tree used to store object index
    /// @param object root object to serialize
    /// @param password password to use for optional encryption. Empty password means no encryption
    /// @param encryptionPolicy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    void deep_serialize(
        std::set<std::string>& _cache,
        zip::archive_writer& _archive,
        boost::property_tree::ptree& _tree,
        data::object::csptr _object,
        const core::crypto::secure_string& _password,
        core::crypto::password_keeper::encryption_policy _encryption_policy
    ) const;
};

} // namespace sight::io::session::detail
