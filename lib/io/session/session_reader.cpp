/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include "session_reader.hpp"

#include "io/session/detail/core/session_deserializer.hpp"

#include <core/crypto/password_keeper.hpp>

namespace sight::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::archive;

class session_reader::session_reader_impl
{
public:

    /// Delete default constructors and assignment operators
    session_reader_impl()                                      = delete;
    session_reader_impl(const session_reader_impl&)            = delete;
    session_reader_impl(session_reader_impl&&)                 = delete;
    session_reader_impl& operator=(const session_reader_impl&) = delete;
    session_reader_impl& operator=(session_reader_impl&&)      = delete;

    /// Constructor
    inline explicit session_reader_impl(session_reader* const _session_reader) :
        m_session_reader(_session_reader),
        m_password(std::make_unique<password_keeper>()),
        m_encryption_policy(password_keeper::encryption_policy::password),
        m_archive_format(archive::archive_format::DEFAULT)
    {
    }

    /// Default destructor
    inline ~session_reader_impl() = default;

    /// Read the session from archive.
    inline void read()
    {
        // Deserialize the root object
        m_object = m_session_deserializer.deserialize(
            m_session_reader->get_file(),
            m_archive_format,
            m_password->get_password(),
            m_encryption_policy
        );
    }

    /// Session deserializer which perform the deserialization
    detail::session_deserializer m_session_deserializer;

    /// Use a shared_ptr to keep the object alive as it is the read() return value
    core::object::sptr m_object;

    /// Pointer to the public interface
    session_reader* const m_session_reader;

    /// Keep the password in a vault
    const std::unique_ptr<password_keeper> m_password;

    /// The encryption policy
    password_keeper::encryption_policy m_encryption_policy;

    /// Archive format to use
    archive::archive_format m_archive_format;
};

session_reader::session_reader() :
    m_pimpl(std::make_unique<session_reader_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
session_reader::~session_reader() = default;

//------------------------------------------------------------------------------

void session_reader::read()
{
    m_pimpl->read();

    // Save the object so the caller can get it with get_object();
    set_object(m_pimpl->m_object);
}

//------------------------------------------------------------------------------

std::string session_reader::extension() const
{
    return ".zip";
}

//------------------------------------------------------------------------------

void session_reader::set_password(const secure_string& _password)
{
    m_pimpl->m_password->set_password(_password);
}

//------------------------------------------------------------------------------

void session_reader::set_encryption_policy(const password_keeper::encryption_policy _policy)
{
    m_pimpl->m_encryption_policy = _policy;
}

//------------------------------------------------------------------------------

void session_reader::set_archive_format(const archive::archive_format _archive_format)
{
    m_pimpl->m_archive_format = _archive_format;
}

//------------------------------------------------------------------------------

void session_reader::set_custom_deserializer(const std::string& _class_name, deserializer_t _deserializer)
{
    m_pimpl->m_session_deserializer.set_custom_deserializer(_class_name, _deserializer);
}

//------------------------------------------------------------------------------

void session_reader::set_deserializer(const std::string& _class_name, deserializer_t _deserializer)
{
    detail::session_deserializer::set_deserializer(_class_name, _deserializer);
}

//------------------------------------------------------------------------------

deserializer_t session_reader::deserializer(const std::string& _class_name)
{
    return detail::session_deserializer::deserializer(_class_name);
}

} // namespace sight::io::session
