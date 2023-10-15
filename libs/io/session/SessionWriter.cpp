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

#include "SessionWriter.hpp"

#include "io/session/detail/core/session_serializer.hpp"

#include <core/crypto/password_keeper.hpp>

#include <io/__/writer/registry/macros.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::session::SessionWriter);

namespace sight::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;
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
    inline explicit SessionWriterImpl(SessionWriter* const sessionWriter) :
        m_sessionWriter(sessionWriter),
        m_password(std::make_unique<password_keeper>()),
        m_encryptionPolicy(password_keeper::encryption_policy::PASSWORD),
        m_archiveFormat(Archive::ArchiveFormat::DEFAULT)
    {
    }

    /// Default destructor
    inline ~SessionWriterImpl() = default;

    /// Read the session from archive.
    inline void write()
    {
        // Retrieve the root object
        auto root_object = std::dynamic_pointer_cast<const data::object>(m_sessionWriter->getObject());
        SIGHT_FATAL_IF("Root object is null or not a data object.", !root_object);

        // Serialize the root object
        m_sessionSerializer.serialize(
            m_sessionWriter->get_file(),
            root_object,
            m_archiveFormat,
            m_password->get_password(),
            m_encryptionPolicy
        );
    }

    /// Session serializer which perform the serialization
    detail::session_serializer m_sessionSerializer;

    /// Pointer to the public interface
    SessionWriter* const m_sessionWriter;

    /// Keep the password in a vault
    const std::unique_ptr<password_keeper> m_password;

    /// The encryption policy
    password_keeper::encryption_policy m_encryptionPolicy;

    /// Archive format to use
    Archive::ArchiveFormat m_archiveFormat;
};

SessionWriter::SessionWriter() :
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

std::string SessionWriter::extension() const
{
    return ".zip";
}

//------------------------------------------------------------------------------

void SessionWriter::setPassword(const secure_string& password)
{
    m_pimpl->m_password->set_password(password);
}

//------------------------------------------------------------------------------

void SessionWriter::setEncryptionPolicy(const password_keeper::encryption_policy policy)
{
    m_pimpl->m_encryptionPolicy = policy;
}

//------------------------------------------------------------------------------

void SessionWriter::setArchiveFormat(const Archive::ArchiveFormat archiveFormat)
{
    m_pimpl->m_archiveFormat = archiveFormat;
}

//------------------------------------------------------------------------------

void SessionWriter::setCustomSerializer(const std::string& className, serializer_t serializer)
{
    m_pimpl->m_sessionSerializer.setCustomSerializer(className, serializer);
}

//------------------------------------------------------------------------------

void SessionWriter::setSerializer(const std::string& className, serializer_t serializer)
{
    detail::session_serializer::setSerializer(className, serializer);
}

//------------------------------------------------------------------------------

serializer_t SessionWriter::serializer(const std::string& className)
{
    return detail::session_serializer::serializer(className);
}

} //namespace sight::io::session
