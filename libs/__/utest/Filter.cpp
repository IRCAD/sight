/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "utest/Filter.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace sight::utest
{

//------------------------------------------------------------------------------

inline static bool getIgnoreTestsEnv(const char* const env)
{
    const char* const value = std::getenv(env);
    if(value == nullptr)
    {
        return false;
    }

    const bool ignored = std::strncmp(value, "0", 1) != 0;

    if(ignored)
    {
        /// Print something on console, usefull for CI console output
        std::cerr << std::endl
        << "Some tests will be ignored, environment variable '"
        << env
        << "' has been set."
        << std::endl;
    }

    return ignored;
}

//------------------------------------------------------------------------------

bool Filter::ignoreSlowTests()
{
    static const bool IGNORE_SLOW_TESTS = getIgnoreTestsEnv("SIGHT_IGNORE_SLOW_TESTS");
    return IGNORE_SLOW_TESTS;
}

//------------------------------------------------------------------------------

bool Filter::ignoreUnstableTests()
{
    static const bool IGNORE_UNSTABLE_TESTS = getIgnoreTestsEnv("SIGHT_IGNORE_UNSTABLE_TESTS");
    return IGNORE_UNSTABLE_TESTS;
}

} // namespace sight::utest
