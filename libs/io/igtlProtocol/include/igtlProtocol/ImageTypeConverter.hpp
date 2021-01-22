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

#pragma once

#include "igtlProtocol/config.hpp"
#include "igtlProtocol/exception/Conversion.hpp"

#include <core/tools/Failed.hpp>
#include <core/tools/Type.hpp>

#include <map>
#include <string>

namespace igtlProtocol
{
/**
 * @brief Utility class for image type conversion between igtl data and fwData
 */
class IGTLPROTOCOL_CLASS_API ImageTypeConverter
{
public:

    typedef std::map< core::tools::Type, std::uint8_t> TypeConverterMap;

    /**
     * @brief init type converter map for conversion between core::tools::Type and igtlImage enum Type
     *
     * @return a map contain all equivalence between core::tools::Type and igtlImage enum Type
     */

    static TypeConverterMap initTypeConverterMap();

    /**
     * @brief get core::tools::Type from an igtlType
     *
     * @param[in] igtlType
     * @return a core::tools::Type
     */
    static const core::tools::Type& getFwToolsType(const std::uint8_t& igtlType);

    /**
     * @brief get igtlType from a core::tools::Type
     *
     * @param[in] type
     * @return uint8 value correspond to an type enum in igtl::ImageMessage2
     */
    static std::uint8_t getIgtlType(const core::tools::Type& type);

private:

    /// Not implemented
    ImageTypeConverter();

    /// Not implemented
    ~ImageTypeConverter();

    /// type converter map between igtl pixel type and Sight pixel type
    static TypeConverterMap s_typeConverterMap;
};

}//namespace igtlProtocol
