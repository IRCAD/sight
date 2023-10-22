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

#include "SessionReader.hpp"

#include "io/session/detail/core/session_deserializer.hpp"

#include <core/crypto/password_keeper.hpp>

#include <io/__/reader/registry/macros.hpp>

SIGHT_REGISTER_IO_READER(sight::io::session::SessionReader);

namespace sight::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;

class SessionReader::SessionReaderImpl
{
public:

    /// Delete default constructors and assignment operators
    SessionReaderImpl()                                    = delete;
    SessionReaderImpl(const SessionReaderImpl&)            = delete;
    SessionReaderImpl(SessionReaderImpl&&)                 = delete;
    SessionReaderImpl& operator=(const SessionReaderImpl&) = delete;
    SessionReaderImpl& operator=(SessionReaderImpl&&)      = delete;

    /// Constructor
    inline explicit SessionReaderImpl(SessionReader* const _session_reader) :
        M_SESSION_READER(_session_reader),
        M_PASSWORD(std::make_unique<password_keeper>()),
        m_encryptionPolicy(password_keeper::encryption_policy::PASSWORD),
        m_archiveFormat(Archive::ArchiveFormat::DEFAULT)
    {
    }

    /// Default destructor
    inline ~SessionReaderImpl() = default;

    /// Read the session from archive.
    inline void read()
    {
        // Deserialize the root object
        m_object = m_sessionDeserializer.deserialize(
            M_SESSION_READER->get_file(),
            m_archiveFormat,
            M_PASSWORD->get_password(),
            m_encryptionPolicy
        );
    }

    /// Session deserializer which perform the deserialization
    detail::session_deserializer m_sessionDeserializer;

    /// Use a shared_ptr to keep the object alive as it is the read() return value
    core::tools::object::sptr m_object;

    /// Pointer to the public interface
    SessionReader* const M_SESSION_READER;

    /// Keep the password in a vault
    const std::unique_ptr<password_keeper> M_PASSWORD;

    /// The encryption policy
    password_keeper::encryption_policy m_encryptionPolicy;

    /// Archive format to use
    Archive::ArchiveFormat m_archiveFormat;
};

SessionReader::SessionReader() :
    m_pimpl(std::make_unique<SessionReaderImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
SessionReader::~SessionReader() = default;

//------------------------------------------------------------------------------

void SessionReader::read()
{
    m_pimpl->read();

    // Save the object so the caller can get it with getObject();
    set_object(m_pimpl->m_object);
}

//------------------------------------------------------------------------------

std::string SessionReader::extension() const
{
    return ".zip";
}

//------------------------------------------------------------------------------

void SessionReader::setPassword(const secure_string& _password)
{
    m_pimpl->M_PASSWORD->set_password(_password);
}

//------------------------------------------------------------------------------

void SessionReader::setEncryptionPolicy(const password_keeper::encryption_policy _policy)
{
    m_pimpl->m_encryptionPolicy = _policy;
}

//------------------------------------------------------------------------------

void SessionReader::setArchiveFormat(const Archive::ArchiveFormat _archive_format)
{
    m_pimpl->m_archiveFormat = _archive_format;
}

//------------------------------------------------------------------------------

void SessionReader::setCustomDeserializer(const std::string& _class_name, deserializer_t _deserializer)
{
    m_pimpl->m_sessionDeserializer.setCustomDeserializer(_class_name, _deserializer);
}

//------------------------------------------------------------------------------

void SessionReader::setDeserializer(const std::string& _class_name, deserializer_t _deserializer)
{
    detail::session_deserializer::setDeserializer(_class_name, _deserializer);
}

//------------------------------------------------------------------------------

deserializer_t SessionReader::deserializer(const std::string& _class_name)
{
    return detail::session_deserializer::deserializer(_class_name);
}

} // namespace sight::io::session
