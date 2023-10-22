/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "Server.hpp"

#include "io/igtl/Exception.hpp"

#include <core/spy_log.hpp>

#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/MessageFactory.hpp>

#include <boost/lexical_cast.hpp>

namespace sight::io::igtl
{

//------------------------------------------------------------------------------

Server::Server() :
    m_serverSocket(::igtl::ServerSocket::New())
{
    m_socket = m_serverSocket;
}

//------------------------------------------------------------------------------

Server::~Server()
{
    if(m_isStarted)
    {
        this->stop();
    }
}

//------------------------------------------------------------------------------

bool Server::started() const
{
    core::mt::scoped_lock lock(m_mutex);

    return m_isStarted;
}

//------------------------------------------------------------------------------

std::uint16_t Server::getPort() const
{
    return m_port;
}

//------------------------------------------------------------------------------

void Server::runServer()
{
    Client::sptr new_client;

    while(this->started())
    {
        new_client = this->waitForConnection();
        if(new_client != nullptr)
        {
            core::mt::scoped_lock lock(m_mutex);
            if(m_receiveTimeout.has_value())
            {
                new_client->getSocket()->SetReceiveTimeout(static_cast<int>(m_receiveTimeout.value()));
            }

            m_clients.push_back(new_client);
        }
    }
}

//------------------------------------------------------------------------------

void Server::broadcast(const data::object::csptr& _obj)
{
    std::vector<Client::sptr>::iterator it;

    for(it = m_clients.begin() ; it != m_clients.end() ; )
    {
        if(!(*it)->sendObject(_obj))
        {
            core::mt::scoped_lock lock(m_mutex);
            (*it)->disconnect();
            it = m_clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//------------------------------------------------------------------------------

void Server::broadcast(::igtl::MessageBase::Pointer _msg)
{
    std::vector<Client::sptr>::iterator it;

    for(it = m_clients.begin() ; it != m_clients.end() ; )
    {
        if(!(*it)->sendMsg(_msg))
        {
            core::mt::scoped_lock lock(m_mutex);
            (*it)->disconnect();
            it = m_clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//------------------------------------------------------------------------------

void Server::start(std::uint16_t _port)
{
    core::mt::scoped_lock lock(m_mutex);

    if(m_isStarted)
    {
        throw Exception("Server already started");
    }

    //NOTE: Use the patched version of createServer
    const int result = this->createServer(_port);

    // Uncomment this when patch version isn't needed anymore.
    // m_serverSocket->CreateServer(port);

    // Ask m_serverSocket to give us the real port number (ex: if port is 0, it will use the first available port).
    m_port = std::uint16_t(m_serverSocket->GetServerPort());

    if(result != Server::s_SUCCESS)
    {
        throw Exception("Cannot create server on port : " + std::to_string(_port));
    }

    m_isStarted = true;
}

//------------------------------------------------------------------------------

Client::sptr Server::waitForConnection(int _msec)
{
    ::igtl::ClientSocket::Pointer client_socket;
    Client::sptr client;

    client_socket = m_serverSocket->WaitForConnection(static_cast<std::uint64_t>(_msec));
    if(client_socket.IsNotNull())
    {
        client = std::make_shared<Client>(client_socket);
    }

    return client;
}

//------------------------------------------------------------------------------

void Server::stop()
{
    core::mt::scoped_lock lock(m_mutex);
    if(!m_isStarted)
    {
        throw io::igtl::Exception("Server is already stopped");
    }

    m_isStarted = false;
    // Disconnect all clients

    for(auto& client : m_clients)
    {
        client->disconnect();
    }

    m_clients.clear();

    // HACK: patched version of closeSocket
    sight::io::igtl::network::closeSocket(m_serverSocket->m_SocketDescriptor);
    m_serverSocket->m_SocketDescriptor = -1;

    sight::io::igtl::network::closeSocket(m_socket->m_SocketDescriptor);
    m_socket->m_SocketDescriptor = -1;

    // Uncomment this when patch isn't needed anymore.
    //m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

std::size_t Server::numClients() const
{
    if(this->started())
    {
        core::mt::scoped_lock lock(m_mutex);

        return m_clients.size();
    }

    return 0;
}

//------------------------------------------------------------------------------

std::vector< ::igtl::MessageHeader::Pointer> Server::receiveHeaders()
{
    std::vector< ::igtl::MessageHeader::Pointer> header_msgs;

    core::mt::scoped_lock lock(m_mutex);
    for(size_t i = 0 ; i < m_clients.size() ; ++i)
    {
        const auto client = m_clients[i];
        //if client is disconnected
        if(client == nullptr)
        {
            m_clients.erase(m_clients.begin() + std::ptrdiff_t(i));
            continue;
        }

        ::igtl::MessageHeader::Pointer header_msg = ::igtl::MessageHeader::New();
        header_msg->InitPack();

        ::igtl::Socket::Pointer socket = client->getSocket();

        if(socket->GetConnected() != 0)
        {
            const int size_receive = socket->Receive(header_msg->GetPackPointer(), header_msg->GetPackSize());

            if(size_receive == -1)
            {
                throw sight::io::igtl::Exception("Network timeout");
            }

            if(size_receive == 0)
            {
                // Disconnect this client.
                sight::io::igtl::Server::removeClient(client);
                m_clients.erase(m_clients.begin() + std::ptrdiff_t(i));
                throw sight::io::igtl::Exception("Network error");
            }

            if(size_receive != header_msg->GetPackSize())
            {
                throw sight::io::igtl::Exception("Mismatch in received message size");
            }

            if(header_msg->Unpack() == ::igtl::MessageBase::UNPACK_HEADER)
            {
                const std::string device_name = header_msg->GetDeviceName();

                if(m_filteringByDeviceName)
                {
                    if(m_deviceNamesIn.find(device_name) != m_deviceNamesIn.end())
                    {
                        header_msgs.push_back(header_msg);
                    }
                    else
                    {
                        header_msgs.emplace_back();
                    }
                }
                else
                {
                    header_msgs.push_back(header_msg);
                }
            }
        }
    }

    return header_msgs;
}

//------------------------------------------------------------------------------

::igtl::MessageBase::Pointer Server::receiveBody(::igtl::MessageHeader::Pointer const _header_msg, unsigned int _client)
{
    ::igtl::MessageBase::Pointer msg;

    if(_header_msg == nullptr)
    {
        throw sight::io::igtl::Exception("Invalid header message");
    }

    msg = io::igtl::detail::MessageFactory::create(_header_msg->GetDeviceType());
    msg->SetMessageHeader(_header_msg);
    msg->AllocatePack();

    core::mt::scoped_lock lock(m_mutex);

    const int result = (m_clients[_client]->getSocket())->Receive(msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if(result == -1) // Timeout
    {
        throw sight::io::igtl::Exception("Network timeout");
    }

    if(result == 0) // Error
    {
        // Disconnect this client.
        sight::io::igtl::Server::removeClient(m_clients[_client]);
        m_clients.erase(m_clients.begin() + _client);
        throw sight::io::igtl::Exception("Network error");
    }

    const auto unpack_result = msg->Unpack();

    if(unpack_result == ::igtl::MessageHeader::UNPACK_UNDEF)
    {
        throw sight::io::igtl::Exception("Network error");
    }

    if(unpack_result == ::igtl::MessageHeader::UNPACK_BODY)
    {
        return msg;
    }

    throw Exception("Body pack is not valid");
}

//------------------------------------------------------------------------------

std::vector<data::object::sptr> Server::receiveObjects(std::vector<std::string>& _device_names)
{
    std::vector<data::object::sptr> obj_vect;
    std::vector< ::igtl::MessageHeader::Pointer> header_msg_vect = this->receiveHeaders();
    unsigned int client                                          = 0;
    for(const auto& header_msg : header_msg_vect)
    {
        if(header_msg.IsNotNull())
        {
            ::igtl::MessageBase::Pointer msg = this->receiveBody(header_msg, std::uint32_t(client));
            if(msg.IsNotNull())
            {
                detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
                obj_vect.push_back(converter->fromIgtlMessage(msg));
                _device_names.emplace_back(header_msg->GetDeviceName());
            }
        }

        ++client;
    }

    return obj_vect;
}

//------------------------------------------------------------------------------

void Server::setMessageDeviceName(const std::string& _device_name)
{
    for(const auto& client : m_clients)
    {
        if(client)
        {
            client->setDeviceNameOut(_device_name);
        }
    }
}

//------------------------------------------------------------------------------

int Server::createServer(std::uint16_t _port)
{
    if(m_serverSocket->m_SocketDescriptor != -1)
    {
        SIGHT_WARN("Server Socket already exists. Closing old socket.");
        sight::io::igtl::network::closeSocket(m_serverSocket->m_SocketDescriptor);
        m_serverSocket->m_SocketDescriptor = -1;
    }

    m_serverSocket->m_SocketDescriptor = sight::io::igtl::network::createSocket();

    if(m_serverSocket->m_SocketDescriptor < 0)
    {
        SIGHT_ERROR("Cannot create socket");
        return -1;
    }

    if(sight::io::igtl::network::bindSocket(m_serverSocket->m_SocketDescriptor, _port) != 0
       || sight::io::igtl::network::listenSocket(m_serverSocket->m_SocketDescriptor) != 0)
    {
        // failed to bind or listen.
        sight::io::igtl::network::closeSocket(m_serverSocket->m_SocketDescriptor);
        m_serverSocket->m_SocketDescriptor = -1;
        return -1;
    }

    // Success.
    return 0;
}

//------------------------------------------------------------------------------

void Server::removeClient(Client::sptr _client)
{
    if(_client)
    {
        _client->disconnect();
        _client.reset();
    }
}

} //namespace sight::io::igtl
