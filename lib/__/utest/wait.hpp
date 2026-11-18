/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <core/time_stamp.hpp> // NOLINT(misc-include-cleaner): used inside the macros below

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/is_empty_variadic.hpp>
#include <boost/preprocessor/if.hpp> // NOLINT(misc-include-cleaner): used inside the macros below
#include <boost/preprocessor/variadic/elem.hpp>

#include <chrono> // NOLINT(misc-include-cleaner): used inside the macros below
#include <thread> // NOLINT(misc-include-cleaner): used inside the macros below

namespace sight::utest
{

/// How long a test waits at most for a condition. It only stops a test that hangs: the wait ends as soon as the
/// condition holds, so a long value does not slow a test down, even on a busy machine.
/// Not named WAIT_TIMEOUT: the Windows headers define that name as a macro.
inline constexpr std::chrono::milliseconds MAX_WAIT {30000};

} // namespace sight::utest

// NOLINTBEGIN(cppcoreguidelines-macro-usage)

// Waits until a condition holds, at most the given time in milliseconds, or sight::utest::MAX_WAIT by default.
#define SIGHT_TEST_WAIT(cond, ...) \
        sight::core::time_stamp BOOST_PP_CAT(timeStamp, __LINE__); \
        BOOST_PP_CAT( \
            timeStamp, \
            __LINE__ \
        ).set_life_period( \
            BOOST_PP_IF( \
                BOOST_PP_IS_EMPTY(__VA_ARGS__), \
                static_cast<sight::core::time_stamp::time_stamp_type>(sight::utest::MAX_WAIT.count()), \
                BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__) \
            ) \
        ); \
        BOOST_PP_CAT(timeStamp, __LINE__).modified(); \
        while(!(cond) && !BOOST_PP_CAT(timeStamp, __LINE__).period_expired()) \
        { \
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); \
        }

#define SIGHT_TEST_CHECK CHECK

// Same as SIGHT_TEST_WAIT, then checks the condition.
#define SIGHT_TEST_FAIL_WAIT(cond, ...) \
        sight::core::time_stamp BOOST_PP_CAT(timeStamp, __LINE__); \
        BOOST_PP_CAT( \
            timeStamp, \
            __LINE__ \
        ).set_life_period( \
            BOOST_PP_IF( \
                BOOST_PP_IS_EMPTY(__VA_ARGS__), \
                static_cast<sight::core::time_stamp::time_stamp_type>(sight::utest::MAX_WAIT.count()), \
                BOOST_PP_VARIADIC_ELEM(0, __VA_ARGS__) \
            ) \
        ); \
        BOOST_PP_CAT(timeStamp, __LINE__).modified(); \
        while(!(cond) && !BOOST_PP_CAT(timeStamp, __LINE__).period_expired()) \
        { \
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); \
        } \
        SIGHT_TEST_CHECK(cond);

// NOLINTEND(cppcoreguidelines-macro-usage)
