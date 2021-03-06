/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/crypto/secure_string.hpp>
#include <core/location/SingleFile.hpp>

#include <io/base/writer/IObjectWriter.hpp>

namespace sight::io::session
{

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

    /// Defines extension supported by this writer ".sight"
    IO_SESSION_API std::string extension() override;

    /// Sets the password
    /// @param password the new password
    IO_SESSION_API void setPassword(const core::crypto::secure_string& password);

private:

    /// PImpl
    class SessionWriterImpl;
    std::unique_ptr<SessionWriterImpl> m_pimpl;
};

} // namespace sight::io::session
