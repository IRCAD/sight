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

#include <io/__/reader/object_reader.hpp>
#include <io/zip/archive_reader.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

/// The deserializer function signature. This is used to register an object deserialization function.
using deserializer_t = std::function<data::object::sptr(
                                         zip::archive_reader&,
                                         const boost::property_tree::ptree&,
                                         const std::map<std::string, data::object::sptr>&,
                                         data::object::sptr,
                                         const core::crypto::secure_string&
                                     )>;

/**
 * @brief Session reader.
 *
 * @details Class to read a session file, and restore recursively a data object, including all fields.
 * The session file is indeed a standard "ZIP" archive, while the compression algorithm for files inside
 * the session archive is ZSTD. A standard archive reader could open a session file, if it is able to handle
 * ZIP archive with ZSTD compression.
 *
 * The archive can be password protected using AES256 algorithm and the compression level is set individually,
 * depending of the type of data to serialize.
 *
 */
class IO_SESSION_CLASS_API session_reader final :
    public io::reader::object_reader,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(session_reader, io::reader::object_reader, io::reader::factory::make<session_reader>);
    SIGHT_ALLOW_SHARED_FROM_THIS()
    /// Delete default constructors and assignment operators
    session_reader(const session_reader&)            = delete;
    session_reader(session_reader&&)                 = delete;
    session_reader& operator=(const session_reader&) = delete;
    session_reader& operator=(session_reader&&)      = delete;

    /// Constructor
    IO_SESSION_API session_reader();

    /// Default destructor
    IO_SESSION_API ~session_reader() override;

    /// Read the session from archive.
    IO_SESSION_API void read() override;

    /// Defines extension supported by this reader ".zip"
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

    /// Set a deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    IO_SESSION_API void set_custom_deserializer(const std::string& _class_name, deserializer_t _deserializer = nullptr);

    /// Set a default deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    IO_SESSION_API static void set_deserializer(
        const std::string& _class_name,
        deserializer_t _deserializer = nullptr
    );

    /// Return the registered deserialization function for an object
    /// @param className the name of the object to deserialize
    /// @return the function pointer to the deserialization function
    IO_SESSION_API static deserializer_t deserializer(const std::string& _class_name);

private:

    /// PImpl
    class session_reader_impl;
    std::unique_ptr<session_reader_impl> m_pimpl;
};

} // namespace sight::io::session
