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

#include "SessionReader.hpp"

#include "detail/SessionDeserializer.hpp"

#include "PasswordKeeper.hpp"

#include <io/base/reader/registry/macros.hpp>

SIGHT_REGISTER_IO_READER(sight::io::session::SessionReader);

namespace sight::io::session
{

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
    inline SessionReaderImpl(SessionReader* const sessionReader) :
        m_sessionReader(sessionReader),
        m_password(std::make_unique<PasswordKeeper>()),
        m_encryptionPolicy(PasswordKeeper::EncryptionPolicy::DEFAULT)
    {
    }

    /// Default destructor
    inline ~SessionReaderImpl() = default;

    /// Read the session from archive.
    inline void read()
    {
        // Deserialize the root object
        m_object = m_sessionDeserializer.deserialize(
            m_sessionReader->getFile(),
            m_password->getPassword(),
            m_encryptionPolicy
        );
    }

    /// Session deserializer which perform the deserialization
    detail::SessionDeserializer m_sessionDeserializer;

    /// Use a shared_ptr to keep the object alive as it is the read() return value
    core::tools::Object::sptr m_object;

    /// Pointer to the public interface
    SessionReader* const m_sessionReader;

    /// Keep the password in a vault
    const std::unique_ptr<PasswordKeeper> m_password;

    /// The encryption policy
    PasswordKeeper::EncryptionPolicy m_encryptionPolicy;
};

SessionReader::SessionReader(base::reader::IObjectReader::Key) :
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
    setObject(m_pimpl->m_object);
}

//------------------------------------------------------------------------------

std::string SessionReader::extension() const
{
    return ".zip";
}

//------------------------------------------------------------------------------

void SessionReader::setPassword(const core::crypto::secure_string& password)
{
    m_pimpl->m_password->setPassword(password);
}

//------------------------------------------------------------------------------

void SessionReader::setEncryptionPolicy(const PasswordKeeper::EncryptionPolicy policy)
{
    m_pimpl->m_encryptionPolicy = policy;
}

//------------------------------------------------------------------------------

void SessionReader::setDeserializer(const std::string& className, deserializer_t deserializer)
{
    m_pimpl->m_sessionDeserializer.setDeserializer(className, deserializer);
}

//------------------------------------------------------------------------------

void SessionReader::setDefaultDeserializer(const std::string& className, deserializer_t deserializer)
{
    detail::SessionDeserializer::setDefaultDeserializer(className, deserializer);
}

} // namespace sight::io::session
