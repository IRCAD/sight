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

#include <core/crypto/PasswordKeeper.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/location/SingleFile.hpp>

#include <data/Object.hpp>

#include <io/base/reader/IObjectReader.hpp>
#include <io/zip/ArchiveReader.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

/// The deserializer function signature. This is used to register an object deserialization function.
using deserializer_t = std::function<data::Object::sptr(
                                         zip::ArchiveReader&,
                                         const boost::property_tree::ptree&,
                                         const std::map<std::string, data::Object::sptr>&,
                                         data::Object::sptr,
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
class IO_SESSION_CLASS_API SessionReader final :
    public base::reader::IObjectReader,
    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(SessionReader, base::reader::IObjectReader, base::reader::factory::New<SessionReader>);
    SIGHT_ALLOW_SHARED_FROM_THIS()
    /// Delete default constructors and assignment operators
    SessionReader()                                = delete;
    SessionReader(const SessionReader&)            = delete;
    SessionReader(SessionReader&&)                 = delete;
    SessionReader& operator=(const SessionReader&) = delete;
    SessionReader& operator=(SessionReader&&)      = delete;

    /// Constructor
    IO_SESSION_API SessionReader(base::reader::IObjectReader::Key key);

    /// Default destructor
    IO_SESSION_API ~SessionReader() override;

    /// Read the session from archive.
    IO_SESSION_API void read() override;

    /// Defines extension supported by this reader ".zip"
    IO_SESSION_API std::string extension() const override;

    /// Sets the password
    /// @param password the new password
    IO_SESSION_API void set_password(const core::crypto::secure_string& password);

    /// Sets the encryption policy
    /// @param policy the encryption policy: @see sight::io::session::PasswordKeeper::EncryptionPolicy
    IO_SESSION_API void setEncryptionPolicy(core::crypto::PasswordKeeper::EncryptionPolicy policy);

    /// Set archive format
    /// @param archiveFormat how files are stored in the archive: @see sight::io::zip::Archive::ArchiveFormat
    IO_SESSION_API void setArchiveFormat(zip::Archive::ArchiveFormat archiveFormat);

    /// Set a deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    IO_SESSION_API void setDeserializer(const std::string& className, deserializer_t deserializer = nullptr);

    /// Set a default deserialization function for an object
    /// @param className the name of the object to serialize
    /// @param deserializer the function pointer to the deserialization function
    IO_SESSION_API static void setDefaultDeserializer(
        const std::string& className,
        deserializer_t deserializer = nullptr
    );

private:

    /// PImpl
    class SessionReaderImpl;
    std::unique_ptr<SessionReaderImpl> m_pimpl;
};

} // namespace sight::io::session
