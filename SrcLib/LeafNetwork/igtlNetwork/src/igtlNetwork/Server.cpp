/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlNetwork/Server.hpp"
#include "igtlNetwork/Exception.hpp"

#include <fwCore/spyLog.hpp>
#include <igtlProtocol/MessageFactory.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>

namespace igtlNetwork
{

//------------------------------------------------------------------------------

Server::Server() :
    m_serverSocket (::igtl::ServerSocket::New()),
    m_isStarted (false),
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

::boost::uint16_t Server::getPort() const
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
        if (newClient != NULL)
        {
            m_clients.push_back(newClient);
            OSLM_TRACE("New client on server "<<m_port);
        }
    }
}

//------------------------------------------------------------------------------

void Server::broadcast(::fwData::Object::sptr obj)
{
    std::vector<Client::sptr>::iterator it;

    for (it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (!(*it)->sendObject(obj))
        {
            (*it)->disconnect();
            it = m_clients.erase(it);
            if (it ==  m_clients.end())
            {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void Server::broadcast(::igtl::MessageBase::Pointer msg)
{
    std::vector<Client::sptr>::iterator it;

    for (it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (!(*it)->sendMsg(msg))
        {
            (*it)->disconnect();
            it = m_clients.erase(it);
            if (it ==  m_clients.end())
            {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void Server::start (::boost::uint16_t port) throw (::fwCore::Exception)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    int result;

    if (m_isStarted)
    {
        throw Exception("Server already started");
    }

    result = this->m_serverSocket->CreateServer(port);
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

    clientSocket = this->m_serverSocket->WaitForConnection (msec);
    if (clientSocket.IsNotNull())
    {
        client = Client::sptr (new Client (clientSocket));
    }
    return client;
}

//------------------------------------------------------------------------------

void Server::stop() throw (::fwCore::Exception)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    if (!m_isStarted)
    {
        throw ::igtlNetwork::Exception("Server is already stopped");
    }

    m_isStarted = false;
    m_clients.clear();
    this->m_serverSocket->CloseSocket();

}

//------------------------------------------------------------------------------

unsigned int Server::getNumberOfClients()
{
    if(this->isStarted())
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);

        return m_clients.size();
    }

    return 0;

}

//------------------------------------------------------------------------------

std::vector< ::igtl::MessageHeader::Pointer > Server::receiveHeader()
{
    std::vector< ::igtl::MessageHeader::Pointer > headerMsgs;

    std::vector< Client::sptr >::iterator it;

    ::fwCore::mt::ScopedLock lock(m_mutex);

    for (it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        int sizeReceive;

        ::igtl::MessageHeader::Pointer headerMsg = ::igtl::MessageHeader::New();
        headerMsg->InitPack();

        //if client is disconnected
        if((*it) == NULL)
        {
            continue;
        }

        ::igtl::Socket::Pointer socket = (*it)->getSocket();

        if(socket->GetConnected())
        {
            sizeReceive = socket->Receive (headerMsg->GetPackPointer(), headerMsg->GetPackSize());

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
                    std::string deviceName = headerMsg->GetDeviceName();

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

::igtl::MessageBase::Pointer Server::receiveBody (::igtl::MessageHeader::Pointer const headerMsg, unsigned int client)
throw (::fwCore::Exception)
{
    int unpackResult;
    int result;
    ::igtl::MessageBase::Pointer msg;

    msg = ::igtlProtocol::MessageFactory::create(headerMsg->GetDeviceType());
    msg->SetMessageHeader (headerMsg);
    msg->AllocatePack();

    ::fwCore::mt::ScopedLock lock(m_mutex);

    result = (m_clients[client]->getSocket())->Receive (msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if (result == -1)
    {
        return ::igtl::MessageBase::Pointer();
    }

    unpackResult = msg->Unpack (1);
    if (unpackResult & igtl::MessageHeader::UNPACK_BODY)
    {
        return msg;
    }
    throw Exception("Body pack is not valid");
}

//------------------------------------------------------------------------------

void Server::setMessageDeviceName(std::string deviceName)
{
    std::vector<Client::sptr>::iterator it;

    for (it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        //if client is disconnected
        if((*it) == NULL)
        {
            continue;
        }

        (*it)->setDeviceNameOut(deviceName);
    }
}

//------------------------------------------------------------------------------
}//namespace igtlNetwork
