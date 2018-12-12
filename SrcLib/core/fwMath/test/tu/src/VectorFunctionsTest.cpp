/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include "VectorFunctionsTest.hpp"

#include <fwCore/base.hpp>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwMath/VectorFunctions.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMath::ut::VectorFunctionsTest );

namespace fwMath
{
namespace ut
{

void VectorFunctionsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkDot()
{
    // Dot product
    const double V1_X = 0.1;
    const double V1_Y = std::rand()%30+0.1;
    const double V1_Z = std::rand()%20+0.4;

    const double V2_X = std::rand()%50+0.4;
    const double V2_Y = 0.5;
    const double V2_Z = std::rand()%10+0.8;

    const fwVec3d V1 = {V1_X, V1_Y, V1_Z};
    const fwVec3d V2 = {V2_X, V2_Y, V2_Z};
    double result;

#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwMath::dot");
        for(int i = 0; i < 2000000; ++i)
        {
            result = ::fwMath::dot(V1, V2);
        }
    }
#else
    result = ::fwMath::dot(V1, V2);
#endif

    double dotResult = V1_X * V2_X + V1_Y  *V2_Y + V1_Z * V2_Z;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(dotResult, result, 0.00001);
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkCross()
{
    // Dot product.
    const double DISTANCE = 10.25;

    // New version
    const double CROOS_X = -0.03;
    const double CROSS_Y = 0.06;
    const double CROSS_Z = -0.03;
    const fwVec3d V1     = {0.1, 0.2, 0.3};
    const fwVec3d V2     = {0.4, 0.5, 0.6};

    fwVec3d result;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwMath::cross");
        for(int i = 0; i < 2000000; ++i)
        {
            result = ::fwMath::cross(V1, V2);
        }
    }
#else
    result = ::fwMath::cross(V1, V2);
#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(CROOS_X, result[0],  0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(CROSS_Y, result[1],  0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(CROSS_Z, result[2],  0.00001);
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkNormalize()
{
    const double NORM = 0.87749;
    fwVec3d v         = {0.4, 0.5, 0.6};

    double norm;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwMath::normalize");
        for(int i = 0; i < 1000000; ++i)
        {
            v    = {0.4, 0.5, 0.6};
            norm = ::fwMath::normalize(v);
        }
    }
#else
    v    = {0.4, 0.5, 0.6};
    norm = ::fwMath::normalize(v);
#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(NORM, norm, 0.00001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.4/NORM, v[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5/NORM, v[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.6/NORM, v[2], 0.00001);
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkNegate()
{
    fwVec3d v = {1.0, 2.0, 3.0};

    ::fwMath::negate(v);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-1.0), v[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-2.0), v[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-3.0), v[2], 0.00001);
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkVecLength()
{
    fwVec3d v = {1.0, 2.0, 3.0};

    double length;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwMath::normalize");
        for(int i = 0; i < 1000000; ++i)
        {
            length = ::fwMath::vecLength(v);
        }
    }
#else
    length = ::fwMath::vecLength(v);
#endif

    double expected = 3.741657387;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected,length, 0.00001);
}

//------------------------------------------------------------------------------

void VectorFunctionsTest::checkOperators()
{
    fwVec3d vec1 = {1.0, 2.0, 3.0};
    vec1 *= 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(2.0), vec1[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), vec1[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(6.0), vec1[2], 0.00001);

    fwVec3d vec2 = {1.0, 2.0, 3.0};
    vec2 /= 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.5), vec2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), vec2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.5), vec2[2], 0.00001);

    fwVec3d addVec = {2.0, 2.0, 2.0};
    fwVec3d vec3;
#ifndef FW_PROFILING_DISABLED
    {
        FW_PROFILE("::fwMath::operator/");
        for(int i = 0; i < 1000000; ++i)
        {
            vec3  = {1.0, 2.0, 3.0};
            vec3 += addVec;
        }
    }
#else

    vec3  = {1.0, 2.0, 3.0};
    vec3 += addVec;
#endif

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(3.0), vec3[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), vec3[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(5.0), vec3[2], 0.00001);

    fwVec3d vec4   = {1.0, 2.0, 3.0};
    fwVec3d subVec = {2.0, 2.0, 2.0};
    vec4 -= subVec;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-1.0), vec4[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.0), vec4[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), vec4[2], 0.00001);

    fwVec3d vec5 = {1.0, 2.0, 3.0};
    fwVec3d res1 = vec5 * 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(2.0), res1[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), res1[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(6.0), res1[2], 0.00001);

    fwVec3d vec6 = {1.0, 2.0, 3.0};
    fwVec3d res2 = vec6 * (-2.0);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-2.0), res2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-4.0), res2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-6.0), res2[2], 0.00001);


    fwVec3d vec7 = {1.0, 2.0, 3.0};
    fwVec3d res3 = vec7 / 2.0;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.5), res3[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), res3[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.5), res3[2], 0.00001);

    fwVec3d vec8 = {1.0, 2.0, 3.0};
    fwVec3d res4 = vec8 + addVec;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(3.0), res4[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(4.0), res4[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(5.0), res4[2], 0.00001);

    fwVec3d vec9 = {1.0, 2.0, 3.0};
    fwVec3d res5 = vec9 - subVec;

    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(-1.0), res5[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(0.0), res5[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(double(1.0), res5[2], 0.00001);

    fwVec3d vec10 = {1.0, 2.0, 3.0};
    fwVec3d res6  = {1.000000, 2.00000000, 3.00000000000};

    int testEven = (vec10 == res6);
    CPPUNIT_ASSERT_EQUAL(testEven, 1);

    fwVec3d vec11 = { 1.0, 2.0, 3.0 };
    fwVec3d res7  = { 1.0000001, 2.05, 3.000000000000009 };

    int testUneven = (vec11 != res7);
    CPPUNIT_ASSERT_EQUAL(testUneven, 1);
}

//------------------------------------------------------------------------------
} //namespace ut
} //namespace fwMath
