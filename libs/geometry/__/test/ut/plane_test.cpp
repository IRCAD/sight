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

#include "plane_test.hpp"

#include <geometry/__/plane.hpp>

#include <glm/glm.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::plane_test);

namespace sight::geometry::data::ut
{

//------------------------------------------------------------------------------

void plane_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void plane_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void plane_test::check_get_plane()
{
    const glm::dvec3 plan_pt1 = {1.0, 0.0, 0.0};
    const glm::dvec3 plan_pt2 = {0.0, 0.0, 1.0};
    const glm::dvec3 plan_pt3 = {0.0, 2.0, 1.0};

    plane_t plane1    = geometry::get_plane(plan_pt1, plan_pt2, plan_pt3);
    glm::dvec3 normal = geometry::get_normal(plane1);

    plane_t plane2 = geometry::get_plane(normal, plan_pt1);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[0], plane2[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[1], plane2[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[2], plane2[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane1[3], plane2[3], 0.00001);
}

//------------------------------------------------------------------------------

void plane_test::check_set_plane()
{
    const double plan_p1_x = 0.0;
    const double plan_p1_y = 0.0;
    const double plan_p1_z = 2.5;

    const double plan_p2_x = 1.0;
    const double plan_p2_y = 0.0;
    const double plan_p2_z = 0.5;

    const double plan_p3_x = 1.0;
    const double plan_p3_y = 1.0;
    const double plan_p3_z = -0.5;

    const glm::dvec3 plan_pt1 = {plan_p1_x, plan_p1_y, plan_p1_z};
    const glm::dvec3 plan_pt2 = {plan_p2_x, plan_p2_y, plan_p2_z};
    const glm::dvec3 plan_pt3 = {plan_p3_x, plan_p3_y, plan_p3_z};

    glm::dvec3 normal     = {0.8164965, 0.408248290, 0.408248290};
    const double distance = 1.02062072;

    plane_t plane;
    geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[0], normal[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[1], normal[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[2], normal[2], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[3], distance, 0.00001);
}

//------------------------------------------------------------------------------

void plane_test::check_distance()
{
    const double distance = 10.25;

    plane_t plane;
    geometry::set_distance(plane, distance);
    CPPUNIT_ASSERT_EQUAL(geometry::get_distance(plane), distance);
}

//------------------------------------------------------------------------------

void plane_test::check_normal()
{
    const double plan_p1_x = 0.0;
    const double plan_p1_y = 0.0;
    const double plan_p1_z = 2.5;

    const double plan_p2_x = 1.0;
    const double plan_p2_y = 0.0;
    const double plan_p2_z = 0.5;

    const double plan_p3_x = 1.0;
    const double plan_p3_y = 1.0;
    const double plan_p3_z = -0.5;

    const glm::dvec3 plan_pt1 = {plan_p1_x, plan_p1_y, plan_p1_z};
    const glm::dvec3 plan_pt2 = {plan_p2_x, plan_p2_y, plan_p2_z};
    const glm::dvec3 plan_pt3 = {plan_p3_x, plan_p3_y, plan_p3_z};

    glm::dvec3 normal  = {0.8164965, 0.408248290, 0.408248290};
    glm::dvec3 normal2 = {2.0, 1.5, 1.0};
    normal2 = glm::normalize(normal2);

    plane_t plane;
    geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);
    glm::dvec3 plane_normal = geometry::get_normal(plane);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane_normal[0], normal[0], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane_normal[1], normal[1], 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane_normal[2], normal[2], 0.00001);

    geometry::set_normal(plane, normal2);
    glm::dvec3 plane_normal2 = geometry::get_normal(plane);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane_normal2[0], normal2[0], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane_normal2[1], normal2[1], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane_normal2[2], normal2[2], 0.001);
}

//------------------------------------------------------------------------------

void plane_test::check_intersect()
{
    const glm::dvec3 line_pos = {1.0, 2.0, 4.0};
//  const glm::dvec3 lineDirection = {{1.0, 0.0, 0.0}};  // ==> No intersection
//   const glm::dvec3 line_direction = {3.0, 0.0, 4.0};  // ==> intersection
    const glm::dvec3 line_direction = {0.0, 0.0, 4.0}; // ==> intersection in (0.0, 0.0, 0.0)

    const line_t line = std::make_pair(line_pos, line_pos + line_direction);

    const glm::dvec3 plan_pt1 = {0.0, 0.0, 0.0};
    const glm::dvec3 plan_pt2 = {2.0, 0.0, 0.0};
    const glm::dvec3 plan_pt3 = {0.0, 2.0, 0.0};
    plane_t plane;
    geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);

    glm::dvec3 point;
    const bool intersect = geometry::intersect(plane, line, point);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(point[0], 1.0, 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point[1], 2.0, 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point[2], 0.0, 0.001);

    CPPUNIT_ASSERT_EQUAL(true, intersect);
}

//------------------------------------------------------------------------------

void plane_test::check_is_in_half_space()
{
    const double plan_p1_x = 1.0;
    const double plan_p1_y = 0.0;
    const double plan_p1_z = 0.0;

    const double plan_p2_x = 0.0;
    const double plan_p2_y = 0.0;
    const double plan_p2_z = 1.0;

    const double plan_p3_x = 0.0;
    const double plan_p3_y = 2.0;
    const double plan_p3_z = 1.0;

    const glm::dvec3 point1   = {1.0, 0.0, 1.0};
    const glm::dvec3 plan_pt1 = {plan_p1_x, plan_p1_y, plan_p1_z};
    const glm::dvec3 plan_pt2 = {plan_p2_x, plan_p2_y, plan_p2_z};
    const glm::dvec3 plan_pt3 = {plan_p3_x, plan_p3_y, plan_p3_z};
    plane_t plane;
    geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);
    glm::dvec3 normal        = geometry::get_normal(plane);
    double distance          = geometry::get_distance(plane);
    const plane_t plane_test = {normal[0], normal[1], normal[2], distance};

    bool result = geometry::is_in_half_space(plane_test, point1);

    CPPUNIT_ASSERT_EQUAL(false, result);
}

//------------------------------------------------------------------------------

void plane_test::check_offset()
{
    static constexpr double s_OFFSET = 0.3;

    const double plan_p1_x = 0.0;
    const double plan_p1_y = 0.0;
    const double plan_p1_z = 2.5;

    const double plan_p2_x = 1.0;
    const double plan_p2_y = 0.0;
    const double plan_p2_z = 0.5;

    const double plan_p3_x = 1.0;
    const double plan_p3_y = 1.0;
    const double plan_p3_z = -0.5;

    const glm::dvec3 plan_pt1 = {plan_p1_x, plan_p1_y, plan_p1_z};
    const glm::dvec3 plan_pt2 = {plan_p2_x, plan_p2_y, plan_p2_z};
    const glm::dvec3 plan_pt3 = {plan_p3_x, plan_p3_y, plan_p3_z};
    plane_t plane;
    geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);

    geometry::offset(plane, s_OFFSET);
    double offset = geometry::get_distance(plane);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.320620, offset, 0.001);
}

//------------------------------------------------------------------------------

void plane_test::check_transform()
{
    const glm::dvec3 normal_res = {0.832, -0.554, 0.0};
    const double distance_res   = -0.028691;

    const glm::dvec3 normal = {4.0, 3.0, 2.0};
    const glm::dvec3 point  = {0.0, 0.0, 0.5};

    plane_t plane = geometry::get_plane(normal, point);

    const glm::dmat4 matrice =
    {
        1.0, 0.0, 1.0, 3.0,
        -1.0, 0.0, 0.0, 5.0,
        0.0, 0.0, 0.0, 2.0,
        0.0, 0.0, 0.0, 1.0
    };

    geometry::transform(plane, matrice);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[0], normal_res[0], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[1], normal_res[1], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[2], normal_res[2], 0.001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(plane[3], distance_res, 0.001);
}

//------------------------------------------------------------------------------

void plane_test::check_operator()
{
    plane_t plane1 = {1.0, 0.0, 1.0, 3.0};
    plane_t plane2 = plane1;

    bool test = (plane1 == plane2);
    CPPUNIT_ASSERT_EQUAL(test, true);
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data::ut
