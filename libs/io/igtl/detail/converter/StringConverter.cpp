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

#include <data/string.hpp>

#include <igtlStringMessage.h>

namespace sight::io::igtl::detail::converter
{

const std::string string_converter::IGTL_TYPE          = "STRING";
const std::string string_converter::FWDATA_OBJECT_TYPE = data::string::classname();

CONVERTER_REGISTER_MACRO(io::igtl::detail::converter::string_converter);

string_converter::string_converter()
= default;

//-----------------------------------------------------------------------------

string_converter::~string_converter()
= default;

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer string_converter::from_fw_data_object(data::object::csptr _src) const
{
    data::string::csptr src_str = std::dynamic_pointer_cast<const data::string>(_src);

    ::igtl::StringMessage::Pointer dest = ::igtl::StringMessage::New();
    dest->SetString(src_str->get_value().c_str());
    return {dest.GetPointer()};
}

//-----------------------------------------------------------------------------

data::object::sptr string_converter::from_igtl_message(const ::igtl::MessageBase::Pointer _src) const
{
    data::string::sptr dest = std::make_shared<data::string>();

    auto* msg                              = dynamic_cast< ::igtl::StringMessage*>(_src.GetPointer());
    ::igtl::StringMessage::Pointer src_str = ::igtl::StringMessage::Pointer(msg);
    dest->set_value(std::string(src_str->GetString()));

    return dest;
}

//-----------------------------------------------------------------------------

base::sptr string_converter::New()
{
    return std::make_shared<string_converter>();
}

//-----------------------------------------------------------------------------

std::string const& string_converter::get_igtl_type() const
{
    return string_converter::IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& string_converter::get_fw_data_object_type() const
{
    return string_converter::FWDATA_OBJECT_TYPE;
}

} // namespace sight::io::igtl::detail::converter
