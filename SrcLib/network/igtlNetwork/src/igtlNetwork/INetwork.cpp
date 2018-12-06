/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
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

#include "igtlNetwork/INetwork.hpp"

#include "igtlNetwork/Exception.hpp"

#include <fwTools/Stringizer.hpp>

#include <igtlProtocol/MessageFactory.hpp>

namespace igtlNetwork
{

//------------------------------------------------------------------------------

INetwork::INetwork() :
    m_filteringByDeviceName(false),
    m_deviceNameOut("Sight")
{
    m_dataConverter = ::igtlProtocol::DataConverter::getInstance();
}

//------------------------------------------------------------------------------

INetwork::~INetwork()
{
}

//------------------------------------------------------------------------------

bool INetwork::sendObject(const ::fwData::Object::csptr& obj)
{
    igtl::MessageBase::Pointer msg;

    msg = m_dataConverter->fromFwObject(obj);
    msg->SetDeviceName(m_deviceNameOut.c_str());
    msg->Pack();
    return (m_socket->Send(msg->GetPackPointer(), msg->GetPackSize()) == 1);
}

//------------------------------------------------------------------------------

bool INetwork::sendMsg (igtl::MessageBase::Pointer msg)
{
    msg->SetDeviceName(m_deviceNameOut.c_str());
    msg->Pack();
    return (m_socket->Send(msg->GetPackPointer(), msg->GetPackSize()) == 1);
}

//------------------------------------------------------------------------------

::fwData::Object::sptr INetwork::receiveObject(std::string& deviceName)
{
    ::fwData::Object::sptr obj;
    ::igtl::MessageHeader::Pointer headerMsg = this->receiveHeader();
    if (headerMsg.IsNotNull())
    {
        ::igtl::MessageBase::Pointer msg = this->receiveBody(headerMsg);
        if (msg.IsNotNull())
        {
            obj        = m_dataConverter->fromIgtlMessage(msg);
            deviceName = headerMsg->GetDeviceName();
        }
    }
    return obj;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr INetwork::receiveObject(std::string& deviceName, double& timestamp)
{
    ::fwData::Object::sptr obj;
    ::igtl::MessageHeader::Pointer headerMsg = this->receiveHeader();
    if (headerMsg.IsNotNull())
    {
        ::igtl::MessageBase::Pointer msg = this->receiveBody(headerMsg);
        if (msg.IsNotNull())
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

            obj        = m_dataConverter->fromIgtlMessage(msg);
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

    if (sizeReceive == -1 || sizeReceive == 0)
    {
        return ::igtl::MessageHeader::Pointer();
    }
    else
    {
        if (sizeReceive != 0 && sizeReceive != headerMsg->GetPackSize())
        {
            return igtl::MessageHeader::Pointer();
        }
        else if (headerMsg->Unpack() & ::igtl::MessageBase::UNPACK_HEADER)
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
                    return igtl::MessageHeader::Pointer();
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

::igtl::MessageBase::Pointer INetwork::receiveBody (::igtl::MessageHeader::Pointer const headerMsg)
{
    int unpackResult;
    int result;
    ::igtl::MessageBase::Pointer msg;

    msg = ::igtlProtocol::MessageFactory::create(headerMsg->GetDeviceType());
    msg->SetMessageHeader(headerMsg);
    msg->AllocatePack();
    result = m_socket->Receive(msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if (result == -1)
    {
        return ::igtl::MessageBase::Pointer();
    }

    unpackResult = msg->Unpack(1);
    if (unpackResult & igtl::MessageHeader::UNPACK_BODY)
    {
        return msg;
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
    std::set< std::string >::iterator it = m_deviceNamesIn.find(deviceName);

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

} // namespace igtlNetwork
