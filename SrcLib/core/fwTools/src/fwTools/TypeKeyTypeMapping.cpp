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

#include "fwTools/TypeKeyTypeMapping.hpp"

namespace fwTools
{

// FIX UNSIGNED
template<>
bool isMapping<std::uint8_t>(const Type& type)
{
    return type.isOfType<std::int8_t>();
}
//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint16_t>(const Type& type)
{
    return type.isOfType<std::uint16_t>();
}
//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint32_t>(const Type& type)
{
    return type.isOfType<std::uint32_t>();
}
//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint64_t>(const Type& type)
{
    return type.isOfType<std::uint64_t>();
}

// FIXED PRECISION SIGNED
template<>
bool isMapping<std::int8_t>(const Type& type)
{
    return type.isOfType<std::int8_t>();
}
//------------------------------------------------------------------------------

template<>
bool isMapping<std::int16_t>(const Type& type)
{
    return type.isOfType<std::int16_t>();
}
//------------------------------------------------------------------------------

template<>
bool isMapping<std::int32_t>(const Type& type)
{
    return type.isOfType<std::int32_t>();
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int64_t>(const Type& type)
{
    return type.isOfType<std::int64_t>();
}

// FLOATING PRECISION
template<>
bool isMapping<float>(const Type& type)
{
    return type.isOfType<float>();
}
//------------------------------------------------------------------------------

template<>
bool isMapping<double>(const Type& type)
{
    return type.isOfType<double>();
}

// MISC
template<>
bool isMapping<std::string>(const Type& type)
{
    return type.isOfType<std::string>();
}

} // end namespace fwTools {
