/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <type_traits>

/**
 * @brief This is a set of operators for enum classes, so we are allowed to use bitwise operations on them.
 *
 */

//------------------------------------------------------------------------------

template<typename T, typename = typename std::enable_if_t<std::is_enum<T>::value, T> >
constexpr T operator|(T _lhs, T _rhs)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T> >(_lhs)
                          | static_cast<std::underlying_type_t<T> >(_rhs));
}

//------------------------------------------------------------------------------

template<typename T, typename = typename std::enable_if_t<std::is_enum<T>::value, T> >
constexpr T operator&(T _lhs, T _rhs)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T> >(_lhs)
                          & static_cast<std::underlying_type_t<T> >(_rhs));
}

//------------------------------------------------------------------------------

template<typename T, typename = typename std::enable_if_t<std::is_enum<T>::value, T> >
constexpr T operator^(T _lhs, T _rhs)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T> >(_lhs)
                          ^ static_cast<std::underlying_type_t<T> >(_rhs));
}

//------------------------------------------------------------------------------

template<typename T, typename = typename std::enable_if_t<std::is_enum<T>::value, T> >
constexpr T operator~(T _rhs)
{
    return static_cast<T>(~static_cast<std::underlying_type_t<T> >(_rhs));
}

//------------------------------------------------------------------------------

template<typename T, typename = typename std::enable_if_t<std::is_enum<T>::value, T> >
constexpr T& operator|=(T& _lhs, const T& _rhs)
{
    return _lhs = _lhs | _rhs;
}

//------------------------------------------------------------------------------

template<typename T, typename = typename std::enable_if_t<std::is_enum<T>::value, T> >
constexpr T& operator&=(T& _lhs, const T& _rhs)
{
    return _lhs = _lhs & _rhs;
}

//------------------------------------------------------------------------------

template<typename T, typename = typename std::enable_if_t<std::is_enum<T>::value, T> >
constexpr T& operator^=(T& _lhs, const T& _rhs)
{
    return _lhs = _lhs ^ _rhs;
}
