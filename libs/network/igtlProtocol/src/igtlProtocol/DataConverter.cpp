/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "igtlProtocol/DataConverter.hpp"

#include "igtlProtocol/converter/AtomConverter.hpp"

#include <fwCore/util/LazyInstantiator.hpp>

#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

#include <igtlCapabilityMessage.h>
#include <igtlStatusMessage.h>

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
