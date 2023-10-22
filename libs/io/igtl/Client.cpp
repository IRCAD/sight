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

#include "Client.hpp"

#include "io/igtl/Exception.hpp"

#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/MessageFactory.hpp>

#include <boost/lexical_cast.hpp>

#include <mutex>

namespace sight::io::igtl
{

/// To guaranty that connexion is thread-safe.
static std::mutex s_connect_lock;

//------------------------------------------------------------------------------

Client::Client()
{
    m_socket = ::igtl::ClientSocket::New();
}

//------------------------------------------------------------------------------

Client::Client(::igtl::ClientSocket::Pointer _socket)
{
    m_socket = _socket;
    SIGHT_ASSERT("socket is null", _socket.IsNotNull());
    SIGHT_ASSERT("socket is not connected", _socket->GetConnected());
}

//------------------------------------------------------------------------------

Client::~Client()
{
    if(m_socket->GetConnected() != 0)
    {
        this->disconnect();
    }
}

//------------------------------------------------------------------------------

bool Client::isConnected() const
{
    return m_socket->GetConnected() == 1;
}

//------------------------------------------------------------------------------

void Client::throwExceptionIfFailed(const std::string& _msg, bool _result)
{
    if(_result)
    {
        this->disconnect();
        throw Exception(_msg);
    }
}

//------------------------------------------------------------------------------

void Client::connect(const std::string& _addr, std::uint16_t _port)
{
    int result          = -1;
    const auto port_str = std::to_string(_port);

    {
        std::lock_guard lock(s_connect_lock);
        auto* client_socket = dynamic_cast< ::igtl::ClientSocket*>(m_socket.GetPointer());
        result = client_socket->ConnectToServer(_addr.c_str(), _port);
    }

    this->throwExceptionIfFailed(
        std::string("Cannot connect to the server at ") + _addr + " : " + port_str,
        result == -1
    );
}

//------------------------------------------------------------------------------

void Client::disconnect()
{
    std::lock_guard lock(s_connect_lock);
    // HACK: Use the patched version of closeSocket
    sight::io::igtl::network::closeSocket(m_socket->m_SocketDescriptor);
    m_socket->m_SocketDescriptor = -1;
    // Uncomment this when patch isn't needed anymore.
    //m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

} // namespace sight::io::igtl
