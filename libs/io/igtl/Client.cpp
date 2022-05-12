/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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
static std::mutex s_connectLock;

//------------------------------------------------------------------------------

Client::Client()
{
    m_socket = ::igtl::ClientSocket::New();
}

//------------------------------------------------------------------------------

Client::Client(::igtl::ClientSocket::Pointer socket)
{
    m_socket = socket;
    SIGHT_ASSERT("socket is null", socket.IsNotNull());
    SIGHT_ASSERT("socket is not connected", socket->GetConnected());
}

//------------------------------------------------------------------------------

Client::~Client()
{
    if(m_socket->GetConnected())
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

void Client::throwExceptionIfFailed(const std::string& msg, bool result)
{
    if(result)
    {
        this->disconnect();
        throw Exception(msg);
    }
}

//------------------------------------------------------------------------------

void Client::connect(const std::string& addr, std::uint16_t port)
{
    int result                = -1;
    const std::string portStr = boost::lexical_cast<std::string>(port);

    {
        std::lock_guard lock(s_connectLock);
        ::igtl::ClientSocket* clientSocket = dynamic_cast< ::igtl::ClientSocket*>(m_socket.GetPointer());
        result = clientSocket->ConnectToServer(addr.c_str(), port);
    }

    this->throwExceptionIfFailed("Cannot connect to the server at " + addr + " : " + portStr, result == -1);
}

//------------------------------------------------------------------------------

void Client::disconnect()
{
    std::lock_guard lock(s_connectLock);
    // HACK: Use the patched version of closeSocket
    sight::io::igtl::INetwork::closeSocket(m_socket->m_SocketDescriptor);
    m_socket->m_SocketDescriptor = -1;
    // Uncomment this when patch isn't needed anymore.
    //m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

} // namespace sight::io::igtl
