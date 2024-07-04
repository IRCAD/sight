/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "vector_functions_test.hpp"

#include <cmath>
#include <core/base.hpp>
#include <core/tools/random/generator.hpp>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <geometry/data/vector_functions.hpp>

// cspell: ignore orthogonalize

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::vector_functions_test);

namespace sight::geometry::data::ut
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

void vector_functions_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void vector_functions_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void vector_functions_test::check_dot()
{
    // Dot product
    const double v1_x = 0.1;
    const double v1_y = safe_rand() % 30 + 0.1;
    const double v1_z = safe_rand() % 20 + 0.4;

    const double v2_x = safe_rand() % 50 + 0.4;
    const double v2_y = 0.5;
    const double v2_z = safe_rand() % 10 + 0.8;

    const fw_vec3d v1 = {v1_x, v1_y, v1_z};
    const fw_vec3d v2 = {v2_x, v2_y, v2_z};
    double result     = NAN;

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::dot");
        for(int i = 0 ; i < 2000000 ; ++i)
        {
            result = geometry::data::dot(V1, V2);
        }
    }
#else
    result = geometry::data::dot(v1, v2);
#endif

    double dot_result = v1_x * v2_x + v1_y * v2_y + v1_z * v2_z;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(dot_result, result, 0.00001);
}

//------------------------------------------------------------------------------

void vector_functions_test::check_cross()
{
    // New version
    const double cross_x = -0.03;
    const double cross_y = 0.06;
    const double cross_z = -0.03;
    const fw_vec3d v1    = {0.1, 0.2, 0.3};
    const fw_vec3d v2    = {0.4, 0.5, 0.6};

    fw_vec3d result;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::cross");
        for(int i = 0 ; i < 2000000 ; ++i)
        {
            result = geometry::data::cross(V1, V2);
        }
    }
#else
    result = geometry::data::cross(v1, v2);
#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(cross_x, result[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(cross_y, result[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(cross_z, result[2], 0.00001);
}

//------------------------------------------------------------------------------

void vector_functions_test::check_normalize()
{
    static constexpr double s_NORM = 0.87749;
    fw_vec3d v                     = {0.4, 0.5, 0.6};

    double norm = NAN;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::normalize");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            v    = {0.4, 0.5, 0.6};
            norm = geometry::data::normalize(v);
        }
    }
#else
    norm = geometry::data::normalize(v);
#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(s_NORM, norm, 0.00001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.4 / s_NORM, v[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5 / s_NORM, v[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.6 / s_NORM, v[2], 0.00001);
}

//------------------------------------------------------------------------------

void vector_functions_test::check_negate()
{
    fw_vec3d v = {1.0, 2.0, 3.0};

    geometry::data::negate(v);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-1.0), v[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-2.0), v[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-3.0), v[2], 0.00001);
}

//------------------------------------------------------------------------------

void vector_functions_test::check_vec_length()
{
    fw_vec3d v = {1.0, 2.0, 3.0};

    double length = NAN;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::normalize");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            length = geometry::data::vecLength(v);
        }
    }
#else
    length = geometry::data::vec_length(v);
#endif

    double expected = 3.741657387;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, length, 0.00001);
}

//------------------------------------------------------------------------------

void vector_functions_test::check_operators()
{
    fw_vec3d vec1 = {1.0, 2.0, 3.0};
    vec1 *= 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(2.0), vec1[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), vec1[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(6.0), vec1[2], 0.00001);

    fw_vec3d vec2 = {1.0, 2.0, 3.0};
    vec2 /= 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.5), vec2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), vec2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.5), vec2[2], 0.00001);

    fw_vec3d add_vec = {2.0, 2.0, 2.0};
    fw_vec3d vec3;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::geometry::data::operator/");
        for(int i = 0 ; i < 1000000 ; ++i)
        {
            vec3  = {1.0, 2.0, 3.0};
            vec3 += addVec;
        }
    }
#else
    vec3  = {1.0, 2.0, 3.0};
    vec3 += add_vec;
#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(3.0), vec3[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), vec3[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(5.0), vec3[2], 0.00001);

    fw_vec3d vec4    = {1.0, 2.0, 3.0};
    fw_vec3d sub_vec = {2.0, 2.0, 2.0};
    vec4 -= sub_vec;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-1.0), vec4[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.0), vec4[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), vec4[2], 0.00001);

    fw_vec3d vec5 = {1.0, 2.0, 3.0};
    fw_vec3d res1 = vec5 * 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(2.0), res1[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), res1[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(6.0), res1[2], 0.00001);

    fw_vec3d vec6 = {1.0, 2.0, 3.0};
    fw_vec3d res2 = vec6 * (-2.0);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-2.0), res2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-4.0), res2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-6.0), res2[2], 0.00001);

    fw_vec3d vec7 = {1.0, 2.0, 3.0};
    fw_vec3d res3 = vec7 / 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.5), res3[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), res3[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.5), res3[2], 0.00001);

    fw_vec3d vec8 = {1.0, 2.0, 3.0};
    fw_vec3d res4 = vec8 + add_vec;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(3.0), res4[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), res4[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(5.0), res4[2], 0.00001);

    fw_vec3d vec9 = {1.0, 2.0, 3.0};
    fw_vec3d res5 = vec9 - sub_vec;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-1.0), res5[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.0), res5[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), res5[2], 0.00001);

    fw_vec3d vec10 = {1.0, 2.0, 3.0};
    fw_vec3d res6  = {1.000000, 2.00000000, 3.00000000000};

    bool test_even = vec10 == res6;
    CPPUNIT_ASSERT_EQUAL(test_even, true);

    fw_vec3d vec11 = {1.0, 2.0, 3.0};
    fw_vec3d res7  = {1.0000001, 2.05, 3.000000000000009};

    bool test_uneven = vec11 != res7;
    CPPUNIT_ASSERT_EQUAL(test_uneven, true);
}

//------------------------------------------------------------------------------

void vector_functions_test::check_orthogonalize()
{
    fw_vec3d u = {0.4, 0.5, 0.6};
    fw_vec3d v = {0.7, 0.8, 0.9};

    CPPUNIT_ASSERT_EQUAL(true, orthogonalize(u, v));
    CPPUNIT_ASSERT_EQUAL(false, orthogonalize(u, v));

    u = {0.4, 0.5, 0.6};
    v = {0.7, 0.8, 0.9};
    fw_vec3d w = {0, 0, 0};

    CPPUNIT_ASSERT_EQUAL(true, orthogonalize(u, v, w));
    CPPUNIT_ASSERT_EQUAL(false, orthogonalize(u, v, w));

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.408248290463863, w[0], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.816496580927726, w[1], 1e-6);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.408248290463862, w[2], 1e-6);
}

} // namespace sight::geometry::data::ut
