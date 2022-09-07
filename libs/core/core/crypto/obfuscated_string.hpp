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

// cspell:ignore NOLINTNEXTLINE hicpp

#pragma once

#include "core/config.hpp"
#include "core/crypto/secure_string.hpp"

#include <array>
#include <iostream>

namespace sight::core::crypto
{

template<std::uint32_t S, std::uint32_t A = 16807UL, std::uint32_t C = 0UL, std::uint32_t M = (1UL << 31) - 1>
struct LinearGenerator
{
    static const std::uint32_t state = ((std::uint64_t) S * A + C) % M;
    static const std::uint32_t value = state;
    using next = LinearGenerator<state>;
    struct Split
    {
        // Leapfrog
        using Gen1 = LinearGenerator<state, A* A, 0, M>;
        using Gen2 = LinearGenerator<next::state, A* A, 0, M>;
    };
};

// Metafunction to get a particular index from generator
template<std::uint32_t S, std::size_t index>
struct Generate
{
    static const std::uint8_t value = Generate<LinearGenerator<S>::state, index - 1>::value;
};

template<std::uint32_t S>
struct Generate<S, 0>
{
    static const std::uint8_t value = static_cast<std::uint8_t>(LinearGenerator<S>::value);
};

// List of indices
template<std::size_t ...>
struct StList {};

// Concatenate
template<typename TL, typename TR>
struct Concat;

template<std::size_t ... SL, std::size_t ... SR>
struct Concat<StList<SL ...>, StList<SR ...> >
{
    using type = StList<SL ..., SR ...>;
};

template<typename TL, typename TR>
using Concat_t = typename Concat<TL, TR>::type;

// Count from zero to n-1
template<std::size_t s>
struct Count
{
    using type = Concat_t<typename Count<s - 1>::type, StList<s - 1> >;
};

template<>
struct Count<0>
{
    using type = StList<>;
};

template<std::size_t s>
using Count_t = typename Count<s>::type;

// Get a scrambled character of a string
template<std::uint32_t seed, std::size_t index, std::size_t N>
constexpr std::uint8_t get_scrambled_char(const std::array<char, N>& a)
{
    return static_cast<std::uint8_t>(a[index]) + Generate<seed, index>::value;
}

// Get a ciphertext from a plaintext string
template<std::uint32_t seed, typename T>
struct cipher_helper;

template<std::uint32_t seed, std::size_t ... SL>
struct cipher_helper<seed, StList<SL ...> >
{
    //------------------------------------------------------------------------------

    static constexpr std::array<std::uint8_t, sizeof...(SL)> get_array(const std::array<char, sizeof...(SL)>& a)
    {
        return {{get_scrambled_char<seed, SL>(a) ...}};
    }
};

//------------------------------------------------------------------------------

template<std::uint32_t seed, std::size_t N>
constexpr std::array<std::uint8_t, N> get_cipher_text(const std::array<char, N>& a)
{
    return cipher_helper<seed, Count_t<N> >::get_array(a);
}

// Get a noise sequence from a seed and string length
template<std::uint32_t seed, typename T>
struct noise_helper;

template<std::uint32_t seed, std::size_t ... SL>
struct noise_helper<seed, StList<SL ...> >
{
    //------------------------------------------------------------------------------

    static constexpr std::array<std::uint8_t, sizeof...(SL)> get_array()
    {
        return {{Generate<seed, SL>::value ...}};
    }
};

//------------------------------------------------------------------------------

template<std::uint32_t seed, std::size_t N>
constexpr std::array<std::uint8_t, N> get_key()
{
    return noise_helper<seed, Count_t<N> >::get_array();
}

template<typename T, std::size_t N>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
constexpr std::array<T, N> to_array(const char (& a)[N])
{
    std::array<T, N> res {};
    for(std::size_t i = 0 ; i < N ; i++)
    {
        res[i] = a[i];
    }

    return res;
}

// Scramble a string
template<std::uint32_t seed, std::size_t N>
class obfuscated_string
{
private:

    std::array<std::uint8_t, N> cipher_text_;
    std::array<std::uint8_t, N> key_;

public:

    explicit constexpr obfuscated_string( /*const char (& a)[N]*/ const std::array<char, N>& a) :
        cipher_text_(get_cipher_text<seed, N>(a)),
        key_(get_key<seed, N>())
    {
    }

    operator sight::core::crypto::secure_string() const
    {
        std::array<char, N> plain_text;
        for(std::size_t i = 0 ; i < N ; ++i)
        {
            const char temp = static_cast<char>(cipher_text_[i] - key_[i]);
            plain_text[i] = temp;
        }

        return sight::core::crypto::secure_string {plain_text.begin(), plain_text.end()};
    }
};

//------------------------------------------------------------------------------

template<std::uint32_t seed, std::size_t N>
std::ostream& operator<<(std::ostream& s, const sight::core::crypto::obfuscated_string<seed, N>& str)
{
    s << static_cast<sight::core::crypto::secure_string>(str);
    return s;
}

#define RNG_SEED ((__TIME__[7] - '0') * 1 + (__TIME__[6] - '0') * 10 \
                  + (__TIME__[4] - '0') * 60 + (__TIME__[3] - '0') * 600 \
                  + (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000) \
    + (__LINE__ * 100000)

#define OBFUSCATED_STR(STR) \
    sight::core::crypto::obfuscated_string<RNG_SEED, sizeof(STR)> {sight::core::crypto::to_array<char, sizeof(STR)>(STR) \
    }

} // namespace sight::core::crypto
