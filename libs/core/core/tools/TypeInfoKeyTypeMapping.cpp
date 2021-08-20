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

#include "core/tools/TypeInfoKeyTypeMapping.hpp"

#include <typeinfo>

namespace sight::core::tools
{

/**
 * @brief template specialization for defining KeyType mapping ( here std::type_info) with their value
 */
template<class T>
bool isMappingTypeInfoCommon(const std::type_info& key)
{
    return key == typeid(T);
}

// warning specialization from a partial specialization is not possible

// FIX UNSIGNED
template<>
bool isMapping<std::uint8_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::uint8_t>(key);
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint16_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::uint16_t>(key);
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint32_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::uint32_t>(key);
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint64_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::uint64_t>(key);
}

// FIXED PRECISION SIGNED
template<>
bool isMapping<std::int8_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::int8_t>(key);
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int16_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::int16_t>(key);
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int32_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::int32_t>(key);
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int64_t>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::int64_t>(key);
}

// FLOATING PRECISION
template<>
bool isMapping<float>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<float>(key);
}

//------------------------------------------------------------------------------

template<>
bool isMapping<double>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<double>(key);
}

// MISC
template<>
bool isMapping<std::string>(const std::type_info& key)
{
    return isMappingTypeInfoCommon<std::string>(key);
}

} // end namespace sight::core::tools {
