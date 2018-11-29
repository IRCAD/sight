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

#include "LineFunctionsTest.hpp"

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwMath/LineFunctions.hpp>
#include <fwMath/VectorFunctions.hpp>

#include <glm/glm.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMath::ut::LineFunctionsTest );

namespace fwMath
{
namespace ut
{

void LineFunctionsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void LineFunctionsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void LineFunctionsTest::checkGetClosestPoint()
{
    {
        fwLine line = { {{0, 0, 0}}, {{1, 0, 0}} };
        fwVec3d pt  = {{5, 10, 0}};

        fwVec3d closestPt = ::fwMath::getClosestPoint(line, pt);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(5., closestPt[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., closestPt[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., closestPt[2], 0.001);
    }
    {
        fwLine line = { {{2, -3, 1}}, {{1, 4, -3}} };
        ::fwMath::normalize(line.second);
        fwVec3d pt = {{1, 42, 2}};

        fwVec3d closestPt;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::fwMath::getClosestPoint");
            for(int i = 0; i < 1000000; ++i)
            {
                closestPt = ::fwMath::getClosestPoint(line, pt);
            }
        }
#else
        closestPt = ::fwMath::getClosestPoint(line, pt);
#endif
        CPPUNIT_ASSERT_DOUBLES_EQUAL(8.76923, closestPt[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(24.0769, closestPt[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-19.3077, closestPt[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void LineFunctionsTest::checkGetClosestPoints()
{
    fwVec3d pointOnThis;
    fwVec3d pointOnLine;

    // No intersection, parallel lines
    {
        fwLine line1 = { {{0, 0, 0}}, {{1, 0, 0}} };
        fwLine line2 = { {{0, 1, 0}}, {{1, 0, 0}} };

        bool bIntersection = ::fwMath::getClosestPoints(line1, line2, pointOnThis, pointOnLine);

        CPPUNIT_ASSERT(bIntersection == false);
    }

    // Intersection
    {
        fwLine line1 = { {{1, 1, 1}}, {{1, 0, 0}} };
        fwLine line2 = { {{0, 0, 0}}, {{1, 1, 1}} };

        ::fwMath::normalize(line2.second);

        bool bIntersection = ::fwMath::getClosestPoints(line1, line2, pointOnThis, pointOnLine);

        CPPUNIT_ASSERT(bIntersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnThis[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnThis[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnThis[2], 0.001);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnLine[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnLine[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnLine[2], 0.001);
    }

    // No intersection, but lines are not parallel
    {
        fwLine line1 = { {{1, 1, 1}}, {{-1, 0, 0}} };
        fwLine line2 = { {{-2, -1, -3}}, {{-1, -1, -1}} };

        ::fwMath::normalize(line2.second);

        bool bIntersection;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::fwMath::getClosestPoints");
            for(int i = 0; i < 1000000; ++i)
            {
                bIntersection = ::fwMath::getClosestPoints(line1, line2, pointOnThis, pointOnLine);
            }
        }
#else
        bIntersection = ::fwMath::getClosestPoints(line1, line2, pointOnThis, pointOnLine);
#endif

        CPPUNIT_ASSERT(bIntersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnThis[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnThis[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnThis[2], 0.001);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnLine[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(2., pointOnLine[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., pointOnLine[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void LineFunctionsTest::checkIntersect1()
{
    {
        fwLine line = { {{0, 0, 0}}, {{1, 0, 0}} };
        fwVec3d pt  = {{5, 10, 0}};

        bool bIntersection;
        bIntersection = ::fwMath::intersect(line, 11.0, pt);
        CPPUNIT_ASSERT(bIntersection == true);

        bIntersection = ::fwMath::intersect(line, 10.0, pt);
        CPPUNIT_ASSERT(bIntersection == true);

        bIntersection = ::fwMath::intersect(line, 9.0, pt);
        CPPUNIT_ASSERT(bIntersection == false);
    }
    {
        fwLine line = { {{2, -3, 1}}, {{1, 4, -3}} };
        ::fwMath::normalize(line.second);
        fwVec3d pt = {{1, 42, 2}};

        bool bIntersection;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::fwMath::intersect1");
            for(int i = 0; i < 1000000; ++i)
            {
                bIntersection = ::fwMath::intersect(line, 10.0, pt);
            }
        }
#else
        bIntersection = ::fwMath::intersect(line, 10.0, pt);
#endif
        CPPUNIT_ASSERT(bIntersection == false);

        bIntersection = ::fwMath::intersect(line, 28.0, pt);
        CPPUNIT_ASSERT(bIntersection == false);

        bIntersection = ::fwMath::intersect(line, 29.0, pt);
        CPPUNIT_ASSERT(bIntersection == true);
    }
}

//------------------------------------------------------------------------------

void LineFunctionsTest::checkIntersect2()
{
    // No intersection, parallel lines
    {
        fwLine line1      = { {{0, 0, 0}}, {{1, 0, 0}} };
        fwVec3d origin    = {{0, 1, 0}};
        fwVec3d direction = {{1, 0, 0}};
        fwVec3d pointOnLine;

        bool bIntersection = ::fwMath::intersect(line1, 100.0, origin, direction, pointOnLine);

        CPPUNIT_ASSERT(bIntersection == false);
    }

    // Intersection
    {
        fwLine line1 = { {{1, 1, 1}}, {{1, 0, 0}} };

        fwVec3d origin    = {{0, 0, 0}};
        fwVec3d direction = {{1, 1, 1}};
        fwVec3d pointOnLine;

        ::fwMath::normalize(direction);

        bool bIntersection;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::fwMath::intersect1");
            for(int i = 0; i < 1000000; ++i)
            {
                bIntersection = ::fwMath::intersect(line1, 2.0, origin, direction, pointOnLine);
            }
        }
#else
        bIntersection = ::fwMath::intersect(line1, 2.0, origin, direction, pointOnLine);
#endif

        CPPUNIT_ASSERT(bIntersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnLine[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnLine[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., pointOnLine[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void LineFunctionsTest::checkIntersect3()
{

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMath
