/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/com/util/auto_bind.hpp>
#include <core/com/util/auto_bind.hxx>
#include <core/com/util/convert_function_type.hpp>
#include <core/com/util/remove_last_arg.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>

TEST_SUITE("sight::core::com::util")
{
//-----------------------------------------------------------------------------

    static int f(int _a, int _b)
    {
        return _a + _b;
    }

//------------------------------------------------------------------------------

    static std::string g(const std::string& _a, const std::string& _b, const std::string& _c)
    {
        return std::string(_a) + _b + _c;
    }

//------------------------------------------------------------------------------

    TEST_CASE("auto_bind")
    {
        CHECK_EQ(sight::core::com::util::autobind(&f)(4, 6), 10);
        CHECK_EQ((sight::core::com::util::auto_bind<int(int), 1>::wrap(f, 40)(2)), 42);
        CHECK_EQ(sight::core::com::util::autobind(&g)("a", "b", "c"), std::string("abc"));
    }

//-----------------------------------------------------------------------------

    class C;

// NOLINTBEGIN(modernize-type-traits)

    TEST_CASE("convert_function_type")
    {
        using c_m2_c = void (C::*)(int, float) const;
        using c_m2   = void (C::*)(int, float);

        using b_f2 = std::function<void (int, float)>;

        using f2 = void (int, float);

        CHECK((std::is_same<f2, sight::core::com::util::convert_function_type<c_m2_c>::type>::value));
        CHECK((std::is_same<f2, sight::core::com::util::convert_function_type<c_m2>::type>::value));
        CHECK((std::is_same<f2, sight::core::com::util::convert_function_type<b_f2>::type>::value));
        CHECK((std::is_same<f2, sight::core::com::util::convert_function_type<f2*>::type>::value));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("remove_last_arg")
    {
        using f9 = std::int64_t(int*, double, float, char, double*, float*, char*, int, const char*);
        using f8 = std::int64_t(int*, double, float, char, double*, float*, char*, int);
        using f7 = std::int64_t(int*, double, float, char, double*, float*, char*);
        using f6 = std::int64_t(int*, double, float, char, double*, float*);
        using f5 = std::int64_t(int*, double, float, char, double*);
        using f4 = std::int64_t(int*, double, float, char);
        using f3 = std::int64_t(int*, double, float);
        using f2 = std::int64_t(int*, double);
        using f1 = std::int64_t(int*);
        using f0 = std::int64_t();

        CHECK((std::is_same<f8, sight::core::com::util::remove_last_arg<f9>::type>::value));
        CHECK((std::is_same<f7, sight::core::com::util::remove_last_arg<f8>::type>::value));
        CHECK((std::is_same<f6, sight::core::com::util::remove_last_arg<f7>::type>::value));
        CHECK((std::is_same<f5, sight::core::com::util::remove_last_arg<f6>::type>::value));
        CHECK((std::is_same<f4, sight::core::com::util::remove_last_arg<f5>::type>::value));
        CHECK((std::is_same<f3, sight::core::com::util::remove_last_arg<f4>::type>::value));
        CHECK((std::is_same<f2, sight::core::com::util::remove_last_arg<f3>::type>::value));
        CHECK((std::is_same<f1, sight::core::com::util::remove_last_arg<f2>::type>::value));
        CHECK((std::is_same<f0, sight::core::com::util::remove_last_arg<f1>::type>::value));
        CHECK((std::is_same<f0, sight::core::com::util::remove_last_arg<f0>::type>::value));
    }

//-----------------------------------------------------------------------------

// NOLINTEND(modernize-type-traits)
} // TEST_SUITE
