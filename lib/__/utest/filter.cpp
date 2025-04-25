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

#include "utest/filter.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace sight::utest
{

//------------------------------------------------------------------------------

inline static bool get_ignore_tests_env(const char* const _env)
{
    const char* const value = std::getenv(_env);
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
        << _env
        << "' has been set."
        << std::endl;
    }

    return ignored;
}

//------------------------------------------------------------------------------

bool filter::ignore_slow_tests()
{
    static const bool s_IGNORE_SLOW_TESTS = get_ignore_tests_env("SIGHT_IGNORE_SLOW_TESTS");
    return s_IGNORE_SLOW_TESTS;
}

//------------------------------------------------------------------------------

bool filter::ignore_unstable_tests()
{
    static const bool s_IGNORE_UNSTABLE_TESTS = get_ignore_tests_env("SIGHT_IGNORE_UNSTABLE_TESTS");
    return s_IGNORE_UNSTABLE_TESTS;
}

} // namespace sight::utest
