/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include <algorithm>
#include <memory>
#include <string>

namespace sight::core::crypto
{

/// Zeroes the pointed memory
/// @param p memory address
/// @param n length to zero
inline void cleanse(void* p, std::size_t n) noexcept
{
    // "volatile" guard from unwanted optimization
    std::fill_n(static_cast<volatile char*>(p), n, 0);
}

/// This class is an implementation of basic_string Allocator. Its main feature is secure deletion of allocated data
/// See https://en.cppreference.com/w/cpp/named_req/Allocator
template<typename T>
struct allocator
{
    using value_type                             = T;
    using propagate_on_container_move_assignment =
        typename std::allocator_traits<std::allocator<T> >::propagate_on_container_move_assignment;

    inline constexpr allocator() = default;

    template<class U>
    inline constexpr allocator(const allocator<U>&) noexcept
    {
    }

    //------------------------------------------------------------------------------

    inline constexpr static T* allocate(std::size_t n)
    {
        return std::allocator<T> {}.allocate(n);
    }

    //------------------------------------------------------------------------------

    inline static void deallocate(T* p, std::size_t n) noexcept
    {
        cleanse(p, n * sizeof(T));
        std::allocator<T> {}.deallocate(p, n);
    }
};

//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool operator==(allocator<T>, allocator<U>) noexcept
{
    return true;
}

//------------------------------------------------------------------------------

template<typename T, typename U>
inline constexpr bool operator!=(allocator<T>, allocator<U>) noexcept
{
    return false;
}

using secure_string = std::basic_string<char, std::char_traits<char>, allocator<char> >;

} // sight::core::crypto

// Zeroes the strings own memory on destruction
template<>
inline sight::core::crypto::secure_string::~basic_string()
{
    clear();
    shrink_to_fit();
    sight::core::crypto::cleanse(this, sizeof(*this));
}
