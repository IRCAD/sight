/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#include "client.hpp"

#include "io/igtl/exception.hpp"

#include <io/igtl/detail/data_converter.hpp>
#include <io/igtl/detail/message_factory.hpp>

#include <boost/lexical_cast.hpp>

#include <mutex>

namespace sight::io::igtl
{

/// To guaranty that connexion is thread-safe.
static std::mutex s_connect_lock;

//------------------------------------------------------------------------------

client::client()
{
    m_socket = ::igtl::ClientSocket::New();
}

//------------------------------------------------------------------------------

client::client(::igtl::ClientSocket::Pointer _socket)
{
    m_socket = _socket;
    SIGHT_ASSERT("socket is null", _socket.IsNotNull());
    SIGHT_ASSERT("socket is not connected", _socket->GetConnected());
}

//------------------------------------------------------------------------------

client::~client()
{
    if(m_socket->GetConnected() != 0)
    {
        this->disconnect();
    }
}

//------------------------------------------------------------------------------

bool client::is_connected() const
{
    return m_socket->GetConnected() == 1;
}

//------------------------------------------------------------------------------

void client::throw_exception_if_failed(const std::string& _msg, bool _result)
{
    if(_result)
    {
        this->disconnect();
        throw exception(_msg);
    }
}

//------------------------------------------------------------------------------

void client::connect(const std::string& _addr, std::uint16_t _port)
{
    int result          = -1;
    const auto port_str = std::to_string(_port);

    {
        std::lock_guard lock(s_connect_lock);
        auto* client_socket = dynamic_cast< ::igtl::ClientSocket*>(m_socket.GetPointer());
        result = client_socket->ConnectToServer(_addr.c_str(), _port);
    }

    this->throw_exception_if_failed(
        std::string("Cannot connect to the server at ") + _addr + " : " + port_str,
        result == -1
    );
}

//------------------------------------------------------------------------------

void client::disconnect()
{
    std::lock_guard lock(s_connect_lock);
    // HACK: Use the patched version of closeSocket
    sight::io::igtl::network::close_socket(m_socket->m_SocketDescriptor);
    m_socket->m_SocketDescriptor = -1;
    // Uncomment this when patch isn't needed anymore.
    //m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

} // namespace sight::io::igtl
