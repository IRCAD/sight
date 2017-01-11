/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IGTLPROTOCOL_IMAGETYPECONVERTER_HPP__
#define __IGTLPROTOCOL_IMAGETYPECONVERTER_HPP__

#include "igtlProtocol/config.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

#include <fwTools/Failed.hpp>
#include <fwTools/Type.hpp>

#include <boost/type.hpp>

#include <string>
#include <map>

namespace igtlProtocol
{
/**
 * @brief Utility class for image type conversion between igtl data and fwData
 */
class IGTLPROTOCOL_CLASS_API ImageTypeConverter
{
public:

    typedef std::map< ::fwTools::Type, ::boost::uint8_t> TypeConverterMap;

    /**
     * @brief init type converter map for conversion between fwTools::Type and igtlImage enum Type
     *
     * @return a map contain all equivalence between fwTools::Type and igtlImage enum Type
     */

    static TypeConverterMap initTypeConverterMap();

    /**
     * @brief get fwTools::Type from an igtlType
     *
     * @param[in] igtlType
     * @return a fwTools::Type
     */
    static ::fwTools::Type const& getFwToolsType(::boost::uint8_t igtlType) throw(::igtlProtocol::exception::Conversion);

    /**
     * @brief get igtlType from a fwTools::Type
     *
     * @param[in] type
     * @return uint8 value correspond to an type enum in igtl::ImageMessage2
     */
    static ::boost::uint8_t getIgtlType(::fwTools::Type const& type) throw(::igtlProtocol::exception::Conversion);

private:

    /// Not implemented
    ImageTypeConverter();

    /// Not implemented
    ~ImageTypeConverter();

    /// type converter map between igtl pixel type and F4S pixel type
    static TypeConverterMap s_typeConverterMap;
};

}//namespace igtlProtocol

#endif // __IGTLPROTOCOL_IMAGETYPECONVERTER_HPP__
