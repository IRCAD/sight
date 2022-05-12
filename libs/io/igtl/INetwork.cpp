/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "INetwork.hpp"

#include "io/igtl/Exception.hpp"

#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/MessageFactory.hpp>

#if defined(_WIN32) && !defined(__CYGWIN__)
  #include <windows.h>
  #include <winsock2.h>
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <sys/time.h>
#endif

namespace sight::io::igtl
{

//------------------------------------------------------------------------------

INetwork::INetwork() :
    m_filteringByDeviceName(false),
    m_deviceNameOut("Sight")
{
}

//------------------------------------------------------------------------------

INetwork::~INetwork()
{
}

//------------------------------------------------------------------------------

bool INetwork::sendObject(const data::Object::csptr& obj)
{
    ::igtl::MessageBase::Pointer msg;

    detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
    msg = converter->fromFwObject(obj);
    msg->SetDeviceName(m_deviceNameOut.c_str());
    msg->Pack();
    return m_socket->Send(msg->GetPackPointer(), msg->GetPackSize()) == 1;
}

//------------------------------------------------------------------------------

bool INetwork::sendMsg(::igtl::MessageBase::Pointer msg)
{
    msg->SetDeviceName(m_deviceNameOut.c_str());
    msg->Pack();
    return m_socket->Send(msg->GetPackPointer(), msg->GetPackSize()) == 1;
}

//------------------------------------------------------------------------------

data::Object::sptr INetwork::receiveObject(std::string& deviceName)
{
    data::Object::sptr obj;
    ::igtl::MessageHeader::Pointer headerMsg = this->receiveHeader();
    if(headerMsg.IsNotNull())
    {
        ::igtl::MessageBase::Pointer msg = this->receiveBody(headerMsg);
        if(msg.IsNotNull())
        {
            detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
            obj        = converter->fromIgtlMessage(msg);
            deviceName = headerMsg->GetDeviceName();
        }
    }

    return obj;
}

//------------------------------------------------------------------------------

data::Object::sptr INetwork::receiveObject(std::string& deviceName, double& timestamp)
{
    data::Object::sptr obj;
    ::igtl::MessageHeader::Pointer headerMsg = this->receiveHeader();
    if(headerMsg.IsNotNull())
    {
        ::igtl::MessageBase::Pointer msg = this->receiveBody(headerMsg);
        if(msg.IsNotNull())
        {
            // get message timestamp
            unsigned int sec, frac;
            msg->GetTimeStamp(&sec, &frac);
            double secD, fracD;
            secD  = static_cast<double>(sec);
            fracD = static_cast<double>(frac);

            // convert into milliseconds
            fracD    /= 1000000.;
            secD     *= 1000.;
            timestamp = fracD + secD;

            detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
            obj        = converter->fromIgtlMessage(msg);
            deviceName = headerMsg->GetDeviceName();
        }
    }

    return obj;
}

//------------------------------------------------------------------------------

::igtl::MessageHeader::Pointer INetwork::receiveHeader()
{
    ::igtl::MessageHeader::Pointer headerMsg = ::igtl::MessageHeader::New();
    headerMsg->InitPack();
    const int sizeReceive = m_socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());

    if(sizeReceive == -1)
    {
        // Case 1: Timeout
        throw sight::io::igtl::Exception("Network timeout");
    }
    else if(sizeReceive == 0)
    {
        // Case 2: Error
        throw sight::io::igtl::Exception("Network Error");
    }
    else if(sizeReceive != headerMsg->GetPackSize())
    {
        // Case 3: mismatch of size
        throw sight::io::igtl::Exception("Received size error");
    }
    else
    {
        if(headerMsg->Unpack() == ::igtl::MessageBase::UNPACK_HEADER)
        {
            const std::string deviceName = headerMsg->GetDeviceName();

            if(m_filteringByDeviceName)
            {
                if(m_deviceNamesIn.find(deviceName) != m_deviceNamesIn.end())
                {
                    return headerMsg;
                }
                else
                {
                    return ::igtl::MessageHeader::Pointer();
                }
            }
            else
            {
                return headerMsg;
            }
        }
    }

    return ::igtl::MessageHeader::Pointer();
}

//------------------------------------------------------------------------------

::igtl::MessageBase::Pointer INetwork::receiveBody(::igtl::MessageHeader::Pointer const headerMsg)
{
    int unpackResult;
    int result;
    ::igtl::MessageBase::Pointer msg;

    if(!headerMsg)
    {
        throw sight::io::igtl::Exception("Invalid header message");
    }

    msg = io::igtl::detail::MessageFactory::create(headerMsg->GetDeviceType());
    msg->SetMessageHeader(headerMsg);
    msg->AllocatePack();
    result = m_socket->Receive(msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if(result == -1) // Timeout
    {
        throw sight::io::igtl::Exception("Network timeout");
    }
    else if(result == 0) // Error
    {
        throw sight::io::igtl::Exception("Network Error");
    }
    else
    {
        unpackResult = msg->Unpack();
        if(unpackResult == ::igtl::MessageHeader::UNPACK_UNDEF)
        {
            throw sight::io::igtl::Exception("Network Error");
        }

        if(unpackResult == ::igtl::MessageHeader::UNPACK_BODY)
        {
            return msg;
        }
    }

    throw Exception("Body pack is not valid");
}

//------------------------------------------------------------------------------

::igtl::Socket::Pointer INetwork::getSocket() const
{
    return m_socket;
}

//------------------------------------------------------------------------------

void INetwork::addAuthorizedDevice(const std::string& deviceName)
{
    std::set<std::string>::iterator it = m_deviceNamesIn.find(deviceName);

    if(it == m_deviceNamesIn.end())
    {
        m_deviceNamesIn.insert(deviceName);
    }
}

//------------------------------------------------------------------------------

bool INetwork::getFilteringByDeviceName() const
{
    return m_filteringByDeviceName;
}

//------------------------------------------------------------------------------

void INetwork::setFilteringByDeviceName(bool filtering)
{
    if(m_deviceNamesIn.empty())
    {
        m_filteringByDeviceName = false;
    }
    else
    {
        m_filteringByDeviceName = filtering;
    }
}

//------------------------------------------------------------------------------

void INetwork::setDeviceNameOut(const std::string& deviceName)
{
    m_deviceNameOut = deviceName;
}

//------------------------------------------------------------------------------

std::string INetwork::getDeviceNameOut() const
{
    return m_deviceNameOut;
}

//------------------------------------------------------------------------------

void INetwork::closeSocket(int socket_descriptor)
{
    // NOTE: Patched version of original CloseSocket() from igtlSocket.h, adding close() after shutdown() on Linux.
    // This can be removed on recent version of IGTL (Debian version is pretty old).

    if(socket_descriptor == -1)
    {
        return;
    }

    #if defined(_WIN32) && !defined(__CYGWIN__)
        #define WSA_VERSION MAKEWORD(1, 1)
    closesocket(socket_descriptor);
    #else
    shutdown(socket_descriptor, 2);
    close(socket_descriptor); // closing socket for latter reuse.
    #endif

    socket_descriptor = -1;
}

//------------------------------------------------------------------------------

int INetwork::createSocket()
{
    #if defined(_WIN32) && !defined(__CYGWIN__)
    // Declare variables
    WSADATA wsaData;
    //SOCKET ListenSocket;
    //sockaddr_in service;

    //---------------------------------------
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(iResult != NO_ERROR)
    {
        std::cerr << "Error at WSAStartup" << std::endl;
        return -1;
    }
#endif

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // Elimate windows 0.2 second delay sending (buffering) data.
    int on = 1;
    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*) &on, sizeof(on)))
    {
        return -1;
    }

    return sock;
}

//------------------------------------------------------------------------------

int INetwork::listenSocket(int socket_descriptor)
{
    if(socket_descriptor < 0)
    {
        return -1;
    }

    return listen(socket_descriptor, 1);
}

//------------------------------------------------------------------------------

int INetwork::bindSocket(int socket_descriptor, std::uint16_t port)
{
    struct sockaddr_in server;

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(port);
    // Allow the socket to be bound to an address that is already in use
#ifdef _WIN32
    int opt = 1;
    setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(int));
#else
    int opt = 1;
    setsockopt(socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (void*) &opt, sizeof(int));
#endif

    if(bind(socket_descriptor, reinterpret_cast<sockaddr*>(&server), sizeof(server)))
    {
        return -1;
    }

    return 0;
}

//------------------------------------------------------------------------------

} // namespace sight::io::igtl
