/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINT

#include "DataConverter.hpp"

#include <core/lazy_instantiator.hpp>

#include <data/integer.hpp>
#include <data/real.hpp>

#include <igtlCapabilityMessage.h>
#include <igtlStatusMessage.h>

namespace sight::io::igtl::detail
{

//------------------------------------------------------------------------------

DataConverter::sptr DataConverter::getInstance()
{
    return core::lazy_instantiator<DataConverter>::get_instance();
}

//-----------------------------------------------------------------------------

void DataConverter::registerConverter(converter::base::sptr _c)
{
    (DataConverter::getInstance())->m_converters.push_back(_c);
}

//-----------------------------------------------------------------------------

DataConverter::DataConverter()
= default;

//-----------------------------------------------------------------------------

DataConverter::~DataConverter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::fromFwObject(data::object::csptr _src) const
{
    const std::string classname = _src->get_classname();
    for(const converter::base::sptr& converter : m_converters)
    {
        if(converter->getFwDataObjectType() == classname)
        {
            return converter->fromFwDataObject(_src);
        }
    }

    SIGHT_THROW("Object of type " << classname << " is not supported");
}

//-----------------------------------------------------------------------------

data::object::sptr DataConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer _src) const
{
    data::object::sptr obj;
    const std::string device_type = _src->GetDeviceType();

    for(const converter::base::sptr& converter : m_converters)
    {
        if(converter->get_igtl_type() == device_type)
        {
            obj = converter->fromIgtlMessage(_src);
            return obj;
        }
    }

    SIGHT_THROW("Message of type " << std::string(_src->GetDeviceType()) << " is not supported");
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::getStatusMessage(
    int _igtl_code,
    int _igtl_sub_code,
    const std::string& _err_msg
)
{
    ::igtl::StatusMessage::Pointer status_msg;

    status_msg = ::igtl::StatusMessage::New();
    status_msg->SetCode(_igtl_code);
    status_msg->SetErrorName(_err_msg.c_str());
    status_msg->SetSubCode(_igtl_sub_code);
    return ::igtl::MessageBase::Pointer(status_msg); // NOLINT(modernize-return-braced-init-list)
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer DataConverter::getCapabilitiesMessage() const
{
    ::igtl::CapabilityMessage::Pointer msg = ::igtl::CapabilityMessage::New();
    msg->SetNumberOfTypes(static_cast<int>(m_converters.size()));
    for(std::size_t i = 0 ; i < m_converters.size() ; ++i)
    {
        msg->SetType(static_cast<int>(i), m_converters[i]->get_igtl_type().c_str());
    }

    return ::igtl::MessageBase::Pointer(msg); // NOLINT(modernize-return-braced-init-list)
}

} //namespace sight::io::igtl::detail
