/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "core/Type.hpp"

#include <array>
#include <sstream>

namespace sight::core
{

//------------------------------------------------------------------------------

const Type Type::INT8   = Type_t::INT8;
const Type Type::INT16  = Type_t::INT16;
const Type Type::INT32  = Type_t::INT32;
const Type Type::INT64  = Type_t::INT64;
const Type Type::UINT8  = Type_t::UINT8;
const Type Type::UINT16 = Type_t::UINT16;
const Type Type::UINT32 = Type_t::UINT32;
const Type Type::UINT64 = Type_t::UINT64;
const Type Type::FLOAT  = Type_t::FLOAT;
const Type Type::DOUBLE = Type_t::DOUBLE;
const Type Type::NONE   = Type_t::NONE;

//------------------------------------------------------------------------------

const std::array<std::tuple<size_t, bool, std::string, Type::Type_t>,
                 size_t(Type::Type_t::NONE) + 1> Type::s_typeProperties =
{{
    {sizeof(std::int8_t), true, "int8", Type_t::INT8},
    {sizeof(std::int16_t), true, "int16", Type_t::INT16},
    {sizeof(std::int32_t), true, "int32", Type_t::INT32},
    {sizeof(std::int64_t), true, "int64", Type_t::INT64},
    {sizeof(std::uint8_t), false, "uint8", Type_t::UINT8},
    {sizeof(std::uint16_t), false, "uint16", Type_t::UINT16},
    {sizeof(std::uint32_t), false, "uint32", Type_t::UINT32},
    {sizeof(std::uint64_t), false, "uint64", Type_t::UINT64},
    {sizeof(float), true, "float", Type_t::FLOAT},
    {sizeof(double), true, "double", Type_t::DOUBLE},
    {0, false, "unspecified", Type_t::NONE}
}
};

//------------------------------------------------------------------------------

Type::Type(const std::string& str)
{
    m_type = Type_t::NONE;

    for(const auto& [size, isSigned, name, type] : s_typeProperties)
    {
        if(name == str)
        {
            m_type = type;
            break;
        }
    }
}

//------------------------------------------------------------------------------

size_t Type::size() const
{
    return std::get<0>(s_typeProperties[size_t(m_type)]);
}

//------------------------------------------------------------------------------

bool Type::isSigned() const
{
    return std::get<1>(s_typeProperties[size_t(m_type)]);
}

//------------------------------------------------------------------------------

const std::string& Type::name() const
{
    return std::get<2>(s_typeProperties[size_t(m_type)]);
}

//------------------------------------------------------------------------------

std::string Type::toString(const void* value) const
{
    switch(m_type)
    {
        case Type_t::INT8:
        {
            const auto& v = *(static_cast<const std::int8_t*>(value));
            return std::to_string(v);
        }

        case Type_t::INT16:
        {
            const auto& v = *(static_cast<const std::int16_t*>(value));
            return std::to_string(v);
        }

        case Type_t::INT32:
        {
            const auto& v = *(static_cast<const std::int32_t*>(value));
            return std::to_string(v);
        }

        case Type_t::INT64:
        {
            const auto& v = *(static_cast<const std::int64_t*>(value));
            return std::to_string(v);
        }

        case Type_t::UINT8:
        {
            const auto& v = *(static_cast<const std::uint8_t*>(value));
            return std::to_string(v);
        }

        case Type_t::UINT16:
        {
            const auto& v = *(static_cast<const std::uint16_t*>(value));
            return std::to_string(v);
        }

        case Type_t::UINT32:
        {
            const auto& v = *(static_cast<const std::uint32_t*>(value));
            return std::to_string(v);
        }

        case Type_t::UINT64:
        {
            const auto& v = *(static_cast<const std::uint64_t*>(value));
            return std::to_string(v);
        }

        case Type_t::FLOAT:
        {
            const auto& v = *(static_cast<const float*>(value));
            return std::to_string(v);
        }

        case Type_t::DOUBLE:
        {
            const auto& v = *(static_cast<const double*>(value));
            return std::to_string(v);
        }

        case Type_t::NONE:
            return "";
    }

    return "";
}

} // namespace sight::core

namespace std
{

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const sight::core::Type& type)
{
    os << type.name();
    return os;
}

} // namespace std
