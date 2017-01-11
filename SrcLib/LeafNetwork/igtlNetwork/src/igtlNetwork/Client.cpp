/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlNetwork/Client.hpp"
#include "igtlNetwork/Exception.hpp"

#include <fwTools/Stringizer.hpp>

#include <igtlProtocol/MessageFactory.hpp>

namespace igtlNetwork
{

//------------------------------------------------------------------------------

Client::Client()
{
    m_socket = ::igtl::ClientSocket::New();
}

//------------------------------------------------------------------------------

Client::Client (::igtl::ClientSocket::Pointer socket)
{
    m_socket = socket;
    SLM_ASSERT ("socket is null", socket.IsNotNull());
    SLM_ASSERT ("socket is not connected", socket->GetConnected());
}

//------------------------------------------------------------------------------

Client::~Client()
{
    if (m_socket->GetConnected())
    {
        this->disconnect();
    }
}

//------------------------------------------------------------------------------

bool Client::isConnected() const
{
    return (m_socket->GetConnected() == 1);
}

//------------------------------------------------------------------------------

void Client::throwExceptionIfFailed (const std::string& msg, bool result)
{
    if (result)
    {
        this->disconnect();
        throw Exception(msg);
    }
}

//------------------------------------------------------------------------------

void Client::connect (const std::string& addr, ::boost::uint16_t port) throw (::fwCore::Exception)
{
    int result;
    std::string portStr;

    result  = dynamic_cast< ::igtl::ClientSocket* >( m_socket.GetPointer() )->ConnectToServer (addr.c_str(), port);
    portStr = boost::lexical_cast<std::string> (port);
    this->throwExceptionIfFailed ("Cannot connect to the server at " + addr + " : " + portStr, result == -1);
}

//------------------------------------------------------------------------------


void Client::disconnect() throw (::fwCore::Exception)
{
    m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

} // namespace igtlNetwork

