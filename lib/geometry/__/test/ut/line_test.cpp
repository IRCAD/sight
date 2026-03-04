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

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <geometry/__/line.hpp>

#include <doctest/doctest.h>

#include <glm/glm.hpp>

TEST_SUITE("sight::geometry::line")
{
//------------------------------------------------------------------------------

    TEST_CASE("get_closest_point")
    {
        {
            sight::geometry::ray_t ray = {{0, 0, 0}, {1, 0, 0}};
            glm::dvec3 pt              = {5, 10, 0};

            glm::dvec3 closest_pt = sight::geometry::get_closest_point(ray, pt);

            CHECK(closest_pt[0] == doctest::Approx(5.).epsilon(0.001));
            CHECK(closest_pt[1] == doctest::Approx(0.).epsilon(0.001));
            CHECK(closest_pt[2] == doctest::Approx(0.).epsilon(0.001));
        }
        {
            sight::geometry::ray_t ray = {{2, -3, 1}, {1, 4, -3}};
            ray.second = glm::normalize(ray.second);
            glm::dvec3 pt = {1, 42, 2};

            glm::dvec3 closest_pt;
#ifndef FW_PROFILING_DISABLED
            {
                FW_PROFILE("::sight::geometry::data::getClosestPoint");
                for(int i = 0 ; i < 1000000 ; ++i)
                {
                    closestPt = sight::geometry::data::getClosestPoint(ray, pt);
                }
            }
#else
            closest_pt = sight::geometry::get_closest_point(ray, pt);
#endif
            CHECK(closest_pt[0] == doctest::Approx(8.76923).epsilon(0.001));
            CHECK(closest_pt[1] == doctest::Approx(24.0769).epsilon(0.001));
            CHECK(closest_pt[2] == doctest::Approx(-19.3077).epsilon(0.001));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("get_closest_points")
    {
        glm::dvec3 point_on_this;
        glm::dvec3 point_on_ray;

        // No intersection, parallel rays
        {
            sight::geometry::ray_t ray1 = {{0, 0, 0}, {1, 0, 0}};
            sight::geometry::ray_t ray2 = {{0, 1, 0}, {1, 0, 0}};

            bool b_intersection = sight::geometry::get_closest_points(ray1, ray2, point_on_this, point_on_ray);

            CHECK(b_intersection == false);
        }

        // Intersection
        {
            sight::geometry::ray_t ray1 = {{1, 1, 1}, {1, 0, 0}};
            sight::geometry::ray_t ray2 = {{0, 0, 0}, {1, 1, 1}};

            ray2.second = glm::normalize(ray2.second);

            bool b_intersection = sight::geometry::get_closest_points(ray1, ray2, point_on_this, point_on_ray);

            CHECK(b_intersection == true);

            CHECK(point_on_this[0] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_this[1] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_this[2] == doctest::Approx(1.).epsilon(0.001));

            CHECK(point_on_ray[0] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_ray[1] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_ray[2] == doctest::Approx(1.).epsilon(0.001));
        }

        // No intersection, but rays are not parallel
        {
            sight::geometry::ray_t ray1 = {{1, 1, 1}, {-1, 0, 0}};
            sight::geometry::ray_t ray2 = {{-2, -1, -3}, {-1, -1, -1}};

            ray2.second = glm::normalize(ray2.second);

            bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
            {
                FW_PROFILE("::sight::geometry::data::getClosestPoints");
                for(int i = 0 ; i < 1000000 ; ++i)
                {
                    bIntersection = sight::geometry::data::getClosestPoints(ray1, ray2, pointOnThis, pointOnray);
                }
            }
#else
            b_intersection = sight::geometry::get_closest_points(ray1, ray2, point_on_this, point_on_ray);
#endif

            CHECK(b_intersection == true);

            CHECK(point_on_this[0] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_this[1] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_this[2] == doctest::Approx(1.).epsilon(0.001));

            CHECK(point_on_ray[0] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_ray[1] == doctest::Approx(2.).epsilon(0.001));
            CHECK(point_on_ray[2] == doctest::Approx(0.).epsilon(0.001));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("intersect1")
    {
        {
            sight::geometry::ray_t ray = {{0, 0, 0}, {1, 0, 0}};
            glm::dvec3 pt              = {5, 10, 0};

            bool b_intersection = false;
            b_intersection = sight::geometry::intersect(ray, 11.0, pt);
            CHECK(b_intersection == true);

            b_intersection = sight::geometry::intersect(ray, 10.0, pt);
            CHECK(b_intersection == true);

            b_intersection = sight::geometry::intersect(ray, 9.0, pt);
            CHECK(b_intersection == false);
        }
        {
            sight::geometry::ray_t ray = {{2, -3, 1}, {1, 4, -3}};
            ray.second = glm::normalize(ray.second);
            glm::dvec3 pt = {1, 42, 2};

            bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
            {
                FW_PROFILE("::sight::geometry::data::intersect1");
                for(int i = 0 ; i < 1000000 ; ++i)
                {
                    bIntersection = sight::geometry::data::intersect(ray, 10.0, pt);
                }
            }
#else
            b_intersection = sight::geometry::intersect(ray, 10.0, pt);
#endif
            CHECK(b_intersection == false);

            b_intersection = sight::geometry::intersect(ray, 28.0, pt);
            CHECK(b_intersection == false);

            b_intersection = sight::geometry::intersect(ray, 29.0, pt);
            CHECK(b_intersection == true);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("intersect2")
    {
        // No intersection, parallel rays
        {
            sight::geometry::ray_t ray1 = {{0, 0, 0}, {1, 0, 0}};
            glm::dvec3 origin           = {0, 1, 0};
            glm::dvec3 direction        = {1, 0, 0};
            glm::dvec3 point_on_ray;

            bool b_intersection = sight::geometry::intersect(ray1, 100.0, origin, direction, point_on_ray);

            CHECK(b_intersection == false);
        }

        // Intersection
        {
            sight::geometry::ray_t ray1 = {{1, 1, 1}, {1, 0, 0}};

            glm::dvec3 origin    = {0, 0, 0};
            glm::dvec3 direction = {1, 1, 1};
            glm::dvec3 point_on_ray;

            direction = glm::normalize(direction);

            bool b_intersection = false;
#ifndef FW_PROFILING_DISABLED
            {
                FW_PROFILE("::sight::geometry::data::intersect1");
                for(int i = 0 ; i < 1000000 ; ++i)
                {
                    bIntersection = sight::geometry::data::intersect(ray1, 2.0, origin, direction, pointOnray);
                }
            }
#else
            b_intersection = sight::geometry::intersect(ray1, 2.0, origin, direction, point_on_ray);
#endif

            CHECK(b_intersection == true);

            CHECK(point_on_ray[0] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_ray[1] == doctest::Approx(1.).epsilon(0.001));
            CHECK(point_on_ray[2] == doctest::Approx(1.).epsilon(0.001));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("intersect_box")
    {
        {
            glm::dvec3 center                   = {0, 0, 0};
            glm::dvec3 extent                   = {1, 1, 1};
            glm::dmat3 orientation              = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
            sight::geometry::oriented_box_t box = {.center = center, .extent = extent, .orientation = orientation};

            {
                sight::geometry::line_t segment = {{0, 0, 0}, {10, 0, 0}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == true);
            }
            {
                sight::geometry::line_t segment = {{-2, 0, 0}, {-1.1, 0, 0}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == false);
            }
            {
                sight::geometry::line_t segment = {{-2, -1, -1}, {2, 0, 1}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == true);
            }
            {
                sight::geometry::line_t segment = {{4, 5, 6}, {2, 3, 4}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == false);
            }
        }
        {
            glm::dvec3 center                   = {-10, 4, 8};
            glm::dvec3 extent                   = {1, 2, 3.5}; // Rotated: 3.5, 2, 1.0
            glm::dmat3 orientation              = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0};
            sight::geometry::oriented_box_t box = {.center = center, .extent = extent, .orientation = orientation};

            {
                sight::geometry::line_t segment = {{-6, 6, 12}, {-11, 5.5, 8}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == true);
            }
            {
                sight::geometry::line_t segment = {{-15, 6, 12}, {-13, 3, 9}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == true);
            }
            {
                sight::geometry::line_t segment = {{-15, 6, 12}, {-14, 3, 8}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == false);
            }
            {
                sight::geometry::line_t segment = {{-15, 6, 12}, {-13, 3, 10}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == false);
            }
            {
                sight::geometry::line_t segment = {{-15, 6.1, 12}, {-13, 6.2, 9}};
                bool intersect                  = sight::geometry::intersect_box(segment, box);
                CHECK(intersect == false);
            }
        }
    }

//------------------------------------------------------------------------------
}
