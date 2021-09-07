/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <string>

namespace sight::core::tools
{

/**
 * @name    String type mapping
 * @{
 */
/**
 * @brief   Template specialization for defining KeyType mapping ( here std::string) with their value.
 *
 * Header declare function already specialized while src provide definition.
 *
 * @warning isMapping does not need to be exported since fwTools Library is builded as static.
 *
 */

// FIX UNSIGNED
template<>
CORE_API
bool isMapping<unsigned char>(const std::string& key);

template<>
CORE_API
bool isMapping<unsigned short>(const std::string& key);

template<>
CORE_API
bool isMapping<unsigned int>(const std::string& key);

template<>
CORE_API
bool isMapping<unsigned long>(const std::string& key);

// FIXED PRECISION SIGNED
template<>
CORE_API
bool isMapping<signed char>(const std::string& key);

template<>
CORE_API
bool isMapping<signed short>(const std::string& key);

template<>
CORE_API
bool isMapping<signed int>(const std::string& key);

template<>
CORE_API
bool isMapping<signed long>(const std::string& key);

// FLOATING PRECISION
template<>
CORE_API
bool isMapping<float>(const std::string& key);

template<>
CORE_API
bool isMapping<double>(const std::string& key);

// MISC
template<>
CORE_API
bool isMapping<std::string>(const std::string& key);
///@}

} // end namespace sight::core::tools
