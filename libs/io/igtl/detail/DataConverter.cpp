/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "DataConverter.hpp"

#include <core/LazyInstantiator.hpp>

#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <igtlCapabilityMessage.h>
#include <igtlStatusMessage.h>

namespace sight::io::igtl::detail
{

//------------------------------------------------------------------------------

DataConverter::sptr DataConverter::getInstance()
{
    return core::LazyInstantiator<DataConverter>::getInstance();
}

//-----------------------------------------------------------------------------

void DataConverter::registerConverter(converter::IConverter::sptr c)
{
    (DataConverter::getInstance())->m_converters.push_back(c);
}

//-----------------------------------------------------------------------------

DataConverter::DataConverter()
{
}

//-----------------------------------------------------------------------------

DataConverter::~DataConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::fromFwObject(data::Object::csptr src) const
{
    const std::string classname = src->getClassname();
    for(const converter::IConverter::sptr& converter : m_converters)
    {
        if(converter->getFwDataObjectType() == classname)
        {
            return converter->fromFwDataObject(src);
        }
    }

    SIGHT_THROW("Object of type " << classname << " is not supported");
}

//-----------------------------------------------------------------------------

data::Object::sptr DataConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    data::Object::sptr obj;
    const std::string deviceType = src->GetDeviceType();

    for(const converter::IConverter::sptr& converter : m_converters)
    {
        if(converter->getIgtlType() == deviceType)
        {
            obj = converter->fromIgtlMessage(src);
            return obj;
        }
    }

    SIGHT_THROW("Message of type " << std::string(src->GetDeviceType()) << " is not supported");
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::getStatusMessage(
    int igtlCode,
    int igtlSubCode,
    const std::string& errMsg
) const
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
    for(std::size_t i = 0 ; i < m_converters.size() ; ++i)
    {
        msg->SetType(static_cast<int>(i), m_converters[i]->getIgtlType().c_str());
    }

    return ::igtl::MessageBase::Pointer(msg);
}

} //namespace sight::io::igtl::detail
