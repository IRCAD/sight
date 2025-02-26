/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE Wclass memaccess

#pragma once

#include <sight/core/config.hpp>

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>

namespace sight::core::crypto
{

/// This class is an implementation of basic_string Allocator. Its main feature is secure deletion of allocated data
/// See https://en.cppreference.com/w/cpp/named_req/Allocator
template<typename T>
struct allocator
{
    using value_type                             = T;
    using propagate_on_container_move_assignment =
        typename std::allocator_traits<std::allocator<T> >::propagate_on_container_move_assignment;

    constexpr allocator() = default;

    template<class U>
    constexpr allocator(const allocator<U>& /*unused*/) noexcept
    {
    }

    //------------------------------------------------------------------------------

    constexpr static T* allocate(std::size_t _n)
    {
        return std::allocator<T> {}.allocate(_n);
    }

    //------------------------------------------------------------------------------

    constexpr static void deallocate(T* _p, std::size_t _n) noexcept
    {
        std::fill_n(((volatile char*) _p), sizeof(T) * _n, static_cast<char>(0));
        std::allocator<T> {}.deallocate(_p, _n);
    }
};

//------------------------------------------------------------------------------

template<typename T, typename U>
constexpr bool operator==(allocator<T>/*unused*/, allocator<U>/*unused*/) noexcept
{
    return true;
}

//------------------------------------------------------------------------------

template<typename T, typename U>
constexpr bool operator!=(allocator<T>/*unused*/, allocator<U>/*unused*/) noexcept
{
    return false;
}

using secure_string = std::basic_string<char, std::char_traits<char>, allocator<char> >;

} // namespace sight::core::crypto

// Zeroes the strings own memory on destruction
template<>
#if defined(WIN32)
constexpr sight::core::crypto::secure_string::~basic_string() noexcept
#elif (_GLIBCXX_RELEASE >= 12)
constexpr sight::core::crypto::secure_string::~basic_string()
#else
inline sight::core::crypto::secure_string::~basic_string()
#endif
{
    clear();
    shrink_to_fit();
}
