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

#include <core/crypto/password_keeper.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/location/single_file.hpp>

#include <data/object.hpp>

#include <io/__/writer/object_writer.hpp>
#include <io/zip/archive_writer.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

/// The serializer function signature. This is used to register an object serialization function.
using serializer_t = std::function<void (
                                       zip::archive_writer&,
                                       boost::property_tree::ptree&,
                                       data::object::csptr,
                                       std::map<std::string, data::object::csptr>&,
                                       const core::crypto::secure_string&)>;

/**
 * @brief Session writer.
 *
 * @details Class to write a session file recursively, including all fields from a data object.
 * The session file is indeed a standard "ZIP" archive, while the compression algorithm for files inside
 * the session archive is ZSTD. A standard archive reader could open a session file, if it is able to handle
 * ZIP archive with ZSTD compression.
 *
 * The archive can be password protected using AES256 algorithm and the compression level is set individually,
 * depending of the type of data to serialize.
 *
 */

class IO_SESSION_CLASS_API session_writer final : public io::writer::object_writer,
                                                  public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(session_writer, io::writer::object_writer, io::writer::factory::make<session_writer>);
    SIGHT_ALLOW_SHARED_FROM_THIS()
    /// Delete default constructors and assignment operators
    session_writer(const session_writer&)            = delete;
    session_writer(session_writer&&)                 = delete;
    session_writer& operator=(const session_writer&) = delete;
    session_writer& operator=(session_writer&&)      = delete;

    /// Use default destructor
    IO_SESSION_API ~session_writer() override;

    /// Constructor
    IO_SESSION_API session_writer();

    /// Write the file
    IO_SESSION_API void write() override;

    /// Defines extension supported by this writer ".zip"
    IO_SESSION_API std::string extension() const override;

    /// Sets the password
    /// @param password the new password
    IO_SESSION_API void set_password(const core::crypto::secure_string& _password);

    /// Sets the encryption policy
    /// @param policy the encryption policy: @see sight::io::session::password_keeper::encryption_policy
    IO_SESSION_API void set_encryption_policy(core::crypto::password_keeper::encryption_policy _policy);

    /// Set archive format
    /// @param archiveFormat how files are stored in the archive: @see sight::io::zip::archive::archiveFormat
    IO_SESSION_API void set_archive_format(zip::archive::archive_format _archive_format);

    /// Set a serialization function for an object
    /// @param className the name of the object to serialize
    /// @param serializer the function pointer to the serialization function
    IO_SESSION_API void set_custom_serializer(const std::string& _class_name, serializer_t _serializer = nullptr);

    /// Set a default serialization function for an object
    /// @param className the name of the object to serialize
    /// @param serializer the function pointer to the serialization function
    IO_SESSION_API static void set_serializer(const std::string& _class_name, serializer_t _serializer = nullptr);

    /// Return the registered serialization function for an object
    /// @param className the name of the object to serialize
    /// @return the function pointer to the serialization function
    IO_SESSION_API static serializer_t serializer(const std::string& _class_name);

private:

    /// PImpl
    class session_writerImpl;
    std::unique_ptr<session_writerImpl> m_pimpl;
};

} // namespace sight::io::session
