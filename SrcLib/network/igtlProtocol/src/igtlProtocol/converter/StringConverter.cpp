/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "igtlProtocol/converter/StringConverter.hpp"

#include "igtlProtocol/DataConverter.hpp"

#include <fwData/String.hpp>

#include <igtl/igtlStringMessage.h>

namespace igtlProtocol
{
namespace converter
{
const std::string StringConverter::s_IGTL_TYPE          = "STRING";
const std::string StringConverter::s_FWDATA_OBJECT_TYPE = ::fwData::String::classname();

converterRegisterMacro(::igtlProtocol::converter::StringConverter);

StringConverter::StringConverter()
{
}

//-----------------------------------------------------------------------------

StringConverter::~StringConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer StringConverter::fromFwDataObject(::fwData::Object::csptr src) const
{
    ::fwData::String::csptr srcStr = ::fwData::String::dynamicConstCast(src);

    ::igtl::StringMessage::Pointer dest = ::igtl::StringMessage::New();
    dest->SetString(srcStr->getValue().c_str());
    return igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr StringConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::fwData::String::sptr dest = ::fwData::String::New();

    ::igtl::StringMessage* msg            = dynamic_cast< ::igtl::StringMessage* >(src.GetPointer());
    ::igtl::StringMessage::Pointer srcStr = ::igtl::StringMessage::Pointer(msg);
    dest->setValue(std::string(srcStr->GetString()));

    return dest;
}

//-----------------------------------------------------------------------------

IConverter::sptr StringConverter::New()
{
    return std::make_shared< StringConverter >();
}

//-----------------------------------------------------------------------------

std::string const& StringConverter::getIgtlType() const
{
    return StringConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& StringConverter::getFwDataObjectType() const
{
    return StringConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter
} // namespace igtlProtocol
