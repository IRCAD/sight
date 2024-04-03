/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

#include <limits>
#include <map>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace sight::core
{

class SIGHT_CORE_CLASS_API type
{
public:

    constexpr type() = default;
    SIGHT_CORE_API type(const std::string& _str);

    constexpr bool operator==(const type& _t) const;
    constexpr bool operator!=(const type& _t) const;
    constexpr bool operator<(const type& _t) const;

    /// Returns the size of the type
    [[nodiscard]] SIGHT_CORE_API size_t size() const;

    /// Returns true is the type is signed
    [[nodiscard]] SIGHT_CORE_API bool is_signed() const;

    /// Returns the name of the type in a string, i.e. "uint8" for UINT8
    [[nodiscard]] SIGHT_CORE_API const std::string& name() const;

    /// Converts the value in parameter into a string according to this type
    SIGHT_CORE_API std::string to_string(const void* _value) const;

    /// Convert a static C++ type to a dynamic Type
    template<typename T>
    static constexpr type get();

    static const type INT8;
    static const type INT16;
    static const type INT32;
    static const type INT64;

    static const type UINT8;
    static const type UINT16;
    static const type UINT32;
    static const type UINT64;

    static const type FLOAT;
    static const type DOUBLE;
    static const type NONE;

    // This enum is internal and only useful to initialize static members
    enum class type_t : std::uint8_t
    {
        in_t8 = 0,
        in_t16,
        in_t32,
        in_t64,
        uin_t8,
        uin_t16,
        uin_t32,
        uin_t64,
        FLOAT,
        DOUBLE,
        none
    };

    // This constructor is only used to initialize static members
    constexpr type(type_t _t);

    // This allows to use a Type in switch case
    constexpr operator type_t() const;

private:

    // Internal map that stores type properties
    static const std::array<std::tuple<size_t, bool, std::string, type::type_t>, size_t(type::type_t::none) + 1>
    TYPE_PROPERTIES;

    // Type storage
    type_t m_type {type_t::none};
};

//------------------------------------------------------------------------------

constexpr type::type(type_t _t) :
    m_type(_t)
{
}

//------------------------------------------------------------------------------

constexpr bool type::operator==(const type& _t) const
{
    return m_type == _t.m_type;
}

//------------------------------------------------------------------------------

constexpr bool type::operator!=(const type& _t) const
{
    return m_type != _t.m_type;
}

//------------------------------------------------------------------------------

constexpr bool type::operator<(const type& _t) const
{
    return m_type < _t.m_type;
}

//------------------------------------------------------------------------------

constexpr type::operator type_t() const
{
    return m_type;
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::int8_t>()
{
    return {type::type_t::in_t8};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<char>()
{
    return {type::type_t::in_t8};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::int16_t>()
{
    return {type::type_t::in_t16};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::int32_t>()
{
    return {type::type_t::in_t32};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::int64_t>()
{
    return {type::type_t::in_t64};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::uint8_t>()
{
    return {type::type_t::uin_t8};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::uint16_t>()
{
    return {type::type_t::uin_t16};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::uint32_t>()
{
    return {type::type_t::uin_t32};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<std::uint64_t>()
{
    return {type::type_t::uin_t64};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<float>()
{
    return {type::type_t::FLOAT};
}

//------------------------------------------------------------------------------

template<>
constexpr type type::get<double>()
{
    return {type::type_t::DOUBLE};
}

//------------------------------------------------------------------------------

SIGHT_CORE_API std::ostream& operator<<(std::ostream& _os, const sight::core::type& _type);

inline constexpr type type::INT8  = type::type_t::in_t8;
inline constexpr type type::INT16 = type::type_t::in_t16;
inline constexpr type type::INT32 = type::type_t::in_t32;
inline constexpr type type::INT64 = type::type_t::in_t64;

inline constexpr type type::UINT8  = type::type_t::uin_t8;
inline constexpr type type::UINT16 = type::type_t::uin_t16;
inline constexpr type type::UINT32 = type::type_t::uin_t32;
inline constexpr type type::UINT64 = type::type_t::uin_t64;

inline constexpr type type::FLOAT  = type::type_t::FLOAT;
inline constexpr type type::DOUBLE = type::type_t::DOUBLE;
inline constexpr type type::NONE   = type::type_t::none;

} // namespace sight::core
