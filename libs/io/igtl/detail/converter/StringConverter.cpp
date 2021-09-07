/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/igtl/detail/converter/StringConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/String.hpp>

#include <igtlStringMessage.h>

namespace sight::io::igtl::detail
{

namespace converter
{

const std::string StringConverter::s_IGTL_TYPE          = "STRING";
const std::string StringConverter::s_FWDATA_OBJECT_TYPE = data::String::classname();

converterRegisterMacro(io::igtl::detail::converter::StringConverter);

StringConverter::StringConverter()
{
}

//-----------------------------------------------------------------------------

StringConverter::~StringConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer StringConverter::fromFwDataObject(data::Object::csptr src) const
{
    data::String::csptr srcStr = data::String::dynamicConstCast(src);

    ::igtl::StringMessage::Pointer dest = ::igtl::StringMessage::New();
    dest->SetString(srcStr->getValue().c_str());
    return ::igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

data::Object::sptr StringConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    data::String::sptr dest = data::String::New();

    ::igtl::StringMessage* msg            = dynamic_cast< ::igtl::StringMessage*>(src.GetPointer());
    ::igtl::StringMessage::Pointer srcStr = ::igtl::StringMessage::Pointer(msg);
    dest->setValue(std::string(srcStr->GetString()));

    return dest;
}

//-----------------------------------------------------------------------------

IConverter::sptr StringConverter::New()
{
    return std::make_shared<StringConverter>();
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

} // namespace sight::io::igtl::detail
