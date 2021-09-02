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

#include "SessionWriter.hpp"

#include "detail/SessionSerializer.hpp"

#include "PasswordKeeper.hpp"

#include <io/base/writer/registry/macros.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::session::SessionWriter);

namespace sight::io::session
{

class SessionWriter::SessionWriterImpl
{
public:

    /// Delete default constructors and assignment operators
    SessionWriterImpl()                                    = delete;
    SessionWriterImpl(const SessionWriterImpl&)            = delete;
    SessionWriterImpl(SessionWriterImpl&&)                 = delete;
    SessionWriterImpl& operator=(const SessionWriterImpl&) = delete;
    SessionWriterImpl& operator=(SessionWriterImpl&&)      = delete;

    /// Constructor
    inline SessionWriterImpl(SessionWriter* const sessionWriter) :
        m_SessionWriter(sessionWriter),
        m_password(std::make_unique<PasswordKeeper>())
    {
    }

    /// Default destructor
    inline ~SessionWriterImpl() = default;

    /// Read the session from archive.
    inline void write()
    {
        // Retrieve the root object
        auto root_object = std::dynamic_pointer_cast<const data::Object>(m_SessionWriter->getObject());
        SIGHT_FATAL_IF("Root object is null or not a data object.", !root_object);

        // Create the session and serialize the root object
        detail::SessionSerializer session;
        session.serialize(m_SessionWriter->getFile(), root_object, m_password->getPassword());
    }

    /// Sets the password
    /// @param password the new password
    inline void setPassword(const core::crypto::secure_string& password)
    {
        m_password->setPassword(password);
    }

private:

    /// Pointer to the public interface
    SessionWriter* const m_SessionWriter;

    /// Keep the password in a vault
    const std::unique_ptr<PasswordKeeper> m_password;
};

SessionWriter::SessionWriter(base::writer::IObjectWriter::Key) :
    m_pimpl(std::make_unique<SessionWriterImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
SessionWriter::~SessionWriter() = default;

//------------------------------------------------------------------------------

void SessionWriter::write()
{
    m_pimpl->write();
}

//------------------------------------------------------------------------------

std::string SessionWriter::extension()
{
    return ".zip";
}

//------------------------------------------------------------------------------

void SessionWriter::setPassword(const core::crypto::secure_string& password)
{
    m_pimpl->setPassword(password);
}

} //namespace sight::io::session
