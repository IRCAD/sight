/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlNetwork/Server.hpp"

#include "igtlNetwork/Exception.hpp"

#include <fwCore/spyLog.hpp>

#include <igtlProtocol/MessageFactory.hpp>

#include <boost/lexical_cast.hpp>

namespace igtlNetwork
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
    if (m_isStarted)
    {
        this->stop();
    }
}

//------------------------------------------------------------------------------

bool Server::isStarted() const
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

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

    while (this->isStarted())
    {
        newClient = this->waitForConnection();
        if (newClient != nullptr)
        {
            ::fwCore::mt::ScopedLock lock(m_mutex);
            m_clients.push_back(newClient);
            OSLM_TRACE("New client on server port: "<<m_port);
        }
    }
}

//------------------------------------------------------------------------------

void Server::broadcast(const ::fwData::Object::csptr& obj)
{
    std::vector<Client::sptr>::iterator it;

    for (it = m_clients.begin(); it != m_clients.end(); )
    {
        if (!(*it)->sendObject(obj))
        {
            ::fwCore::mt::ScopedLock lock(m_mutex);
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

    for (it = m_clients.begin(); it != m_clients.end(); )
    {
        if (!(*it)->sendMsg(msg))
        {
            ::fwCore::mt::ScopedLock lock(m_mutex);
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

void Server::start (std::uint16_t port)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    if (m_isStarted)
    {
        throw Exception("Server already started");
    }

    const int result = m_serverSocket->CreateServer(port);
    m_port = port;

    if (result != Server::s_SUCCESS)
    {
        throw Exception("Cannot create server on port : " + ::boost::lexical_cast< std::string >(port));
    }
    m_isStarted = true;
}

//------------------------------------------------------------------------------

Client::sptr Server::waitForConnection (int msec)
{
    ::igtl::ClientSocket::Pointer clientSocket;
    Client::sptr client;

    clientSocket = m_serverSocket->WaitForConnection(static_cast<unsigned long>(msec));
    if (clientSocket.IsNotNull())
    {
        client = std::make_shared< Client >(clientSocket);
    }
    return client;
}

//------------------------------------------------------------------------------

void Server::stop()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    if (!m_isStarted)
    {
        throw ::igtlNetwork::Exception("Server is already stopped");
    }

    m_isStarted = false;
    m_clients.clear();
    m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

size_t Server::getNumberOfClients() const
{
    if(this->isStarted())
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);

        return m_clients.size();
    }
    return 0;
}

//------------------------------------------------------------------------------

std::vector< ::igtl::MessageHeader::Pointer > Server::receiveHeaders()
{
    std::vector< ::igtl::MessageHeader::Pointer > headerMsgs;

    ::fwCore::mt::ScopedLock lock(m_mutex);

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

            if (sizeReceive == -1 || sizeReceive == 0)
            {
                headerMsgs.push_back( ::igtl::MessageHeader::Pointer());
            }
            else
            {
                if (sizeReceive != 0 && sizeReceive != headerMsg->GetPackSize())
                {
                    headerMsgs.push_back( ::igtl::MessageHeader::Pointer());
                }
                else if (headerMsg->Unpack() & ::igtl::MessageBase::UNPACK_HEADER)
                {
                    const std::string deviceName = headerMsg->GetDeviceName();

                    if(m_filteringByDeviceName)
                    {
                        if(m_deviceNamesIn.find(deviceName) != m_deviceNamesIn.end())
                        {
                            headerMsgs.push_back( headerMsg );
                        }
                        else
                        {
                            headerMsgs.push_back( ::igtl::MessageHeader::Pointer());
                        }
                    }
                    else
                    {
                        headerMsgs.push_back( headerMsg );
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

    msg = ::igtlProtocol::MessageFactory::create(headerMsg->GetDeviceType());
    msg->SetMessageHeader(headerMsg);
    msg->AllocatePack();

    ::fwCore::mt::ScopedLock lock(m_mutex);

    const int result = (m_clients[client]->getSocket())->Receive(msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if (result == -1)
    {
        return ::igtl::MessageBase::Pointer();
    }

    const int unpackResult = msg->Unpack(1);
    if (unpackResult & igtl::MessageHeader::UNPACK_BODY)
    {
        return msg;
    }
    throw Exception("Body pack is not valid");
}

//------------------------------------------------------------------------------

std::vector< ::fwData::Object::sptr > Server::receiveObjects(std::vector<std::string>& deviceNames)
{
    std::vector< ::fwData::Object::sptr > objVect;
    std::vector< ::igtl::MessageHeader::Pointer > headerMsgVect = this->receiveHeaders();
    size_t client                                               = 0;
    for(const auto& headerMsg : headerMsgVect)
    {
        if (headerMsg.IsNotNull())
        {
            ::igtl::MessageBase::Pointer msg = this->receiveBody(headerMsg, client);
            if (msg.IsNotNull())
            {
                objVect.push_back(m_dataConverter->fromIgtlMessage(msg));
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
}//namespace igtlNetwork
