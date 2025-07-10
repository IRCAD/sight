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

#include "mesh_test.hpp"

#include <geometry/__/mesh.hpp>

#include <cmath>

namespace sight::geometry::ut
{

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

CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::ut::mesh_test);

//-----------------------------------------------------------------------------

void mesh_test::setUp()
{
}

//-----------------------------------------------------------------------------

void mesh_test::tearDown()
{
}

//-----------------------------------------------------------------------------

void mesh_test::compare(const ::glm::dvec3& _expected, const ::glm::dvec3& _actual)
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
        "Conversion world-barycentric-world error.",
        _expected[X],
        _actual[X],
        EPSILON
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
        "Conversion world-barycentric-world error.",
        _expected[Y],
        _actual[Y],
        EPSILON
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
        "Conversion world-barycentric-world error.",
        _expected[Z],
        _actual[Z],
        EPSILON
    );
}

//-----------------------------------------------------------------------------

void mesh_test::compute_barycenter_abc_2d()
{
    // With the same Z it correspond to a 2d case

    // Creating triangle points A, B, C.
    const ::glm::dvec3 a {9., 6., 1.};
    const ::glm::dvec3 b {9., 12., 1.};
    const ::glm::dvec3 c {2., 9., 1.};

    // Creating intersection Point P.
    const ::glm::dvec3 p {7., 9., 1.};

    const ::glm::dvec3 barycentric = geometry::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE(
        "0 ≤ v ≤ 1",
        std::isgreaterequal(barycentric[V], 0.) && std::islessequal(
            barycentric[V],
            1.
        )
    );
    CPPUNIT_ASSERT_MESSAGE(
        "0 ≤ w ≤ 1",
        std::isgreaterequal(barycentric[W], 0.) && std::islessequal(
            barycentric[W],
            1.
        )
    );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", std::islessequal(barycentric[V] + barycentric[W], 1.));

    // Convert back to world coordinates.

    const ::glm::dvec3 p2 = geometry::from_barycentric_coord(barycentric, a, b, c);

    sight::geometry::ut::mesh_test::compare(p, p2);
}

//-----------------------------------------------------------------------------

void mesh_test::compute_barycenter_abc_3d()
{
    // Second test in 3d.
    const ::glm::dvec3 a {1., 0., 0.};
    const ::glm::dvec3 b {0., 1., 0.};
    const ::glm::dvec3 c {0., 0., 1.};

    // Should be at the center of ABC
    const ::glm::dvec3 p {1. / 3., 1. / 3., 1. / 3.};

    const ::glm::dvec3 barycentric = geometry::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE(
        "0 ≤ v ≤ 1",
        std::isgreaterequal(barycentric[V], 0.) && std::islessequal(
            barycentric[V],
            1.
        )
    );
    CPPUNIT_ASSERT_MESSAGE(
        "0 ≤ w ≤ 1",
        std::isgreaterequal(barycentric[W], 0.) && std::islessequal(
            barycentric[W],
            1.
        )
    );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", std::islessequal(barycentric[V] + barycentric[W], 1.));

    // Convert back to world coordinates.

    const ::glm::dvec3 p2 = geometry::from_barycentric_coord(barycentric, a, b, c);

    sight::geometry::ut::mesh_test::compare(p, p2);
}

//-----------------------------------------------------------------------------

void mesh_test::compute_barycenter_abc_real_coords()
{
    // Second test in 3d.
    const ::glm::dvec3 a {-0.5, 0., 3.};
    const ::glm::dvec3 b {0.5, -0.7071, 3.7071};
    const ::glm::dvec3 c {-0.5, -0.7071, 3.7071};

    // Should be at the center of ABC
    const ::glm::dvec3 p {-0.39, -0.30, 3.30};

    const ::glm::dvec3 barycentric = geometry::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE(
        "0 ≤ v ≤ 1",
        std::isgreaterequal(barycentric[V], 0.) && std::islessequal(
            barycentric[V],
            1.
        )
    );
    CPPUNIT_ASSERT_MESSAGE(
        "0 ≤ w ≤ 1",
        std::isgreaterequal(barycentric[W], 0.) && std::islessequal(
            barycentric[W],
            1.
        )
    );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", std::islessequal(barycentric[V] + barycentric[W], 1.));

    // Convert back to world coordinates.

    const ::glm::dvec3 p2 = geometry::from_barycentric_coord(barycentric, a, b, c);

    sight::geometry::ut::mesh_test::compare(p, p2);
}

//-----------------------------------------------------------------------------

void mesh_test::compute_barycenter_outside_abc()
{
    // Test with a point outside of the triangle.

    // Second test in 3d.
    const ::glm::dvec3 a {0., 0., 0.};
    const ::glm::dvec3 b {0., 1., 0.};
    const ::glm::dvec3 c {1., 0., 1.};

    const ::glm::dvec3 p {10., 20., 30.};

    const ::glm::dvec3 barycentric = geometry::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, EPSILON);

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] > 1.); // v = 20
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] > 1.); //w = 20
}

//-----------------------------------------------------------------------------

void mesh_test::compute_barycenter_tetrahedron()
{
    // First test in 3d.
    const ::glm::dvec3 a {0., 0., 0.};
    const ::glm::dvec3 b {1., 0., 0.};
    const ::glm::dvec3 c {0., 1., 0.};
    const ::glm::dvec3 d {0., 0., 1.};

    // Should be at the center of ABC
    const ::glm::dvec3 p {0.25, 0.25, 0.25};

    const ::glm::dvec4 barycentric = geometry::to_barycentric_coord(p, a, b, c, d);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W] + barycentric[H]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w + h = 1", 1., sum, EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] >= 0. && barycentric[V] <= 1);
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] >= 0. && barycentric[W] <= 1);
    CPPUNIT_ASSERT_MESSAGE("0 ≤ h ≤ 1", barycentric[H] >= 0. && barycentric[H] <= 1);
    CPPUNIT_ASSERT_MESSAGE("v + w + h ≤ 1", (barycentric[V] + barycentric[W] + barycentric[H]) <= 1.);

    // Convert back to world coordinates.

    const ::glm::dvec3 p2 = geometry::from_barycentric_coord(barycentric, a, b, c, d);

    sight::geometry::ut::mesh_test::compare(p, p2);

    CPPUNIT_ASSERT_MESSAGE(
        "The two methods verifying if a point is inside a tetrahedron don't provide the same results",
        geometry::is_inside_tetrahedron(
            p,
            a,
            b,
            c,
            d
        ) == geometry::is_inside_tetrahedron(barycentric)
    );
}

//------------------------------------------------------------------------------

void mesh_test::compute_barycenter_outside_tetrahedron()
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

    const bool is_inside_p_in     = geometry::is_inside_tetrahedron(p_in, a, b, c, d);
    const bool is_inside_p_out    = geometry::is_inside_tetrahedron(p_out, a, b, c, d);
    const bool is_inside_p_edge   = geometry::is_inside_tetrahedron(p_edge, a, b, c, d);
    const bool is_inside_p_vertex = geometry::is_inside_tetrahedron(p_vertex, a, b, c, d);

    CPPUNIT_ASSERT_MESSAGE("p_in should be inside though is compute outside", is_inside_p_in);
    CPPUNIT_ASSERT_MESSAGE("p_out should be outside though is compute inside", !is_inside_p_out);
    CPPUNIT_ASSERT_MESSAGE("p_edge should be inside though is compute outside", is_inside_p_edge);
    CPPUNIT_ASSERT_MESSAGE("p_vertex should be inside though is compute outside", is_inside_p_vertex);
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::ut
