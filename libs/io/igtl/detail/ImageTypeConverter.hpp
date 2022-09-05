/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/igtl/config.hpp"
#include "io/igtl/detail/exception/Conversion.hpp"

#include <core/tools/Failed.hpp>
#include <core/Type.hpp>

#include <map>
#include <string>

namespace sight::io::igtl::detail
{

/**
 * @brief Utility class for image type conversion between igtl data and fwData
 */
class IO_IGTL_CLASS_API ImageTypeConverter
{
public:

    typedef std::map<core::Type, std::uint8_t> TypeConverterMap;

    /// Not implemented
    ImageTypeConverter() = delete;

    /// Not implemented
    ~ImageTypeConverter() = delete;

    /**
     * @brief init type converter map for conversion between core::Type and igtlImage enum Type
     *
     * @return a map contain all equivalence between core::Type and igtlImage enum Type
     */

    static TypeConverterMap initTypeConverterMap();

    /**
     * @brief get core::Type from an igtlType
     *
     * @param[in] igtlType
     * @return a core::Type
     */
    static const core::Type& getFwToolsType(const std::uint8_t& igtlType);

    /**
     * @brief get igtlType from a core::Type
     *
     * @param[in] type
     * @return uint8 value correspond to an type enum in ::igtl::ImageMessage2
     */
    static std::uint8_t getIgtlType(const core::Type& type);

private:

    /// type converter map between igtl pixel type and Sight pixel type
    static TypeConverterMap s_typeConverterMap;
};

} //namespace sight::io::igtl::detail
