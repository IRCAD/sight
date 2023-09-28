/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "util_test.hpp"

#include <core/com/util/auto_bind.hpp>
#include <core/com/util/auto_bind.hxx>
#include <core/com/util/convert_function_type.hpp>
#include <core/com/util/remove_last_arg.hpp>

#include <functional>
#include <type_traits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::util_test);

namespace sight::core::com::ut
{

//------------------------------------------------------------------------------

void util_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void util_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

int f(int a, int b)
{
    return a + b;
}

//------------------------------------------------------------------------------

std::string g(const std::string& a, const std::string& b, const std::string& c)
{
    return std::string(a) + b + c;
}

//------------------------------------------------------------------------------

void util_test::auto_bind_test()
{
    CPPUNIT_ASSERT_EQUAL(10, core::com::util::autobind(&f)(4, 6));
    CPPUNIT_ASSERT_EQUAL(42, (core::com::util::auto_bind<int(int), 1>::wrap(f, 40)(2)));
    CPPUNIT_ASSERT_EQUAL(std::string("abc"), core::com::util::autobind(&g)("a", "b", "c"));
}

//-----------------------------------------------------------------------------

class C;

// NOLINTBEGIN(modernize-type-traits)

void util_test::convert_function_type_test()
{
    using CM2C = void (C::*)(int, float) const;
    using CM2  = void (C::*)(int, float);

    using BF2 = std::function<void (int, float)>;

    using F2 = void (int, float);

    CPPUNIT_ASSERT((std::is_same<F2, core::com::util::convert_function_type<CM2C>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F2, core::com::util::convert_function_type<CM2>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F2, core::com::util::convert_function_type<BF2>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F2, core::com::util::convert_function_type<F2*>::type>::value));
}

//-----------------------------------------------------------------------------

void util_test::remove_last_arg_test()
{
    using F9 = std::int64_t(int*, double, float, char, double*, float*, char*, int, const char*);
    using F8 = std::int64_t(int*, double, float, char, double*, float*, char*, int);
    using F7 = std::int64_t(int*, double, float, char, double*, float*, char*);
    using F6 = std::int64_t(int*, double, float, char, double*, float*);
    using F5 = std::int64_t(int*, double, float, char, double*);
    using F4 = std::int64_t(int*, double, float, char);
    using F3 = std::int64_t(int*, double, float);
    using F2 = std::int64_t(int*, double);
    using F1 = std::int64_t(int*);
    using F0 = std::int64_t();

    CPPUNIT_ASSERT((std::is_same<F8, core::com::util::remove_last_arg<F9>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F7, core::com::util::remove_last_arg<F8>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F6, core::com::util::remove_last_arg<F7>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F5, core::com::util::remove_last_arg<F6>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F4, core::com::util::remove_last_arg<F5>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F3, core::com::util::remove_last_arg<F4>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F2, core::com::util::remove_last_arg<F3>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F1, core::com::util::remove_last_arg<F2>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F0, core::com::util::remove_last_arg<F1>::type>::value));
    CPPUNIT_ASSERT((std::is_same<F0, core::com::util::remove_last_arg<F0>::type>::value));
}

//-----------------------------------------------------------------------------

// NOLINTEND(modernize-type-traits)

} // namespace sight::core::com::ut
