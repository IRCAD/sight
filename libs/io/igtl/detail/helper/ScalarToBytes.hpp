/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <boost/type_traits/is_arithmetic.hpp>

#include <algorithm>
#include <string>

namespace sight::io::igtl::detail::helper
{

namespace detail
{

/**
 * @union ConvertData
 *
 * @tparam T T must be a scalar type
 * @brief data structure for scalar and his representation in bytes
 */
template<typename T>
union ConvertData
{
    T scalar;
    std::array<char, sizeof(T)> bytes;
};

} // namespace detail

/**
 *
 * @tparam T T must be a scalar type
 * @brief utility class to convert bytes to scalar and scalar to bytes
 */
template<typename T>
class IO_IGTL_CLASS_API ScalarToBytes
{
public:

    typedef std::vector<char> raw_data_t;

    /**
     * @brief method to convert a scalar to his representation in bytes
     *
     * @param[in] scalar
     * @return the representation in bytes for the given scalar
     */
    static raw_data_t toBytes(T const _scalar)
    {
        io::igtl::detail::helper::detail::ConvertData<T> convert_data {};
        raw_data_t bytes;

        static_assert(std::is_arithmetic<T>::value);
        convert_data.scalar = _scalar;
        bytes.resize(sizeof(T));

        std::copy(convert_data.bytes.begin(), convert_data.bytes.end(), bytes.begin());

        return bytes;
    }

    /**
     * @brief method to convert bytes to a scalar
     *
     * @param[in] bytes
     * @return the scalar value from bytes
     */
    static T fromBytes(const char* const _bytes)
    {
        io::igtl::detail::helper::detail::ConvertData<T> convert_data {};
        std::copy(_bytes, _bytes + sizeof(T), convert_data.bytes.begin());

        return convert_data.scalar;
    }
};

} // namespace sight::io::igtl::detail::helper
