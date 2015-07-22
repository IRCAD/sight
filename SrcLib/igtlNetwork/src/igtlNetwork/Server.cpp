/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlNetwork/Server.hpp"
#include "igtlNetwork/Exception.hpp"

#include <fwCore/spyLog.hpp>
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
        }
    }
}

//------------------------------------------------------------------------------

void Server::broadcast (::fwData::Object::sptr obj)
{
    std::list<Client::sptr>::iterator it;

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

}
