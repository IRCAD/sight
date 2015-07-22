/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlNetwork/INetwork.hpp"
#include "igtlNetwork/Exception.hpp"

#include <fwTools/Stringizer.hpp>

#include <igtlProtocol/MessageFactory.hpp>

namespace igtlNetwork
{

//------------------------------------------------------------------------------

INetwork::INetwork()
{
    m_dataConverter = ::igtlProtocol::DataConverter::getInstance();
}

//------------------------------------------------------------------------------

INetwork::~INetwork()
{
}

//------------------------------------------------------------------------------

bool INetwork::sendObject (::fwData::Object::sptr obj) throw (::fwCore::Exception)
{
    igtl::MessageBase::Pointer msg;

    msg = m_dataConverter->fromFwObject(obj);
    msg->Pack();
    return (m_socket->Send(msg->GetPackPointer(), msg->GetPackSize()) == 1);
}


//------------------------------------------------------------------------------

bool INetwork::receiveObject(::fwData::Object::sptr obj) throw (::fwCore::Exception)
{
    ::igtl::MessageHeader::Pointer headerMsg;
    ::igtl::MessageBase::Pointer msg;

    headerMsg = this->receiveHeader();
    if (headerMsg.IsNotNull())
    {
        msg = this->receiveBody (headerMsg);
        if (msg.IsNotNull())
        {
            m_dataConverter->fromIgtlMessage(msg, obj);
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------

::igtl::MessageHeader::Pointer INetwork::receiveHeader()
{
    ::igtl::MessageHeader::Pointer headerMsg;
    int sizeReceive;

    headerMsg = ::igtl::MessageHeader::New();
    headerMsg->InitPack();
    sizeReceive = m_socket->Receive (headerMsg->GetPackPointer(), headerMsg->GetPackSize());
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
            return headerMsg;
        }
    }
    return ::igtl::MessageHeader::Pointer();
}

//------------------------------------------------------------------------------

::igtl::MessageBase::Pointer INetwork::receiveBody (::igtl::MessageHeader::Pointer const headerMsg)
throw (::fwCore::Exception)
{
    int unpackResult;
    int result;
    ::igtl::MessageBase::Pointer msg;

    msg = ::igtlProtocol::MessageFactory::create(headerMsg->GetDeviceType());
    msg->SetMessageHeader (headerMsg);
    msg->AllocatePack();
    result = m_socket->Receive (msg->GetPackBodyPointer(), msg->GetPackBodySize());
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


} // namespace igtlNetwork

