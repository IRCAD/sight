/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
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

#include "zmqNetwork/Socket.hpp"

#include <igtlProtocol/MessageFactory.hpp>

#include <igtl/igtlMessageBase.h>

#include <cstring>

namespace zmqNetwork
{

//------------------------------------------------------------------------------

Socket::Socket(SocketMode socketMode, PatternMode patternMode) :
    m_isStarted(false),
    m_sockMode(socketMode),
    m_patternMode(patternMode),
    m_timeout(1000),
    m_deviceNameOut("Sight")
{
    m_dataConverter = ::igtlProtocol::DataConverter::getInstance();
}

//------------------------------------------------------------------------------

Socket::~Socket()
{

}

//------------------------------------------------------------------------------

void Socket::deleteMessage(void* data, void* hint)
{
    ::igtl::MessageBase::Pointer ptr = reinterpret_cast< ::igtl::MessageBase* >(hint);

    if(ptr)
    {
        ptr->UnRegister();
    }
}

//------------------------------------------------------------------------------

void Socket::sendObject(const ::fwData::Object::csptr& data)
{
    ::igtl::MessageBase::Pointer igtlMsg = m_dataConverter->fromFwObject(data);
    igtlMsg->SetDeviceName(m_deviceNameOut.c_str());
    igtlMsg->Pack();
    igtlMsg->Register();

    ::zmq::message_t zmqMsg(igtlMsg->GetPackPointer(), igtlMsg->GetPackSize(),
                            Socket::deleteMessage, igtlMsg.GetPointer());

    m_socket->send(zmqMsg);
}

//------------------------------------------------------------------------------

::fwData::Object::sptr Socket::receiveObject(std::string& deviceName)
{
    ::fwData::Object::sptr obj;
    ::igtl::MessageBase::Pointer msg;
    ::zmq::message_t response;

    ::igtl::MessageHeader::Pointer headerMsg = ::igtl::MessageHeader::New();
    headerMsg->InitPack();
    m_socket->recv(&response);
    if (response.size() > Socket::s_HEADER_SIZE)
    {
        std::memcpy(headerMsg->GetPackPointer(), response.data(), headerMsg->GetPackSize());
        if (headerMsg->Unpack() & ::igtl::MessageHeader::UNPACK_HEADER)
        {
            msg = ::igtlProtocol::MessageFactory::create(headerMsg->GetDeviceType());
            msg->SetMessageHeader(headerMsg);
            msg->AllocatePack();
            std::memcpy(msg->GetPackBodyPointer(),
                        (reinterpret_cast<char*>(response.data())) + Socket::s_HEADER_SIZE, msg->GetPackBodySize());
            if (!(msg->Unpack(1) & ::igtl::MessageHeader::UNPACK_BODY))
            {
                throw Exception("Cannot unpack body");
            }
            deviceName = msg->GetDeviceName();
            obj        = m_dataConverter->fromIgtlMessage(msg);
        }
        else
        {
            throw Exception("Cannot unpack header");
        }
    }
    return obj;
}

//------------------------------------------------------------------------------

bool Socket::isStarted() const
{
    return m_isStarted;
}

//------------------------------------------------------------------------------

Socket::SocketMode Socket::getSocketMode() const
{
    return m_sockMode;
}

//------------------------------------------------------------------------------

Socket::PatternMode Socket::getPatternMode() const
{
    return m_patternMode;
}

//------------------------------------------------------------------------------

void Socket::initialize()
{
    m_context = std::make_shared< ::zmq::context_t >(1);

    m_socket = std::make_shared< ::zmq::socket_t >(*m_context, static_cast<int>(m_patternMode));
    if (m_patternMode == Subscribe)
    {
        m_socket->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);
    }
}

//------------------------------------------------------------------------------

void Socket::start(const std::string& str)
{
    if (m_context == nullptr)
    {
        this->initialize();
    }
    if (m_sockMode == Server)
    {
        m_socket->bind(str.c_str());
    }
    else
    {
        m_socket->connect(str.c_str());
    }
    m_isStarted = true;
}

//------------------------------------------------------------------------------

void Socket::terminate()
{
    m_isStarted = false;
}

//------------------------------------------------------------------------------

void Socket::setReceiveTimeout(int msec)
{
    m_timeout = msec;
    m_socket->setsockopt(ZMQ_RCVTIMEO, &m_timeout, sizeof(m_timeout));
}

//------------------------------------------------------------------------------

void Socket::stop()
{
    m_socket->close();
    m_context->close();
    m_isStarted = false;
}

//------------------------------------------------------------------------------

std::string Socket::getDeviceNameOut() const
{
    return m_deviceNameOut;
}

//------------------------------------------------------------------------------

void Socket::setDeviceNameOut(const std::string& deviceName)
{
    m_deviceNameOut = deviceName;
}

//------------------------------------------------------------------------------

}//namespace zmqNetwork
