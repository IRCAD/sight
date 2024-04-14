/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <core/tools/random/generator.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <algorithm>
#include <cmath>

namespace sight::geometry::eigen::ut
{

class eigen_tools_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(eigen_tools_test);
    CPPUNIT_TEST(f4s_to_eigen);
    CPPUNIT_TEST(from_eigen);
    CPPUNIT_TEST(float16_to_eigen);
    CPPUNIT_TEST(eigen_mat_to_rvec_tvec);
    CPPUNIT_TEST(f4s_mat_to_rvec_tvec);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void f4s_to_eigen();
    static void from_eigen();

    static void float16_to_eigen();

    static void eigen_mat_to_rvec_tvec();
    static void f4s_mat_to_rvec_tvec();

    //------------------------------------------------------------------------------

    template<class T>
    static T random(const T& _a, const T& _b)
    {
        return static_cast<T>(core::tools::random::safe_rand()) / static_cast<T>(RAND_MAX) * (_b - _a) + _a;
    }
};

} // namespace sight::geometry::eigen::ut
