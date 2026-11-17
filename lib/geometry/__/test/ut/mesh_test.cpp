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

#include <geometry/__/mesh.hpp>

#include <doctest/doctest.h>

#include <cmath>

// To avoid using indexes in our vectors (for mathematical clarity).
// x, y , z.
constexpr int X = 0;
constexpr int Y = 1;
constexpr int Z = 2;
// u, v, w.
constexpr int U = X;
constexpr int V = Y;
constexpr int W = Z;
constexpr int H = 3;

const static double EPSILON = 10e-9;

TEST_SUITE("sight::geometry::mesh")
{
//-----------------------------------------------------------------------------

    static void compare(const ::glm::dvec3& _expected, const ::glm::dvec3& _actual)
    {
        CHECK(_expected[X] == doctest::Approx(_actual[X]).epsilon(EPSILON));
        CHECK(_expected[Y] == doctest::Approx(_actual[Y]).epsilon(EPSILON));
        CHECK(_expected[Z] == doctest::Approx(_actual[Z]).epsilon(EPSILON));
    }

//-----------------------------------------------------------------------------

    TEST_CASE("compute_barycenter_abc_2d")
    {
        // With the same Z it correspond to a 2d case

        // Creating triangle points A, B, C.
        const ::glm::dvec3 a {9., 6., 1.};
        const ::glm::dvec3 b {9., 12., 1.};
        const ::glm::dvec3 c {2., 9., 1.};

        // Creating intersection Point P.
        const ::glm::dvec3 p {7., 9., 1.};

        const ::glm::dvec3 barycentric = sight::geometry::to_barycentric_coord(p, a, b, c);

        // Test if sum of barycentric coordinates are equal to 1.

        const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

        CHECK(1. == doctest::Approx(sum).epsilon(EPSILON));

        // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

        CHECK(std::isgreaterequal(barycentric[V], 0.));
        CHECK(std::islessequal(barycentric[V], 1.));
        CHECK(std::isgreaterequal(barycentric[W], 0.));
        CHECK(std::islessequal(barycentric[W], 1.));
        CHECK(std::islessequal(barycentric[V] + barycentric[W], 1.));

        // Convert back to world coordinates.

        const ::glm::dvec3 p2 = sight::geometry::from_barycentric_coord(barycentric, a, b, c);

        compare(p, p2);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("compute_barycenter_abc_3d")
    {
        // Second test in 3d.
        const ::glm::dvec3 a {1., 0., 0.};
        const ::glm::dvec3 b {0., 1., 0.};
        const ::glm::dvec3 c {0., 0., 1.};

        // Should be at the center of ABC
        const ::glm::dvec3 p {1. / 3., 1. / 3., 1. / 3.};

        const ::glm::dvec3 barycentric = sight::geometry::to_barycentric_coord(p, a, b, c);

        // Test if sum of barycentric coordinates are equal to 1.

        const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

        CHECK(1. == doctest::Approx(sum).epsilon(EPSILON));

        // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

        CHECK(std::isgreaterequal(barycentric[V], 0.));
        CHECK(std::islessequal(barycentric[V], 1.));
        CHECK(std::isgreaterequal(barycentric[W], 0.));
        CHECK(std::islessequal(barycentric[W], 1.));
        CHECK(std::islessequal(barycentric[V] + barycentric[W], 1.));

        // Convert back to world coordinates.

        const ::glm::dvec3 p2 = sight::geometry::from_barycentric_coord(barycentric, a, b, c);

        compare(p, p2);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("compute_barycenter_abc_real_coords")
    {
        // Second test in 3d.
        const ::glm::dvec3 a {-0.5, 0., 3.};
        const ::glm::dvec3 b {0.5, -0.7071, 3.7071};
        const ::glm::dvec3 c {-0.5, -0.7071, 3.7071};

        // Should be at the center of ABC
        const ::glm::dvec3 p {-0.39, -0.30, 3.30};

        const ::glm::dvec3 barycentric = sight::geometry::to_barycentric_coord(p, a, b, c);

        // Test if sum of barycentric coordinates are equal to 1.

        const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

        CHECK(1. == doctest::Approx(sum).epsilon(EPSILON));

        // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

        CHECK(std::isgreaterequal(barycentric[V], 0.));
        CHECK(std::islessequal(barycentric[V], 1.));
        CHECK(std::isgreaterequal(barycentric[W], 0.));
        CHECK(std::islessequal(barycentric[W], 1.));
        CHECK(std::islessequal(barycentric[V] + barycentric[W], 1.));

        // Convert back to world coordinates.

        const ::glm::dvec3 p2 = sight::geometry::from_barycentric_coord(barycentric, a, b, c);

        compare(p, p2);
    }

//-----------------------------------------------------------------------------

    TEST_CASE("compute_barycenter_outside_abc")
    {
        // Test with a point outside of the triangle.

        // Second test in 3d.
        const ::glm::dvec3 a {0., 0., 0.};
        const ::glm::dvec3 b {0., 1., 0.};
        const ::glm::dvec3 c {1., 0., 1.};

        const ::glm::dvec3 p {10., 20., 30.};

        const ::glm::dvec3 barycentric = sight::geometry::to_barycentric_coord(p, a, b, c);

        // Test if sum of barycentric coordinates are equal to 1.

        const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

        CHECK(1. == doctest::Approx(sum).epsilon(EPSILON));

        CHECK(barycentric[V] > 1.); // v = 20
        CHECK(barycentric[W] > 1.); //w = 20
    }

//-----------------------------------------------------------------------------

    TEST_CASE("compute_barycenter_tetrahedron")
    {
        // First test in 3d.
        const ::glm::dvec3 a {0., 0., 0.};
        const ::glm::dvec3 b {1., 0., 0.};
        const ::glm::dvec3 c {0., 1., 0.};
        const ::glm::dvec3 d {0., 0., 1.};

        // Should be at the center of ABC
        const ::glm::dvec3 p {0.25, 0.25, 0.25};

        const ::glm::dvec4 barycentric = sight::geometry::to_barycentric_coord(p, a, b, c, d);

        // Test if sum of barycentric coordinates are equal to 1.

        const double sum = (barycentric[U] + barycentric[V] + barycentric[W] + barycentric[H]);

        CHECK(1. == doctest::Approx(sum).epsilon(EPSILON));

        // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

        CHECK(barycentric[V] >= 0.);
        CHECK(barycentric[V] <= 1);
        CHECK(barycentric[W] >= 0.);
        CHECK(barycentric[W] <= 1);
        CHECK(barycentric[H] >= 0.);
        CHECK(barycentric[H] <= 1);
        CHECK((barycentric[V] + barycentric[W] + barycentric[H]) <= 1.);

        // Convert back to world coordinates.

        const ::glm::dvec3 p2 = sight::geometry::from_barycentric_coord(barycentric, a, b, c, d);

        compare(p, p2);

        CHECK(
            sight::geometry::is_inside_tetrahedron(
                p,
                a,
                b,
                c,
                d
            ) == sight::geometry::is_inside_tetrahedron(barycentric)
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("compute_barycenter_outside_tetrahedron")
    {
        // Test with a point outside of the tetrahedron.

        // Second test in 3d.
        const ::glm::dvec3 a {0., 0., 0.};
        const ::glm::dvec3 b {1., 0., 0.};
        const ::glm::dvec3 c {0., 1., 0.};
        const ::glm::dvec3 d {0., 0., 1.};

        const ::glm::dvec3 p_in {0.25, 0.25, 0.25};
        const ::glm::dvec3 p_out {1., 2., 3.};
        const ::glm::dvec3 p_edge {0.5, 0., 0.};
        const ::glm::dvec3 p_vertex {0., 0., 0.};

        const bool is_inside_p_in     = sight::geometry::is_inside_tetrahedron(p_in, a, b, c, d);
        const bool is_inside_p_out    = sight::geometry::is_inside_tetrahedron(p_out, a, b, c, d);
        const bool is_inside_p_edge   = sight::geometry::is_inside_tetrahedron(p_edge, a, b, c, d);
        const bool is_inside_p_vertex = sight::geometry::is_inside_tetrahedron(p_vertex, a, b, c, d);

        CHECK(is_inside_p_in);
        CHECK(!is_inside_p_out);
        CHECK(is_inside_p_edge);
        CHECK(is_inside_p_vertex);
    }
}
