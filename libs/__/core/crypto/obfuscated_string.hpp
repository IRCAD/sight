/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE hicpp

#pragma once

#include <sight/core/config.hpp>

#include "core/crypto/secure_string.hpp"

#include <array>
#include <cstdint>
#include <iostream>

namespace sight::core::crypto
{

template<std::uint32_t S, std::uint32_t A = 16807UL, std::uint32_t C = 0UL, std::uint32_t M = (1UL << 31) - 1>
struct linear_generator
{
    static const std::uint32_t STATE = ((std::uint64_t) S * A + C) % M;
    static const std::uint32_t VALUE = STATE;
    using next = linear_generator<STATE>;
    struct split
    {
        // Leapfrog
        using gen1 = linear_generator<STATE, A* A, 0, M>;
        using gen2 = linear_generator<next::state, A* A, 0, M>;
    };
};

// Metafunction to get a particular index from generator
template<std::uint32_t S, std::size_t index>
struct generate
{
    static const std::uint8_t VALUE = generate<linear_generator<S>::STATE, index - 1>::VALUE;
};

template<std::uint32_t S>
struct generate<S, 0>
{
    static const std::uint8_t VALUE = static_cast<std::uint8_t>(linear_generator<S>::VALUE);
};

// List of indices
template<std::size_t ...>
struct st_list {};

// Concatenate
template<typename TL, typename TR>
struct concat;

template<std::size_t ... SL, std::size_t ... SR>
struct concat<st_list<SL ...>, st_list<SR ...> >
{
    using type = st_list<SL ..., SR ...>;
};

template<typename TL, typename TR>
using concat_t = typename concat<TL, TR>::type;

// Count from zero to n-1
template<std::size_t s>
struct count
{
    using type = concat_t<typename count<s - 1>::type, st_list<s - 1> >;
};

template<>
struct count<0>
{
    using type = st_list<>;
};

template<std::size_t s>
using count_t = typename count<s>::type;

// Get a scrambled character of a string
template<std::uint32_t seed, std::size_t index, std::size_t N>
constexpr std::uint8_t get_scrambled_char(const std::array<char, N>& _a)
{
    return static_cast<std::uint8_t>(_a[index]) + generate<seed, index>::VALUE;
}

// Get a ciphertext from a plaintext string
template<std::uint32_t seed, typename T>
struct cipher_helper;

template<std::uint32_t seed, std::size_t ... SL>
struct cipher_helper<seed, st_list<SL ...> >
{
    //------------------------------------------------------------------------------

    static constexpr std::array<std::uint8_t, sizeof...(SL)> get_array(const std::array<char, sizeof...(SL)>& _a)
    {
        return {{get_scrambled_char<seed, SL>(_a) ...}};
    }
};

//------------------------------------------------------------------------------

template<std::uint32_t seed, std::size_t N>
constexpr std::array<std::uint8_t, N> get_cipher_text(const std::array<char, N>& _a)
{
    return cipher_helper<seed, count_t<N> >::get_array(_a);
}

// Get a noise sequence from a seed and string length
template<std::uint32_t seed, typename T>
struct noise_helper;

template<std::uint32_t seed, std::size_t ... SL>
struct noise_helper<seed, st_list<SL ...> >
{
    //------------------------------------------------------------------------------

    static constexpr std::array<std::uint8_t, sizeof...(SL)> get_array()
    {
        return {{generate<seed, SL>::VALUE ...}};
    }
};

//------------------------------------------------------------------------------

template<std::uint32_t seed, std::size_t N>
constexpr std::array<std::uint8_t, N> get_key()
{
    return noise_helper<seed, count_t<N> >::get_array();
}

template<typename T, std::size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
constexpr std::array<T, N> to_array(const char (& _a)[N])
{
    std::array<T, N> res {};
    for(std::size_t i = 0 ; i < N ; i++)
    {
        res[i] = _a[i];
    }

    return res;
}

// Scramble a string
template<std::uint32_t seed, std::size_t N>
class obfuscated_string
{
private:

    std::array<std::uint8_t, N> m_cipher_text;
    std::array<std::uint8_t, N> m_key;

public:

    explicit constexpr obfuscated_string(/*const char (& a)[N]*/ const std::array<char, N>& _a) :
        m_cipher_text(get_cipher_text<seed, N>(_a)),
        m_key(get_key<seed, N>())
    {
    }

    operator sight::core::crypto::secure_string() const
    {
        std::array<char, N> plain_text;
        for(std::size_t i = 0 ; i < N ; ++i)
        {
            const char temp = static_cast<char>(m_cipher_text[i] - m_key[i]);
            plain_text[i] = temp;
        }

        return sight::core::crypto::secure_string {plain_text.begin(), plain_text.end()};
    }
};

//------------------------------------------------------------------------------

template<std::uint32_t seed, std::size_t N>
std::ostream& operator<<(std::ostream& _s, const sight::core::crypto::obfuscated_string<seed, N>& _str)
{
    _s << static_cast<sight::core::crypto::secure_string>(_str);
    return _s;
}

#define RNG_SEED ((__TIME__[7] - '0') * 1 + (__TIME__[6] - '0') * 10 \
                  + (__TIME__[4] - '0') * 60 + (__TIME__[3] - '0') * 600 \
                  + (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000) \
        + (__LINE__ * 100000)

#define OBFUSCATED_STR(STR) \
        sight::core::crypto::obfuscated_string<RNG_SEED, \
                                               sizeof(STR)> {sight::core::crypto::to_array<char, sizeof(STR)>(STR) \
        }

} // namespace sight::core::crypto
