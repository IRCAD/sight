/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "line_functions_test.hpp"

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <geometry/data/line_functions.hpp>
#include <geometry/data/vector_functions.hpp>

#include <glm/glm.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::line_functions_test);

namespace sight::geometry::data::ut
{

//------------------------------------------------------------------------------

void line_functions_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void line_functions_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void line_functions_test::checkGetClosestPoint()
{
    {
        fwLine line = {{{0, 0, 0}}, {{1, 0, 0}}};
        fwVec3d pt  = {{5, 10, 0}};

        fwVec3d closest_pt = geometry::data::get_closest_point(line, pt);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(5., closest_pt[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., closest_pt[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., closest_pt[2], 0.001);
    }
    {
        fwLine line = {{{2, -3, 1}}, {{1, 4, -3}}};
        geometry::data::normalize(line.second);
        fwVec3d pt = {{1, 42, 2}};

        fwVec3d closest_pt;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::getClosestPoint");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                closestPt = geometry::data::getClosestPoint(line, pt);
            }
        }
#else
        closest_pt = geometry::data::get_closest_point(line, pt);
#endif
        CPPUNIT_ASSERT_DOUBLES_EQUAL(8.76923, closest_pt[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(24.0769, closest_pt[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-19.3077, closest_pt[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void line_functions_test::checkGetClosestPoints()
{
    fwVec3d point_on_this;
    fwVec3d point_on_line;

    // No intersection, parallel lines
    {
        fwLine line1 = {{{0, 0, 0}}, {{1, 0, 0}}};
        fwLine line2 = {{{0, 1, 0}}, {{1, 0, 0}}};

        bool b_intersection = geometry::data::get_closest_points(line1, line2, point_on_this, point_on_line);

        CPPUNIT_ASSERT(b_intersection == false);
    }

    // Intersection
    {
        fwLine line1 = {{{1, 1, 1}}, {{1, 0, 0}}};
        fwLine line2 = {{{0, 0, 0}}, {{1, 1, 1}}};

        geometry::data::normalize(line2.second);

        bool b_intersection = geometry::data::get_closest_points(line1, line2, point_on_this, point_on_line);

        CPPUNIT_ASSERT(b_intersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[2], 0.001);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_line[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_line[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_line[2], 0.001);
    }

    // No intersection, but lines are not parallel
    {
        fwLine line1 = {{{1, 1, 1}}, {{-1, 0, 0}}};
        fwLine line2 = {{{-2, -1, -3}}, {{-1, -1, -1}}};

        geometry::data::normalize(line2.second);

        bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::getClosestPoints");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                bIntersection = geometry::data::getClosestPoints(line1, line2, pointOnThis, pointOnLine);
            }
        }
#else
        b_intersection = geometry::data::get_closest_points(line1, line2, point_on_this, point_on_line);
#endif

        CPPUNIT_ASSERT(b_intersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[2], 0.001);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_line[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(2., point_on_line[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., point_on_line[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void line_functions_test::checkIntersect1()
{
    {
        fwLine line = {{{0, 0, 0}}, {{1, 0, 0}}};
        fwVec3d pt  = {{5, 10, 0}};

        bool b_intersection = false;
        b_intersection = geometry::data::intersect(line, 11.0, pt);
        CPPUNIT_ASSERT(b_intersection == true);

        b_intersection = geometry::data::intersect(line, 10.0, pt);
        CPPUNIT_ASSERT(b_intersection == true);

        b_intersection = geometry::data::intersect(line, 9.0, pt);
        CPPUNIT_ASSERT(b_intersection == false);
    }
    {
        fwLine line = {{{2, -3, 1}}, {{1, 4, -3}}};
        geometry::data::normalize(line.second);
        fwVec3d pt = {{1, 42, 2}};

        bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::intersect1");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                bIntersection = geometry::data::intersect(line, 10.0, pt);
            }
        }
#else
        b_intersection = geometry::data::intersect(line, 10.0, pt);
#endif
        CPPUNIT_ASSERT(b_intersection == false);

        b_intersection = geometry::data::intersect(line, 28.0, pt);
        CPPUNIT_ASSERT(b_intersection == false);

        b_intersection = geometry::data::intersect(line, 29.0, pt);
        CPPUNIT_ASSERT(b_intersection == true);
    }
}

//------------------------------------------------------------------------------

void line_functions_test::checkIntersect2()
{
    // No intersection, parallel lines
    {
        fwLine line1      = {{{0, 0, 0}}, {{1, 0, 0}}};
        fwVec3d origin    = {{0, 1, 0}};
        fwVec3d direction = {{1, 0, 0}};
        fwVec3d point_on_line;

        bool b_intersection = geometry::data::intersect(line1, 100.0, origin, direction, point_on_line);

        CPPUNIT_ASSERT(b_intersection == false);
    }

    // Intersection
    {
        fwLine line1 = {{{1, 1, 1}}, {{1, 0, 0}}};

        fwVec3d origin    = {{0, 0, 0}};
        fwVec3d direction = {{1, 1, 1}};
        fwVec3d point_on_line;

        geometry::data::normalize(direction);

        bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::intersect1");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                bIntersection = geometry::data::intersect(line1, 2.0, origin, direction, pointOnLine);
            }
        }
#else
        b_intersection = geometry::data::intersect(line1, 2.0, origin, direction, point_on_line);
#endif

        CPPUNIT_ASSERT(b_intersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_line[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_line[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_line[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void line_functions_test::checkIntersect3()
{
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data::ut
