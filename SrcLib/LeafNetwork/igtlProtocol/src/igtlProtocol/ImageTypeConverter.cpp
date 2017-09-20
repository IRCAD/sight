/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/ImageTypeConverter.hpp"

#include <igtl/igtlImageMessage.h>

namespace igtlProtocol
{

ImageTypeConverter::TypeConverterMap ImageTypeConverter::s_typeConverterMap =
    ImageTypeConverter::initTypeConverterMap();

//------------------------------------------------------------------------------

ImageTypeConverter::TypeConverterMap ImageTypeConverter::initTypeConverterMap()
{
    ImageTypeConverter::TypeConverterMap convertMap;

    convertMap[::fwTools::Type::s_INT8]   = ::igtl::ImageMessage::TYPE_INT8;
    convertMap[::fwTools::Type::s_UINT8]  = ::igtl::ImageMessage::TYPE_UINT8;
    convertMap[::fwTools::Type::s_INT16]  = ::igtl::ImageMessage::TYPE_INT16;
    convertMap[::fwTools::Type::s_UINT16] = ::igtl::ImageMessage::TYPE_UINT16;
    convertMap[::fwTools::Type::s_INT32]  = ::igtl::ImageMessage::TYPE_INT32;
    convertMap[::fwTools::Type::s_UINT32] = ::igtl::ImageMessage::TYPE_UINT32;
    convertMap[::fwTools::Type::s_FLOAT]  = ::igtl::ImageMessage::TYPE_FLOAT32;
    convertMap[::fwTools::Type::s_DOUBLE] = ::igtl::ImageMessage::TYPE_FLOAT64;
    return convertMap;
}

//-----------------------------------------------------------------------------

::fwTools::Type const &
ImageTypeConverter::getFwToolsType(std::uint8_t igtlType)
{
    ImageTypeConverter::TypeConverterMap::const_iterator it;

    for (it = ImageTypeConverter::s_typeConverterMap.begin();
         it != ImageTypeConverter::s_typeConverterMap.end();
         ++it)
    {
        if (it->second == igtlType)
        {
            return it->first;
        }
    }
    throw ::igtlProtocol::exception::Conversion("Cannot found a fwTools type for igtl image type");
}

//-----------------------------------------------------------------------------

std::uint8_t ImageTypeConverter::getIgtlType (::fwTools::Type const& type)
{
    ImageTypeConverter::TypeConverterMap::const_iterator it;

    it = ImageTypeConverter::s_typeConverterMap.find(type);
    if (it != ImageTypeConverter::s_typeConverterMap.end())
    {
        return it->second;
    }
    throw ::igtlProtocol::exception::Conversion("Cannot found a igtl image type for type : " + type.string());
}

}//namespace igtlProtocol
