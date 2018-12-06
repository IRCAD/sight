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

#include "PlaneFunctionsTest.hpp"

#include <fwMath/PlaneFunctions.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMath::ut::PlaneFunctionsTest );

namespace fwMath
{
namespace ut
{
//------------------------------------------------------------------------------

void PlaneFunctionsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkGetPlane()
{
    const double PlanP1_X = 1.0;
    const double PlanP1_Y = 0.0;
    const double PlanP1_Z = 0.0;

    const double PlanP2_X = 0.0;
    const double PlanP2_Y = 0.0;
    const double PlanP2_Z = 1.0;

    const double PlanP3_X = 0.0;
    const double PlanP3_Y = 2.0;
    const double PlanP3_Z = 1.0;

    const fwVec3d planPt1 = {{PlanP1_X, PlanP1_Y, PlanP1_Z}};
    const fwVec3d planPt2 = {{PlanP2_X, PlanP2_Y, PlanP2_Z}};
    const fwVec3d planPt3 = {{PlanP3_X, PlanP3_Y, PlanP3_Z}};

    fwPlane plane1 = ::fwMath::getPlane(planPt1, planPt2, planPt3);
    fwVec3d normal = ::fwMath::getNormal(plane1);

    fwPlane plane2 = ::fwMath::getPlane(normal, planPt1);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[0], plane2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[1], plane2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[2], plane2[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[3], plane2[3], 0.00001);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkSetValues()
{
    const double planP1_X = 0.0;
    const double planP1_Y = 0.0;
    const double planP1_Z = 2.5;

    const double planP2_X = 1.0;
    const double planP2_Y = 0.0;
    const double planP2_Z = 0.5;

    const double planP3_X = 1.0;
    const double planP3_Y = 1.0;
    const double planP3_Z = -0.5;

    const fwVec3d planPt1 = {{planP1_X, planP1_Y, planP1_Z}};
    const fwVec3d planPt2 = {{planP2_X, planP2_Y, planP2_Z}};
    const fwVec3d planPt3 = {{planP3_X, planP3_Y, planP3_Z}};

    fwVec3d normal        = {{0.8164965, 0.408248290, 0.408248290}};
    const double distance = 1.02062072;

    fwPlane plane;
    ::fwMath::setValues(plane, planPt1, planPt2, planPt3);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[0], normal[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[1], normal[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[2], normal[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[3], distance, 0.00001);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkDistance()
{
    const double DISTANCE = 10.25;

    fwPlane plane;
    ::fwMath::setDistance(plane, DISTANCE);
    CPPUNIT_ASSERT_EQUAL(::fwMath::getDistance(plane), DISTANCE);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkNormal()
{
    const double planP1_X = 0.0;
    const double planP1_Y = 0.0;
    const double planP1_Z = 2.5;

    const double planP2_X = 1.0;
    const double planP2_Y = 0.0;
    const double planP2_Z = 0.5;

    const double planP3_X = 1.0;
    const double planP3_Y = 1.0;
    const double planP3_Z = -0.5;

    const fwVec3d planPt1 = {{planP1_X, planP1_Y, planP1_Z}};
    const fwVec3d planPt2 = {{planP2_X, planP2_Y, planP2_Z}};
    const fwVec3d planPt3 = {{planP3_X, planP3_Y, planP3_Z}};

    fwVec3d normal  = {{0.8164965, 0.408248290, 0.408248290}};
    fwVec3d normal2 = {{2.0, 1.5, 1.0}};
    ::fwMath::normalize(normal2);

    fwPlane plane;
    ::fwMath::setValues(plane, planPt1, planPt2, planPt3);
    fwVec3d planeNormal = ::fwMath::getNormal(plane);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(planeNormal[0], normal[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(planeNormal[1], normal[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(planeNormal[2], normal[2], 0.00001);

    ::fwMath::setNormal(plane, normal2);
    fwVec3d planeNormal2 = ::fwMath::getNormal(plane);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(planeNormal2[0], normal2[0], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(planeNormal2[1], normal2[1], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(planeNormal2[2], normal2[2], 0.001);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkIntersect()
{
    const bool INTERSECT = true;

    fwVec3d point;
    fwLine line;
    const fwVec3d linePos = {{1.0, 2.0, 4.0}};
//  const fwVec3d lineDirection = {{1.0, 0.0, 0.0}};  // ==> pas d'intercestion
//  const fwVec3d lineDirection = {{3.0, 0.0, 4.0}};  // ==> intercestion
    const fwVec3d lineDirection = {{0.0, 0.0, 4.0}};  // ==> intersection en (0.0, 0.0, 0.0)

    line = std::make_pair(linePos, lineDirection);

    const fwVec3d planPt1 = {{0.0, 0.0,0.0}};
    const fwVec3d planPt2 = {{2.0, 0.0, 0.0}};
    const fwVec3d planPt3 = {{0.0, 2.0, 0.0}};
    fwPlane plane;
    ::fwMath::setValues(plane, planPt1, planPt2, planPt3);
    bool intersect = ::fwMath::intersect( plane, line,  point);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(point[0], 1.0, 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point[1], 2.5, 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point[2], 0.0, 0.001);

    CPPUNIT_ASSERT_EQUAL(intersect, INTERSECT);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkIsInHalfSpace()
{
    const double planP1_X = 1.0;
    const double planP1_Y = 0.0;
    const double planP1_Z = 0.0;

    const double planP2_X = 0.0;
    const double planP2_Y = 0.0;
    const double planP2_Z = 1.0;

    const double planP3_X = 0.0;
    const double planP3_Y = 2.0;
    const double planP3_Z = 1.0;

    const fwVec3d point1  = {{1.0, 0.0, 1.0}};
    const fwVec3d planPt1 = {{planP1_X, planP1_Y, planP1_Z}};
    const fwVec3d planPt2 = {{planP2_X, planP2_Y, planP2_Z}};
    const fwVec3d planPt3 = {{planP3_X, planP3_Y, planP3_Z}};
    fwPlane plane;
    ::fwMath::setValues(plane, planPt1, planPt2, planPt3);
    fwVec3d normal          = ::fwMath::getNormal(plane);
    double distance         = ::fwMath::getDistance(plane);
    const fwPlane planeTest = {{normal[0],normal[1],normal[2],distance}};

    bool result = ::fwMath::isInHalfSpace(planeTest, point1);

    CPPUNIT_ASSERT_EQUAL(false, result);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkOffset()
{
    const double OFFSET = 0.3;

    const double planP1_X = 0.0;
    const double planP1_Y = 0.0;
    const double planP1_Z = 2.5;

    const double planP2_X = 1.0;
    const double planP2_Y = 0.0;
    const double planP2_Z = 0.5;

    const double planP3_X = 1.0;
    const double planP3_Y = 1.0;
    const double planP3_Z = -0.5;

    const fwVec3d planPt1 = {{planP1_X, planP1_Y, planP1_Z}};
    const fwVec3d planPt2 = {{planP2_X, planP2_Y, planP2_Z}};
    const fwVec3d planPt3 = {{planP3_X, planP3_Y, planP3_Z}};
    fwPlane plane;
    ::fwMath::setValues(plane, planPt1, planPt2, planPt3);

    ::fwMath::offset(plane,OFFSET );
    double offset = ::fwMath::getDistance(plane);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.320620, offset, 0.001);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkTransform()
{
    const fwVec3d normalRes  = {{0.832, -0.554, 0.0}};
    const double distanceRes = -0.028691;

    const fwVec3d normal = {{4.0, 3.0, 2.0}};
    const fwVec3d point  = {{0.0, 0.0, 0.5}};

    fwPlane plane = ::fwMath::getPlane(normal, point);

    const fwMatrix4x4 matrice = {{ {{1.0,0.0,1.0,3.0}}, {{-1.0,0.0,0.0,5.0}},
                                   {{0.0,0.0,0.0,2.0}}, {{0.0,0.0,0.0,1.0 }} }};

    ::fwMath::transform(plane, matrice);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[0], normalRes[0], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[1], normalRes[1], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[2], normalRes[2], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[3], distanceRes, 0.001);
}

//------------------------------------------------------------------------------

void PlaneFunctionsTest::checkOperator()
{
    fwPlane plane1 = {{1.0, 0.0, 1.0, 3.0}};
    fwPlane plane2 = plane1;

    bool test = (plane1==plane2);
    CPPUNIT_ASSERT_EQUAL(test, true);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMath

