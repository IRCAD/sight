/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "fwTools/config.hpp"
#include "fwTools/Type.hpp"
#include "fwTools/TypeMapping.hpp"

namespace fwTools
{

/**
 * @name Type mapping used for Dispatcher
 * @{
 */

/**
 * @brief   Template specialization for defining KeyType mapping ( here Type) with their value.
 *
 * Header declare function already specialized while src provide definition
 * @warning isMapping does not need to be exported since fwTools Library is builded as static
 */
// FIX UNSIGNED
template<> FWTOOLS_API
bool isMapping<std::uint8_t>(const Type& type);

template<> FWTOOLS_API
bool isMapping<std::uint16_t>(const Type& type);

template<> FWTOOLS_API
bool isMapping<std::uint32_t>(const Type& type);

template<> FWTOOLS_API
bool isMapping<std::uint64_t>(const Type& type);

// FIXED PRECISION SIGNED
template<> FWTOOLS_API
bool isMapping<std::int8_t>(const Type& type);

template<> FWTOOLS_API
bool isMapping<std::int16_t>(const Type& type);

template<> FWTOOLS_API
bool isMapping<std::int32_t>(const Type& type);

template<> FWTOOLS_API
bool isMapping<std::int64_t>(const Type& type);

// FLOATING PRECISION
template<> FWTOOLS_API
bool isMapping<float>(const Type& type);

template<> FWTOOLS_API
bool isMapping<double>(const Type& type);

// MISC
template<> FWTOOLS_API
bool isMapping<std::string>(const Type& type);

///@}

} //end namespace fwTools
