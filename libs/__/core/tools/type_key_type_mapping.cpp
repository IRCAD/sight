/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "core/tools/type_key_type_mapping.hpp"

namespace sight::core::tools
{

// FIX UNSIGNED
template<>
bool is_mapping<std::uint8_t>(const type& type)
{
    return type == type::UINT8;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<std::uint16_t>(const type& type)
{
    return type == type::UINT16;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<std::uint32_t>(const type& type)
{
    return type == type::UINT32;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<std::uint64_t>(const type& type)
{
    return type == type::UINT64;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<std::int8_t>(const type& type)
{
    return type == type::INT8;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<std::int16_t>(const type& type)
{
    return type == type::INT16;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<std::int32_t>(const type& type)
{
    return type == type::INT32;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<std::int64_t>(const type& type)
{
    return type == type::INT64;
}

// FLOATING PRECISION
template<>
bool is_mapping<float>(const type& type)
{
    return type == type::FLOAT;
}

//------------------------------------------------------------------------------

template<>
bool is_mapping<double>(const type& type)
{
    return type == type::DOUBLE;
}

} // namespace sight::core::tools
