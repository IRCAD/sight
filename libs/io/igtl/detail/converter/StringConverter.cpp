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

#include "io/igtl/detail/converter/StringConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"

#include <data/String.hpp>

#include <igtlStringMessage.h>

namespace sight::io::igtl::detail::converter
{

const std::string StringConverter::s_IGTL_TYPE          = "STRING";
const std::string StringConverter::s_FWDATA_OBJECT_TYPE = data::String::classname();

converterRegisterMacro(io::igtl::detail::converter::StringConverter);

StringConverter::StringConverter()
= default;

//-----------------------------------------------------------------------------

StringConverter::~StringConverter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer StringConverter::fromFwDataObject(data::Object::csptr src) const
{
    data::String::csptr srcStr = std::dynamic_pointer_cast<const data::String>(src);

    ::igtl::StringMessage::Pointer dest = ::igtl::StringMessage::New();
    dest->SetString(srcStr->getValue().c_str());
    return {dest.GetPointer()};
}

//-----------------------------------------------------------------------------

data::Object::sptr StringConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    data::String::sptr dest = std::make_shared<data::String>();

    auto* msg                             = dynamic_cast< ::igtl::StringMessage*>(src.GetPointer());
    ::igtl::StringMessage::Pointer srcStr = ::igtl::StringMessage::Pointer(msg);
    dest->setValue(std::string(srcStr->GetString()));

    return dest;
}

//-----------------------------------------------------------------------------

base::sptr StringConverter::New()
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

} // namespace sight::io::igtl::detail::converter
