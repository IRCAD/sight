/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "line_test.hpp"

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <geometry/__/line.hpp>

#include <glm/glm.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::ut::line_test);

namespace sight::geometry::ut
{

//------------------------------------------------------------------------------

void line_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void line_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void line_test::get_closest_point()
{
    {
        ray_t ray     = {{0, 0, 0}, {1, 0, 0}};
        glm::dvec3 pt = {5, 10, 0};

        glm::dvec3 closest_pt = geometry::get_closest_point(ray, pt);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(5., closest_pt[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., closest_pt[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., closest_pt[2], 0.001);
    }
    {
        ray_t ray = {{2, -3, 1}, {1, 4, -3}};
        ray.second = glm::normalize(ray.second);
        glm::dvec3 pt = {1, 42, 2};

        glm::dvec3 closest_pt;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::getClosestPoint");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                closestPt = geometry::data::getClosestPoint(ray, pt);
            }
        }
#else
        closest_pt = geometry::get_closest_point(ray, pt);
#endif
        CPPUNIT_ASSERT_DOUBLES_EQUAL(8.76923, closest_pt[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(24.0769, closest_pt[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(-19.3077, closest_pt[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void line_test::get_closest_points()
{
    glm::dvec3 point_on_this;
    glm::dvec3 point_on_ray;

    // No intersection, parallel rays
    {
        ray_t ray1 = {{0, 0, 0}, {1, 0, 0}};
        ray_t ray2 = {{0, 1, 0}, {1, 0, 0}};

        bool b_intersection = geometry::get_closest_points(ray1, ray2, point_on_this, point_on_ray);

        CPPUNIT_ASSERT(b_intersection == false);
    }

    // Intersection
    {
        ray_t ray1 = {{1, 1, 1}, {1, 0, 0}};
        ray_t ray2 = {{0, 0, 0}, {1, 1, 1}};

        ray2.second = glm::normalize(ray2.second);

        bool b_intersection = geometry::get_closest_points(ray1, ray2, point_on_this, point_on_ray);

        CPPUNIT_ASSERT(b_intersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[2], 0.001);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_ray[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_ray[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_ray[2], 0.001);
    }

    // No intersection, but rays are not parallel
    {
        ray_t ray1 = {{1, 1, 1}, {-1, 0, 0}};
        ray_t ray2 = {{-2, -1, -3}, {-1, -1, -1}};

        ray2.second = glm::normalize(ray2.second);

        bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::getClosestPoints");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                bIntersection = geometry::data::getClosestPoints(ray1, ray2, pointOnThis, pointOnray);
            }
        }
#else
        b_intersection = geometry::get_closest_points(ray1, ray2, point_on_this, point_on_ray);
#endif

        CPPUNIT_ASSERT(b_intersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_this[2], 0.001);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_ray[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(2., point_on_ray[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., point_on_ray[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void line_test::intersect1()
{
    {
        ray_t ray     = {{0, 0, 0}, {1, 0, 0}};
        glm::dvec3 pt = {5, 10, 0};

        bool b_intersection = false;
        b_intersection = geometry::intersect(ray, 11.0, pt);
        CPPUNIT_ASSERT(b_intersection == true);

        b_intersection = geometry::intersect(ray, 10.0, pt);
        CPPUNIT_ASSERT(b_intersection == true);

        b_intersection = geometry::intersect(ray, 9.0, pt);
        CPPUNIT_ASSERT(b_intersection == false);
    }
    {
        ray_t ray = {{2, -3, 1}, {1, 4, -3}};
        ray.second = glm::normalize(ray.second);
        glm::dvec3 pt = {1, 42, 2};

        bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::intersect1");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                bIntersection = geometry::data::intersect(ray, 10.0, pt);
            }
        }
#else
        b_intersection = geometry::intersect(ray, 10.0, pt);
#endif
        CPPUNIT_ASSERT(b_intersection == false);

        b_intersection = geometry::intersect(ray, 28.0, pt);
        CPPUNIT_ASSERT(b_intersection == false);

        b_intersection = geometry::intersect(ray, 29.0, pt);
        CPPUNIT_ASSERT(b_intersection == true);
    }
}

//------------------------------------------------------------------------------

void line_test::intersect2()
{
    // No intersection, parallel rays
    {
        ray_t ray1           = {{0, 0, 0}, {1, 0, 0}};
        glm::dvec3 origin    = {0, 1, 0};
        glm::dvec3 direction = {1, 0, 0};
        glm::dvec3 point_on_ray;

        bool b_intersection = geometry::intersect(ray1, 100.0, origin, direction, point_on_ray);

        CPPUNIT_ASSERT(b_intersection == false);
    }

    // Intersection
    {
        ray_t ray1 = {{1, 1, 1}, {1, 0, 0}};

        glm::dvec3 origin    = {0, 0, 0};
        glm::dvec3 direction = {1, 1, 1};
        glm::dvec3 point_on_ray;

        direction = glm::normalize(direction);

        bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
        {
            FW_PROFILE("::geometry::data::intersect1");
            for(int i = 0 ; i < 1000000 ; ++i)
            {
                bIntersection = geometry::data::intersect(ray1, 2.0, origin, direction, pointOnray);
            }
        }
#else
        b_intersection = geometry::intersect(ray1, 2.0, origin, direction, point_on_ray);
#endif

        CPPUNIT_ASSERT(b_intersection == true);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_ray[0], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_ray[1], 0.001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., point_on_ray[2], 0.001);
    }
}

//------------------------------------------------------------------------------

void line_test::intersect_box()
{
    {
        glm::dvec3 center            = {0, 0, 0};
        glm::dvec3 extent            = {1, 1, 1};
        glm::dmat3 orientation       = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
        geometry::oriented_box_t box = {.center = center, .extent = extent, .orientation = orientation};

        {
            geometry::line_t segment = {{0, 0, 0}, {10, 0, 0}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == true);
        }
        {
            geometry::line_t segment = {{-2, 0, 0}, {-1.1, 0, 0}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == false);
        }
        {
            geometry::line_t segment = {{-2, -1, -1}, {2, 0, 1}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == true);
        }
        {
            geometry::line_t segment = {{4, 5, 6}, {2, 3, 4}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == false);
        }
    }
    {
        glm::dvec3 center            = {-10, 4, 8};
        glm::dvec3 extent            = {1, 2, 3.5}; // Rotated: 3.5, 2, 1.0
        glm::dmat3 orientation       = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0};
        geometry::oriented_box_t box = {.center = center, .extent = extent, .orientation = orientation};

        {
            geometry::line_t segment = {{-6, 6, 12}, {-11, 5.5, 8}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == true);
        }
        {
            geometry::line_t segment = {{-15, 6, 12}, {-13, 3, 9}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == true);
        }
        {
            geometry::line_t segment = {{-15, 6, 12}, {-14, 3, 8}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == false);
        }
        {
            geometry::line_t segment = {{-15, 6, 12}, {-13, 3, 10}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == false);
        }
        {
            geometry::line_t segment = {{-15, 6.1, 12}, {-13, 6.2, 9}};
            bool intersect           = geometry::intersect_box(segment, box);
            CPPUNIT_ASSERT(intersect == false);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::ut
