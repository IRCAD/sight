/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/base.hpp>

#include <algorithm>
#include <ctime>
#include <functional>
#include <mutex>
#include <random>
#include <type_traits>

namespace sight::core::tools::random
{

/**
 * @brief On each invocation, it returns a random value uniformly distributed in the range [min..max).
 * @param min      The "min" parameter of the distribution.
 * @param max      The "max" parameter of the distribution.
 * @param seedVal  Seed for generator.
 * @pre   min <= max
 *
 * @return returns a random value uniformly distributed in the range [min..max)
 */
template<typename T>
T getValue(T min, T max, std::uint32_t seedVal = static_cast<std::uint32_t>(std::time(nullptr)))
{
    SIGHT_ASSERT("Wrong min/max value", min <= max);
    typedef typename std::conditional<
            std::is_floating_point<T>::value,
            std::uniform_real_distribution<T>,
            std::uniform_int_distribution<T>
    >::type DistroType;

    std::mt19937 gen(seedVal);
    DistroType dist(min, max);
    return dist(gen);
}

/**
 * @brief On each invocation, this method fills specified container with random values uniformly distributed in the
 * range [min..max).
 * @param[in] min            The "min" parameter of the distribution.
 * @param[in] max            The "max" parameter of the distribution.
 * @param[out] randContainer Container to fill with random values.
 * @param[in] seedVal        Seed for generator.
 * @pre  min <= max
 * @pre  CONTAINER type same as template T
 */
template<typename T, typename CONTAINER>
void fillContainer(
    T min,
    T max,
    CONTAINER& randContainer,
    std::uint32_t seedVal = static_cast<std::uint32_t>(std::time(nullptr))
)
{
    SIGHT_ASSERT("Wrong min/max value", min <= max);
    SIGHT_ASSERT("Container type not same as T", (std::is_same<T, typename CONTAINER::value_type>::value));
    typedef typename std::conditional<
            std::is_floating_point<T>::value,
            std::uniform_real_distribution<T>,
            std::uniform_int_distribution<T>
    >::type DistroType;

    std::mt19937 gen(seedVal);
    DistroType dist(min, max);
    const auto generator = [&dist, &gen](auto&& ...){return dist(gen);};
    std::generate(randContainer.begin(), randContainer.end(), generator);
}

/// This is a thread safe "std::rand" implementation
CORE_API int safeRand(bool reset = false);

} // namespace sight::core::tools::random
