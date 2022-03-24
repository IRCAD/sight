/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <any>
#include <limits>
#include <map>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace sight::core
{

class CORE_CLASS_API Type
{
public:

    constexpr Type() = default;
    CORE_API Type(const std::string& str);

    constexpr bool operator==(const Type& t) const;
    constexpr bool operator!=(const Type& t) const;
    constexpr bool operator<(const Type& t) const;

    /// Returns the size of the type
    CORE_API size_t size() const;

    /// Returns true is the type is signed
    CORE_API bool isSigned() const;

    /// Returns the name of the type in a string, i.e. "uint8" for UINT8
    CORE_API const std::string& name() const;

    /// Converts the value in parameter into a string according to this type
    CORE_API std::string toString(const void* value) const;

    /// Convert a static C++ type to a dynamic Type
    template<typename T>
    static constexpr Type get();

    CORE_API static const Type INT8;
    CORE_API static const Type INT16;
    CORE_API static const Type INT32;
    CORE_API static const Type INT64;

    CORE_API static const Type UINT8;
    CORE_API static const Type UINT16;
    CORE_API static const Type UINT32;
    CORE_API static const Type UINT64;

    CORE_API static const Type FLOAT;
    CORE_API static const Type DOUBLE;
    CORE_API static const Type NONE;

private:

    // This enum is internal and only useful to initialize static members
    enum class Type_t : std::uint8_t
    {
        INT8 = 0,
        INT16,
        INT32,
        INT64,
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        FLOAT,
        DOUBLE,
        NONE
    };

    // Internal map that stores type properties
    static const std::array<std::tuple<size_t, bool, std::string, Type::Type_t>, size_t(Type::Type_t::NONE) + 1>
    s_typeProperties;

    // This constructor is only used to initialize static members
    constexpr Type(Type_t t);

    // Type storage
    Type_t m_type {Type_t::NONE};
};

//------------------------------------------------------------------------------

constexpr Type::Type(Type_t t) :
    m_type(t)
{
}

//------------------------------------------------------------------------------

constexpr bool Type::operator==(const Type& t) const
{
    return m_type == t.m_type;
}

//------------------------------------------------------------------------------

constexpr bool Type::operator!=(const Type& t) const
{
    return m_type != t.m_type;
}

//------------------------------------------------------------------------------

constexpr bool Type::operator<(const Type& t) const
{
    return m_type < t.m_type;
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::int8_t>()
{
    return Type(Type::Type_t::INT8);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<char>()
{
    return Type(Type::Type_t::INT8);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::int16_t>()
{
    return Type(Type::Type_t::INT16);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::int32_t>()
{
    return Type(Type::Type_t::INT32);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::int64_t>()
{
    return Type(Type::Type_t::INT64);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::uint8_t>()
{
    return Type(Type::Type_t::UINT8);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::uint16_t>()
{
    return Type(Type::Type_t::UINT16);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::uint32_t>()
{
    return Type(Type::Type_t::UINT32);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<std::uint64_t>()
{
    return Type(Type::Type_t::UINT64);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<float>()
{
    return Type(Type::Type_t::FLOAT);
}

//------------------------------------------------------------------------------

template<>
constexpr Type Type::get<double>()
{
    return Type(Type::Type_t::DOUBLE);
}

} // namespace sight::core

//------------------------------------------------------------------------------
namespace std
{

CORE_API std::ostream& operator<<(std::ostream& os, const sight::core::Type& type);

} // namespace std
