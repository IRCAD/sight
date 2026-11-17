/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <cmath>
#include <concepts>
#include <cstdint>

namespace sight::core
{

/**
 * @brief Rounds a floating-point value to the nearest integer using round-half-to-even
 * (banker's rounding), matching the behavior of Python's built-in @c round().
 *
 * Unlike @c std::round(), which rounds 0.5 away from zero, this function rounds 0.5
 * toward the nearest even integer:
 * @code
 *   round_half_to_even(0.5)  == 0   // 0 is even
 *   round_half_to_even(1.5)  == 2   // 2 is even
 *   round_half_to_even(2.5)  == 2   // 2 is even
 *   round_half_to_even(3.5)  == 4   // 4 is even
 *   round_half_to_even(-0.5) == 0   // 0 is even
 *   round_half_to_even(-1.5) == -2  // -2 is even
 * @endcode
 *
 * @tparam TReturn Integer type to return (defaults to @c std::int64_t).
 * @tparam TFloat  Floating-point input type, deduced from the argument.
 * @param  _value  Value to round.
 * @return Nearest integer as @c TReturn, with ties broken toward the nearest even number.
 */
template<typename TReturn = std::int64_t, std::floating_point TFloat>
[[nodiscard]] TReturn round_half_to_even(TFloat _value)
{
    const TFloat floor_val  = std::floor(_value);
    const TFloat fractional = _value - floor_val;

    if(fractional < TFloat(0.5))
    {
        return static_cast<TReturn>(floor_val);
    }

    if(fractional > TFloat(0.5))
    {
        return static_cast<TReturn>(floor_val) + TReturn(1);
    }

    // Exactly 0.5: round toward the nearest even integer.
    const TReturn floor_ret = static_cast<TReturn>(floor_val);
    return (floor_ret % TReturn(2) == TReturn(0)) ? floor_ret : floor_ret + TReturn(1);
}

} // namespace sight::core
