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

#include "ImageTypeConverter.hpp"

#include <igtlImageMessage.h>

namespace sight::io::igtl::detail
{

ImageTypeConverter::TypeConverterMap ImageTypeConverter::s_typeConverterMap =
    ImageTypeConverter::initTypeConverterMap();

//------------------------------------------------------------------------------

ImageTypeConverter::TypeConverterMap ImageTypeConverter::initTypeConverterMap()
{
    ImageTypeConverter::TypeConverterMap convert_map;

    convert_map[core::type::INT8]   = ::igtl::ImageMessage::TYPE_INT8;
    convert_map[core::type::UINT8]  = ::igtl::ImageMessage::TYPE_UINT8;
    convert_map[core::type::INT16]  = ::igtl::ImageMessage::TYPE_INT16;
    convert_map[core::type::UINT16] = ::igtl::ImageMessage::TYPE_UINT16;
    convert_map[core::type::INT32]  = ::igtl::ImageMessage::TYPE_INT32;
    convert_map[core::type::UINT32] = ::igtl::ImageMessage::TYPE_UINT32;
    convert_map[core::type::FLOAT]  = ::igtl::ImageMessage::TYPE_FLOAT32;
    convert_map[core::type::DOUBLE] = ::igtl::ImageMessage::TYPE_FLOAT64;
    return convert_map;
}

//-----------------------------------------------------------------------------

const core::type& ImageTypeConverter::get_fw_tools_type(const std::uint8_t& _igtl_type)
{
    for(const auto& type_converter : ImageTypeConverter::s_typeConverterMap)
    {
        if(type_converter.second == _igtl_type)
        {
            return type_converter.first;
        }
    }

    throw io::igtl::detail::exception::Conversion("Cannot found a fwTools type for igtl image type");
}

//-----------------------------------------------------------------------------

std::uint8_t ImageTypeConverter::get_igtl_type(const core::type& _type)
{
    ImageTypeConverter::TypeConverterMap::const_iterator it;

    it = ImageTypeConverter::s_typeConverterMap.find(_type);
    if(it != ImageTypeConverter::s_typeConverterMap.end())
    {
        return it->second;
    }

    throw io::igtl::detail::exception::Conversion(
              "Cannot found a igtl image type for type : "
              + _type.name()
    );
}

} //namespace sight::io::igtl::detail
