/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <geometry/__/plane.hpp>

#include <doctest/doctest.h>

#include <glm/glm.hpp>

TEST_SUITE("sight::geometry::plane")
{
    static const double EPSILON = 1e-5;

//------------------------------------------------------------------------------

    TEST_CASE("get_plane")
    {
        const glm::dvec3 plan_pt1 = {1.0, 0.0, 0.0};
        const glm::dvec3 plan_pt2 = {0.0, 0.0, 1.0};
        const glm::dvec3 plan_pt3 = {0.0, 2.0, 1.0};

        sight::geometry::plane_t plane1 = sight::geometry::get_plane(plan_pt1, plan_pt2, plan_pt3);
        glm::dvec3 normal               = sight::geometry::get_normal(plane1);

        sight::geometry::plane_t plane2 = sight::geometry::get_plane(normal, plan_pt1);

        CHECK(plane1[0] == doctest::Approx(plane2[0]).epsilon(std::numeric_limits<double>::epsilon()));
        CHECK(plane1[1] == doctest::Approx(plane2[1]).epsilon(std::numeric_limits<double>::epsilon()));
        CHECK(plane1[2] == doctest::Approx(plane2[2]).epsilon(std::numeric_limits<double>::epsilon()));
        CHECK(plane1[3] == doctest::Approx(plane2[3]).epsilon(std::numeric_limits<double>::epsilon()));
    }

//------------------------------------------------------------------------------

    TEST_CASE("set_plane")
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

        sight::geometry::plane_t plane;
        sight::geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);

        CHECK(plane[0] == doctest::Approx(normal[0]).epsilon(EPSILON));
        CHECK(plane[1] == doctest::Approx(normal[1]).epsilon(EPSILON));
        CHECK(plane[2] == doctest::Approx(normal[2]).epsilon(EPSILON));
        CHECK(plane[3] == doctest::Approx(distance).epsilon(EPSILON));
    }

//------------------------------------------------------------------------------

    TEST_CASE("distance")
    {
        const double distance = 10.25;

        sight::geometry::plane_t plane;
        sight::geometry::set_distance(plane, distance);
        CHECK_EQ(sight::geometry::get_distance(plane), distance);
    }

//------------------------------------------------------------------------------

    TEST_CASE("normal")
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

        sight::geometry::plane_t plane;
        sight::geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);
        glm::dvec3 plane_normal = sight::geometry::get_normal(plane);

        CHECK(plane_normal[0] == doctest::Approx(normal[0]).epsilon(EPSILON));
        CHECK(plane_normal[1] == doctest::Approx(normal[1]).epsilon(EPSILON));
        CHECK(plane_normal[2] == doctest::Approx(normal[2]).epsilon(EPSILON));

        sight::geometry::set_normal(plane, normal2);
        glm::dvec3 plane_normal2 = sight::geometry::get_normal(plane);

        CHECK(plane_normal2[0] == doctest::Approx(normal2[0]).epsilon(EPSILON));
        CHECK(plane_normal2[1] == doctest::Approx(normal2[1]).epsilon(EPSILON));
        CHECK(plane_normal2[2] == doctest::Approx(normal2[2]).epsilon(EPSILON));
    }

//------------------------------------------------------------------------------

    TEST_CASE("intersect")
    {
        const glm::dvec3 plan_pt1 = {0.0, 0.0, 0.0};
        const glm::dvec3 plan_pt2 = {2.0, 0.0, 0.0};
        const glm::dvec3 plan_pt3 = {0.0, 2.0, 0.0};
        sight::geometry::plane_t plane;
        sight::geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);

        const glm::dvec3 line_pos = {1.0, 2.0, 4.0};
        {
            const glm::dvec3 line_direction    = {0.0, 0.0, 4.0}; // ==> intersection in (0.0, 0.0, 0.0)
            const sight::geometry::line_t line = {line_pos, line_pos + line_direction};

            const auto intersect = sight::geometry::intersect(plane, line);

            CHECK(intersect.has_value());
            CHECK(intersect.value()[0] == doctest::Approx(1.0).epsilon(std::numeric_limits<double>::epsilon()));
            CHECK(intersect.value()[1] == doctest::Approx(2.0).epsilon(std::numeric_limits<double>::epsilon()));
            CHECK(intersect.value()[2] == doctest::Approx(0.0).epsilon(std::numeric_limits<double>::epsilon()));
        }
        {
            const glm::dvec3 line_direction    = {12.0, 0.0, 0.0}; // ==> intersection in (0.0, 0.0, 0.0)
            const sight::geometry::line_t line = {line_pos, line_pos + line_direction};

            const auto intersect = sight::geometry::intersect(plane, line);
            CHECK(not intersect.has_value());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("intersect_ray")
    {
        const glm::dvec3 plan_pt1 = {0.0, 0.0, 0.0};
        const glm::dvec3 plan_pt2 = {2.0, 0.0, 0.0};
        const glm::dvec3 plan_pt3 = {0.0, 2.0, 0.0};
        sight::geometry::plane_t plane;
        sight::geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);

        const glm::dvec3 line_pos = {1.0, 2.0, 4.0};
        {
            const glm::dvec3 line_direction   = {0.0, 0.0, 1.0};
            const sight::geometry::ray_t line = {line_pos, line_direction};

            const auto intersect = sight::geometry::intersect_ray(plane, line);

            CHECK(intersect.has_value());
            CHECK(intersect.value()[0] == doctest::Approx(1.0).epsilon(std::numeric_limits<double>::epsilon()));
            CHECK(intersect.value()[1] == doctest::Approx(2.0).epsilon(std::numeric_limits<double>::epsilon()));
            CHECK(intersect.value()[2] == doctest::Approx(0.0).epsilon(std::numeric_limits<double>::epsilon()));
        }
        {
            const glm::dvec3 line_direction   = {1.0, 0.0, 0.0};
            const sight::geometry::ray_t line = {line_pos, line_direction};

            const auto intersect = sight::geometry::intersect_ray(plane, line);
            CHECK(not intersect.has_value());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("is_in_half_space")
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
        sight::geometry::plane_t plane;
        sight::geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);
        glm::dvec3 normal                         = sight::geometry::get_normal(plane);
        double distance                           = sight::geometry::get_distance(plane);
        const sight::geometry::plane_t plane_test = {normal[0], normal[1], normal[2], distance};

        bool result = sight::geometry::is_in_half_space(plane_test, point1);

        CHECK_EQ(false, result);
    }

//------------------------------------------------------------------------------

    TEST_CASE("offset")
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
        sight::geometry::plane_t plane;
        sight::geometry::set_plane(plane, plan_pt1, plan_pt2, plan_pt3);

        sight::geometry::offset(plane, s_OFFSET);
        double offset = sight::geometry::get_distance(plane);

        CHECK(1.320620 == doctest::Approx(offset).epsilon(EPSILON));
    }

//------------------------------------------------------------------------------

    TEST_CASE("transform")
    {
        const glm::dvec3 normal_res = {0.83205, -0.55470, 0.0};
        const double distance_res   = -0.028691;

        const glm::dvec3 normal = {4.0, 3.0, 2.0};
        const glm::dvec3 point  = {0.0, 0.0, 0.5};

        sight::geometry::plane_t plane = sight::geometry::get_plane(normal, point);

        const glm::dmat4 matrice =
        {
            1.0, 0.0, 1.0, 3.0,
            -1.0, 0.0, 0.0, 5.0,
            0.0, 0.0, 0.0, 2.0,
            0.0, 0.0, 0.0, 1.0
        };

        sight::geometry::transform(plane, matrice);

        CHECK(plane[0] == doctest::Approx(normal_res[0]).epsilon(EPSILON));
        CHECK(plane[1] == doctest::Approx(normal_res[1]).epsilon(EPSILON));
        CHECK(plane[2] == doctest::Approx(normal_res[2]).epsilon(EPSILON));
        CHECK(plane[3] == doctest::Approx(distance_res).epsilon(EPSILON));
    }

//------------------------------------------------------------------------------

    TEST_CASE("operator")
    {
        sight::geometry::plane_t plane1 = {1.0, 0.0, 1.0, 3.0};
        sight::geometry::plane_t plane2 = plane1;

        bool test = (plane1 == plane2);
        CHECK_EQ(test, true);
    }

//------------------------------------------------------------------------------
}
