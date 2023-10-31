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

#include "session_writer.hpp"

#include "io/session/detail/core/session_serializer.hpp"

#include <core/crypto/password_keeper.hpp>

#include <io/__/writer/registry/macros.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::session::session_writer);

namespace sight::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::archive;
class session_writer::session_writer_impl
{
public:

    /// Delete default constructors and assignment operators
    session_writer_impl()                                      = delete;
    session_writer_impl(const session_writer_impl&)            = delete;
    session_writer_impl(session_writer_impl&&)                 = delete;
    session_writer_impl& operator=(const session_writer_impl&) = delete;
    session_writer_impl& operator=(session_writer_impl&&)      = delete;

    /// Constructor
    inline explicit session_writer_impl(session_writer* const _session_writer) :
        m_session_writer(_session_writer),
        m_password(std::make_unique<password_keeper>()),
        m_encryption_policy(password_keeper::encryption_policy::password),
        m_archive_format(archive::archive_format::DEFAULT)
    {
    }

    /// Default destructor
    inline ~session_writer_impl() = default;

    /// Read the session from archive.
    inline void write()
    {
        // Retrieve the root object
        auto root_object = std::dynamic_pointer_cast<const data::object>(m_session_writer->get_object());
        SIGHT_FATAL_IF("Root object is null or not a data object.", !root_object);

        // Serialize the root object
        m_session_serializer.serialize(
            m_session_writer->get_file(),
            root_object,
            m_archive_format,
            m_password->get_password(),
            m_encryption_policy
        );
    }

    /// Session serializer which perform the serialization
    detail::session_serializer m_session_serializer;

    /// Pointer to the public interface
    session_writer* const m_session_writer;

    /// Keep the password in a vault
    const std::unique_ptr<password_keeper> m_password;

    /// The encryption policy
    password_keeper::encryption_policy m_encryption_policy;

    /// Archive format to use
    archive::archive_format m_archive_format;
};

session_writer::session_writer() :
    m_pimpl(std::make_unique<session_writer_impl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
session_writer::~session_writer() = default;

//------------------------------------------------------------------------------

void session_writer::write()
{
    m_pimpl->write();
}

//------------------------------------------------------------------------------

std::string session_writer::extension() const
{
    return ".zip";
}

//------------------------------------------------------------------------------

void session_writer::set_password(const secure_string& _password)
{
    m_pimpl->m_password->set_password(_password);
}

//------------------------------------------------------------------------------

void session_writer::set_encryption_policy(const password_keeper::encryption_policy _policy)
{
    m_pimpl->m_encryption_policy = _policy;
}

//------------------------------------------------------------------------------

void session_writer::set_archive_format(const archive::archive_format _archive_format)
{
    m_pimpl->m_archive_format = _archive_format;
}

//------------------------------------------------------------------------------

void session_writer::set_custom_serializer(const std::string& _class_name, serializer_t _serializer)
{
    m_pimpl->m_session_serializer.set_custom_serializer(_class_name, _serializer);
}

//------------------------------------------------------------------------------

void session_writer::set_serializer(const std::string& _class_name, serializer_t _serializer)
{
    detail::session_serializer::set_serializer(_class_name, _serializer);
}

//------------------------------------------------------------------------------

serializer_t session_writer::serializer(const std::string& _class_name)
{
    return detail::session_serializer::serializer(_class_name);
}

} //namespace sight::io::session
