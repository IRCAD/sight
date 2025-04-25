/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/igtl/config.hpp>

#include "io/igtl/detail/exception/conversion.hpp"

#include <core/tools/failed.hpp>
#include <core/type.hpp>

#include <map>
#include <string>

namespace sight::io::igtl::detail
{

/**
 * @brief Utility class for image type conversion between igtl data and fwData
 */
class SIGHT_IO_IGTL_CLASS_API image_type_converter
{
public:

    using type_converter_map_t = std::map<core::type, std::uint8_t>;

    /// Not implemented
    image_type_converter() = delete;

    /// Not implemented
    ~image_type_converter() = delete;

    /**
     * @brief init type converter map for conversion between core::type and igtlImage enum Type
     *
     * @return a map contain all equivalence between core::type and igtlImage enum Type
     */

    static type_converter_map_t init_type_converter_map();

    /**
     * @brief get core::type from an igtlType
     *
     * @param[in] _igtl_type
     * @return a core::type
     */
    static const core::type& get_fw_tools_type(const std::uint8_t& _igtl_type);

    /**
     * @brief get igtlType from a core::type
     *
     * @param[in] _type
     * @return uint8 value correspond to an type enum in ::igtl::ImageMessage2
     */
    static std::uint8_t get_igtl_type(const core::type& _type);

private:

    /// type converter map between igtl pixel type and Sight pixel type
    static type_converter_map_t s_type_converter_map;
};

} //namespace sight::io::igtl::detail
