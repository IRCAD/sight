/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/tools/TypeMapping.hpp"
#include "core/Type.hpp"

#include <boost/mpl/vector.hpp>

namespace sight::core::tools
{

/**
 * @name numeric mapping used for Dispatcher
 * @{
 */

/**
 * @brief   Template specialization for defining KeyType mapping ( here numeric) with their value.
 *
 * Header declare function already specialized while src provide definition
 * @warning isMapping does not need to be exported since fwTools Library is builded as static
 */
template<>
CORE_API
bool isMapping<std::uint8_t>(const Type& type);

template<>
CORE_API
bool isMapping<std::uint16_t>(const Type& type);

template<>
CORE_API
bool isMapping<std::uint32_t>(const Type& type);

template<>
CORE_API
bool isMapping<std::uint64_t>(const Type& type);

// FIXED PRECISION SIGNED
template<>
CORE_API
bool isMapping<std::int8_t>(const Type& type);

template<>
CORE_API
bool isMapping<std::int16_t>(const Type& type);

template<>
CORE_API
bool isMapping<std::int32_t>(const Type& type);

template<>
CORE_API
bool isMapping<std::int64_t>(const Type& type);

// FLOATING PRECISION
template<>
CORE_API
bool isMapping<float>(const Type& type);

template<>
CORE_API
bool isMapping<double>(const Type& type);

// MISC
template<>
CORE_API
bool isMapping<std::string>(const Type& type);

///@}
///
typedef boost::mpl::vector<
        std::int8_t,
        std::uint8_t,
        std::int16_t,
        std::uint16_t,
        std::int32_t,
        std::uint32_t,
        std::int64_t,
        std::uint64_t,
        float,
        double
>::type SupportedDispatcherTypes;

} //end namespace sight::core::tools
