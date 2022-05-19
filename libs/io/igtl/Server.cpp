/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include <core/spyLog.hpp>

#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/MessageFactory.hpp>

#include <boost/lexical_cast.hpp>

namespace sight::io::igtl
{

//------------------------------------------------------------------------------

Server::Server() :
    m_serverSocket(::igtl::ServerSocket::New()),
    m_isStarted(false),
    m_port(0)
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

bool Server::isStarted() const
{
    core::mt::ScopedLock lock(m_mutex);

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
    Client::sptr newClient;

    while(this->isStarted())
    {
        newClient = this->waitForConnection();
        if(newClient != nullptr)
        {
            core::mt::ScopedLock lock(m_mutex);
            m_clients.push_back(newClient);
        }
    }
}

//------------------------------------------------------------------------------

void Server::broadcast(const data::Object::csptr& obj)
{
    std::vector<Client::sptr>::iterator it;

    for(it = m_clients.begin() ; it != m_clients.end() ; )
    {
        if(!(*it)->sendObject(obj))
        {
            core::mt::ScopedLock lock(m_mutex);
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

void Server::broadcast(::igtl::MessageBase::Pointer msg)
{
    std::vector<Client::sptr>::iterator it;

    for(it = m_clients.begin() ; it != m_clients.end() ; )
    {
        if(!(*it)->sendMsg(msg))
        {
            core::mt::ScopedLock lock(m_mutex);
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

void Server::start(std::uint16_t port)
{
    core::mt::ScopedLock lock(m_mutex);

    if(m_isStarted)
    {
        throw Exception("Server already started");
    }

    const int result = m_serverSocket->CreateServer(port);
    // Ask m_serverSocket to give us the real port number (ex: if port is 0, it will use the first available port).
    m_port = std::uint16_t(m_serverSocket->GetServerPort());

    if(result != Server::s_SUCCESS)
    {
        throw Exception("Cannot create server on port : " + boost::lexical_cast<std::string>(port));
    }

    m_isStarted = true;
}

//------------------------------------------------------------------------------

Client::sptr Server::waitForConnection(int msec)
{
    ::igtl::ClientSocket::Pointer clientSocket;
    Client::sptr client;

    clientSocket = m_serverSocket->WaitForConnection(static_cast<unsigned long>(msec));
    if(clientSocket.IsNotNull())
    {
        client = std::make_shared<Client>(clientSocket);
    }

    return client;
}

//------------------------------------------------------------------------------

void Server::stop()
{
    core::mt::ScopedLock lock(m_mutex);
    if(!m_isStarted)
    {
        throw io::igtl::Exception("Server is already stopped");
    }

    m_isStarted = false;
    m_clients.clear();
    m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

std::size_t Server::numClients() const
{
    if(this->isStarted())
    {
        core::mt::ScopedLock lock(m_mutex);

        return m_clients.size();
    }

    return 0;
}

//------------------------------------------------------------------------------

std::vector< ::igtl::MessageHeader::Pointer> Server::receiveHeaders()
{
    std::vector< ::igtl::MessageHeader::Pointer> headerMsgs;

    core::mt::ScopedLock lock(m_mutex);

    for(const auto& client : m_clients)
    {
        //if client is disconnected
        if(client == nullptr)
        {
            continue;
        }

        ::igtl::MessageHeader::Pointer headerMsg = ::igtl::MessageHeader::New();
        headerMsg->InitPack();

        ::igtl::Socket::Pointer socket = client->getSocket();

        if(socket->GetConnected())
        {
            const int sizeReceive = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());

            if(sizeReceive == -1 || sizeReceive == 0)
            {
                headerMsgs.push_back(::igtl::MessageHeader::Pointer());
            }
            else
            {
                if(sizeReceive != 0 && sizeReceive != headerMsg->GetPackSize())
                {
                    headerMsgs.push_back(::igtl::MessageHeader::Pointer());
                }
                else if(headerMsg->Unpack() & ::igtl::MessageBase::UNPACK_HEADER)
                {
                    const std::string deviceName = headerMsg->GetDeviceName();

                    if(m_filteringByDeviceName)
                    {
                        if(m_deviceNamesIn.find(deviceName) != m_deviceNamesIn.end())
                        {
                            headerMsgs.push_back(headerMsg);
                        }
                        else
                        {
                            headerMsgs.push_back(::igtl::MessageHeader::Pointer());
                        }
                    }
                    else
                    {
                        headerMsgs.push_back(headerMsg);
                    }
                }
            }
        }
    }

    return headerMsgs;
}

//------------------------------------------------------------------------------

::igtl::MessageBase::Pointer Server::receiveBody(::igtl::MessageHeader::Pointer const headerMsg, unsigned int client)
{
    ::igtl::MessageBase::Pointer msg;

    msg = io::igtl::detail::MessageFactory::create(headerMsg->GetDeviceType());
    msg->SetMessageHeader(headerMsg);
    msg->AllocatePack();

    core::mt::ScopedLock lock(m_mutex);

    const int result = (m_clients[client]->getSocket())->Receive(msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if(result == -1)
    {
        return ::igtl::MessageBase::Pointer();
    }

    const int unpackResult = msg->Unpack(1);
    if(unpackResult & ::igtl::MessageHeader::UNPACK_BODY)
    {
        return msg;
    }

    throw Exception("Body pack is not valid");
}

//------------------------------------------------------------------------------

std::vector<data::Object::sptr> Server::receiveObjects(std::vector<std::string>& deviceNames)
{
    std::vector<data::Object::sptr> objVect;
    std::vector< ::igtl::MessageHeader::Pointer> headerMsgVect = this->receiveHeaders();
    std::size_t client                                         = 0;
    for(const auto& headerMsg : headerMsgVect)
    {
        if(headerMsg.IsNotNull())
        {
            ::igtl::MessageBase::Pointer msg = this->receiveBody(headerMsg, std::uint32_t(client));
            if(msg.IsNotNull())
            {
                detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
                objVect.push_back(converter->fromIgtlMessage(msg));
                deviceNames.push_back(headerMsg->GetDeviceName());
            }
        }

        ++client;
    }

    return objVect;
}

//------------------------------------------------------------------------------

void Server::setMessageDeviceName(const std::string& deviceName)
{
    for(const auto& client : m_clients)
    {
        if(client)
        {
            client->setDeviceNameOut(deviceName);
        }
    }
}

//------------------------------------------------------------------------------

} //namespace sight::io::igtl
