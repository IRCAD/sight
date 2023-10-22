/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "network.hpp"

#include "io/igtl/Exception.hpp"

#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/MessageFactory.hpp>

#include <cmath>

#if defined(_WIN32)
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

network::network() :

    m_deviceNameOut("Sight")
{
}

//------------------------------------------------------------------------------

network::~network()
= default;

//------------------------------------------------------------------------------

bool network::sendObject(const data::object::csptr& _obj)
{
    ::igtl::MessageBase::Pointer msg;

    detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
    msg = converter->fromFwObject(_obj);
    msg->SetDeviceName(m_deviceNameOut.c_str());
    msg->Pack();
    return m_socket->Send(msg->GetPackPointer(), msg->GetPackSize()) == 1;
}

//------------------------------------------------------------------------------

bool network::sendMsg(::igtl::MessageBase::Pointer _msg)
{
    _msg->SetDeviceName(m_deviceNameOut.c_str());
    _msg->Pack();
    return m_socket->Send(_msg->GetPackPointer(), _msg->GetPackSize()) == 1;
}

//------------------------------------------------------------------------------

data::object::sptr network::receiveObject(std::string& _device_name)
{
    data::object::sptr obj;
    ::igtl::MessageHeader::Pointer header_msg = this->receiveHeader();
    if(header_msg.IsNotNull())
    {
        ::igtl::MessageBase::Pointer msg = this->receiveBody(header_msg);
        if(msg.IsNotNull())
        {
            detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
            obj          = converter->fromIgtlMessage(msg);
            _device_name = header_msg->GetDeviceName();
        }
    }

    return obj;
}

//------------------------------------------------------------------------------

data::object::sptr network::receiveObject(std::string& _device_name, double& _timestamp)
{
    data::object::sptr obj;
    ::igtl::MessageHeader::Pointer header_msg = this->receiveHeader();
    if(header_msg.IsNotNull())
    {
        ::igtl::MessageBase::Pointer msg = this->receiveBody(header_msg);
        if(msg.IsNotNull())
        {
            // get message timestamp
            unsigned int sec  = 0;
            unsigned int frac = 0;
            msg->GetTimeStamp(&sec, &frac);
            double sec_d  = NAN;
            double frac_d = NAN;
            sec_d  = static_cast<double>(sec);
            frac_d = static_cast<double>(frac);

            // convert into milliseconds
            frac_d    /= 1000000.;
            sec_d     *= 1000.;
            _timestamp = frac_d + sec_d;

            detail::DataConverter::sptr converter = detail::DataConverter::getInstance();
            obj          = converter->fromIgtlMessage(msg);
            _device_name = header_msg->GetDeviceName();
        }
    }

    return obj;
}

//------------------------------------------------------------------------------

::igtl::MessageHeader::Pointer network::receiveHeader()
{
    ::igtl::MessageHeader::Pointer header_msg = ::igtl::MessageHeader::New();
    header_msg->InitPack();
    const int size_receive = m_socket->Receive(header_msg->GetPackPointer(), header_msg->GetPackSize());

    if(size_receive == -1)
    {
        // Case 1: Timeout
        throw sight::io::igtl::Exception("Network timeout");
    }

    if(size_receive == 0)
    {
        // Case 2: Error
        throw sight::io::igtl::Exception("Network Error");
    }

    if(size_receive != header_msg->GetPackSize())
    {
        // Case 3: mismatch of size
        throw sight::io::igtl::Exception("Received size error");
    }

    if(header_msg->Unpack() == ::igtl::MessageBase::UNPACK_HEADER)
    {
        const std::string device_name = header_msg->GetDeviceName();

        if(m_filteringByDeviceName)
        {
            if(m_deviceNamesIn.find(device_name) != m_deviceNamesIn.end())
            {
                return header_msg;
            }

            return {};
        }

        return header_msg;
    }

    return {};
}

//------------------------------------------------------------------------------

::igtl::MessageBase::Pointer network::receiveBody(::igtl::MessageHeader::Pointer const _header_msg)
{
    int unpack_result = 0;
    int result        = 0;
    ::igtl::MessageBase::Pointer msg;

    if(_header_msg == nullptr)
    {
        throw sight::io::igtl::Exception("Invalid header message");
    }

    msg = io::igtl::detail::MessageFactory::create(_header_msg->GetDeviceType());
    msg->SetMessageHeader(_header_msg);
    msg->AllocatePack();
    result = m_socket->Receive(msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if(result == -1) // Timeout
    {
        throw sight::io::igtl::Exception("Network timeout");
    }

    if(result == 0) // Error
    {
        throw sight::io::igtl::Exception("Network Error");
    }

    unpack_result = msg->Unpack();
    if(unpack_result == ::igtl::MessageHeader::UNPACK_UNDEF)
    {
        throw sight::io::igtl::Exception("Network Error");
    }

    if(unpack_result == ::igtl::MessageHeader::UNPACK_BODY)
    {
        return msg;
    }

    throw Exception("Body pack is not valid");
}

//------------------------------------------------------------------------------

::igtl::Socket::Pointer network::getSocket() const
{
    return m_socket;
}

//------------------------------------------------------------------------------

void network::addAuthorizedDevice(const std::string& _device_name)
{
    auto it = m_deviceNamesIn.find(_device_name);

    if(it == m_deviceNamesIn.end())
    {
        m_deviceNamesIn.insert(_device_name);
    }
}

//------------------------------------------------------------------------------

bool network::getFilteringByDeviceName() const
{
    return m_filteringByDeviceName;
}

//------------------------------------------------------------------------------

void network::setFilteringByDeviceName(bool _filtering)
{
    if(m_deviceNamesIn.empty())
    {
        m_filteringByDeviceName = false;
    }
    else
    {
        m_filteringByDeviceName = _filtering;
    }
}

//------------------------------------------------------------------------------

void network::setDeviceNameOut(const std::string& _device_name)
{
    m_deviceNameOut = _device_name;
}

//------------------------------------------------------------------------------

std::string network::getDeviceNameOut() const
{
    return m_deviceNameOut;
}

//------------------------------------------------------------------------------

void network::closeSocket(int _socket_descriptor)
{
    // NOTE: Patched version of original CloseSocket() from igtlSocket.h, adding close() after shutdown() on Linux.
    // This can be removed on recent version of IGTL (Debian version is pretty old).

    if(_socket_descriptor == -1)
    {
        return;
    }

#if defined(_WIN32)
#define WSA_VERSION MAKEWORD(1, 1)
    closesocket(_socket_descriptor);
#else
    shutdown(_socket_descriptor, 2);
    close(_socket_descriptor); // closing socket for latter reuse.
#endif
}

//------------------------------------------------------------------------------

int network::createSocket()
{
#if defined(_WIN32)
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
    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&on), sizeof(on)) != 0)
    {
#if defined(_WIN32)
        closesocket(sock);
#else
        close(sock);
#endif
        return -1;
    }

    return sock;
}

//------------------------------------------------------------------------------

int network::listenSocket(int _socket_descriptor)
{
    if(_socket_descriptor < 0)
    {
        return -1;
    }

    return listen(_socket_descriptor, 1);
}

//------------------------------------------------------------------------------

int network::bindSocket(int _socket_descriptor, std::uint16_t _port)
{
    struct sockaddr_in server {};

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(_port);
    // Allow the socket to be bound to an address that is already in use
#ifdef _WIN32
    int opt = 1;
    setsockopt(_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(int));
#else
    int opt = 1;
    setsockopt(_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (void*) &opt, sizeof(int));
#endif

    if(bind(_socket_descriptor, reinterpret_cast<sockaddr*>(&server), sizeof(server)) != 0)
    {
        return -1;
    }

    return 0;
}

//------------------------------------------------------------------------------

} // namespace sight::io::igtl
