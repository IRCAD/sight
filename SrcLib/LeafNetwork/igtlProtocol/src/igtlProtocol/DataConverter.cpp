/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/DataConverter.hpp"

#include "igtlProtocol/converter/AtomConverter.hpp"

#include <fwCore/util/LazyInstantiator.hpp>

#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

#include <igtl/igtlCapabilityMessage.h>
#include <igtl/igtlStatusMessage.h>

namespace igtlProtocol
{

//------------------------------------------------------------------------------

DataConverter::sptr DataConverter::getInstance()
{
    return ::fwCore::util::LazyInstantiator< DataConverter >::getInstance();
}

//-----------------------------------------------------------------------------

void DataConverter::registerConverter(converter::IConverter::sptr c)
{
    (DataConverter::getInstance())->m_converters.push_back(c);
}

//-----------------------------------------------------------------------------

DataConverter::DataConverter()
{
    m_defaultConverter = ::igtlProtocol::converter::AtomConverter::New();
}

//-----------------------------------------------------------------------------

DataConverter::~DataConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::fromFwObject(::fwData::Object::csptr src) const
{
    const std::string classname = src->getClassname();
    for(const converter::IConverter::sptr& converter :  m_converters)
    {
        if (converter->getFwDataObjectType() == classname)
        {
            ::fwCore::mt::ReadLock lock(src->getMutex());
            return converter->fromFwDataObject(src);
        }
    }
    return m_defaultConverter->fromFwDataObject(src);
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr DataConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::fwData::Object::sptr obj;
    const std::string deviceType = src->GetDeviceType();

    if (deviceType == "ATOMS")
    {
        obj = m_defaultConverter->fromIgtlMessage(src);
        return obj;
    }

    for(const converter::IConverter::sptr& converter : m_converters)
    {
        if (converter->getIgtlType() == deviceType)
        {
            obj = converter->fromIgtlMessage(src);
            return obj;
        }
    }

    SLM_WARN("Message type not supported : " + std::string(src->GetDeviceType()));
    return obj;
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::getStatusMessage(int igtlCode,
                                                             int igtlSubCode,
                                                             const std::string& errMsg) const
{
    ::igtl::StatusMessage::Pointer statusMsg;

    statusMsg = ::igtl::StatusMessage::New();
    statusMsg->SetCode(igtlCode);
    statusMsg->SetErrorName(errMsg.c_str());
    statusMsg->SetSubCode(igtlSubCode);
    return ::igtl::MessageBase::Pointer(statusMsg);
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::getCapabilitiesMessage() const
{
    ::igtl::CapabilityMessage::Pointer msg = ::igtl::CapabilityMessage::New();
    msg->SetNumberOfTypes(static_cast<int>(m_converters.size()));
    for(size_t i = 0; i < m_converters.size(); ++i)
    {
        msg->SetType(static_cast<int>(i), m_converters[i]->getIgtlType().c_str());
    }
    return ::igtl::MessageBase::Pointer(msg);
}

}//namespace igtlProtocol
