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

#include "ImageTypeConverter.hpp"

#include <igtlImageMessage.h>

namespace sight::io::igtl::detail
{

ImageTypeConverter::TypeConverterMap ImageTypeConverter::s_typeConverterMap =
    ImageTypeConverter::initTypeConverterMap();

//------------------------------------------------------------------------------

ImageTypeConverter::TypeConverterMap ImageTypeConverter::initTypeConverterMap()
{
    ImageTypeConverter::TypeConverterMap convertMap;

    convertMap[core::tools::Type::s_INT8]   = ::igtl::ImageMessage::TYPE_INT8;
    convertMap[core::tools::Type::s_UINT8]  = ::igtl::ImageMessage::TYPE_UINT8;
    convertMap[core::tools::Type::s_INT16]  = ::igtl::ImageMessage::TYPE_INT16;
    convertMap[core::tools::Type::s_UINT16] = ::igtl::ImageMessage::TYPE_UINT16;
    convertMap[core::tools::Type::s_INT32]  = ::igtl::ImageMessage::TYPE_INT32;
    convertMap[core::tools::Type::s_UINT32] = ::igtl::ImageMessage::TYPE_UINT32;
    convertMap[core::tools::Type::s_FLOAT]  = ::igtl::ImageMessage::TYPE_FLOAT32;
    convertMap[core::tools::Type::s_DOUBLE] = ::igtl::ImageMessage::TYPE_FLOAT64;
    return convertMap;
}

//-----------------------------------------------------------------------------

const core::tools::Type& ImageTypeConverter::getFwToolsType(const std::uint8_t& igtlType)
{
    for(const auto& typeConverter : ImageTypeConverter::s_typeConverterMap)
    {
        if(typeConverter.second == igtlType)
        {
            return typeConverter.first;
        }
    }

    throw io::igtl::detail::exception::Conversion("Cannot found a fwTools type for igtl image type");
}

//-----------------------------------------------------------------------------

std::uint8_t ImageTypeConverter::getIgtlType(const core::tools::Type& type)
{
    ImageTypeConverter::TypeConverterMap::const_iterator it;

    it = ImageTypeConverter::s_typeConverterMap.find(type);
    if(it != ImageTypeConverter::s_typeConverterMap.end())
    {
        return it->second;
    }

    throw io::igtl::detail::exception::Conversion("Cannot found a igtl image type for type : " + type.string());
}

} //namespace sight::io::igtl::detail
