/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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
#include "io/session/PasswordKeeper.hpp"

#include <core/crypto/secure_string.hpp>
#include <core/location/SingleFile.hpp>

#include <io/base/writer/IObjectWriter.hpp>

namespace sight::io::session
{

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

class IO_SESSION_CLASS_API SessionWriter final :
    public base::writer::IObjectWriter,
    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(SessionWriter, base::writer::IObjectWriter, base::writer::factory::New<SessionWriter>);
    SIGHT_ALLOW_SHARED_FROM_THIS()
    /// Delete default constructors and assignment operators
    SessionWriter()                                = delete;
    SessionWriter(const SessionWriter&)            = delete;
    SessionWriter(SessionWriter&&)                 = delete;
    SessionWriter& operator=(const SessionWriter&) = delete;
    SessionWriter& operator=(SessionWriter&&)      = delete;

    /// Use default destructor
    IO_SESSION_API ~SessionWriter() override;

    /// Constructor
    IO_SESSION_API SessionWriter(base::writer::IObjectWriter::Key key);

    /// Write the file
    IO_SESSION_API void write() override;

    /// Defines extension supported by this writer ".zip"
    IO_SESSION_API std::string extension() override;

    /// Sets the password
    /// @param password the new password
    IO_SESSION_API void setPassword(const core::crypto::secure_string& password);

    /// Sets the encryption policy
    /// @param policy the encryption policy: @see sight::io::session::PasswordKeeper::EncryptionPolicy
    IO_SESSION_API void setEncryptionPolicy(const PasswordKeeper::EncryptionPolicy policy);

private:

    /// PImpl
    class SessionWriterImpl;
    std::unique_ptr<SessionWriterImpl> m_pimpl;
};

} // namespace sight::io::session
