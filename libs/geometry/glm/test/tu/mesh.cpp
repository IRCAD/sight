/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "mesh.hpp"

#include <geometry/glm/mesh.hpp>

#include <cmath>

namespace sight::geometry::glm::ut
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

const static double s_EPSILON = 10e-9;

CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::glm::ut::MeshFunctionsTest);

//-----------------------------------------------------------------------------

void MeshFunctionsTest::setUp()
{
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::tearDown()
{
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::compare(const ::glm::dvec3& _expected, const ::glm::dvec3& _actual)
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
        "Conversion world-barycentric-world error.",
        _expected[X],
        _actual[X],
        s_EPSILON
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
        "Conversion world-barycentric-world error.",
        _expected[Y],
        _actual[Y],
        s_EPSILON
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
        "Conversion world-barycentric-world error.",
        _expected[Z],
        _actual[Z],
        s_EPSILON
    );
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABC2D()
{
    // With the same Z it correspond to a 2d case

    // Creating triangle points A, B, C.
    const ::glm::dvec3 a {9., 6., 1.};
    const ::glm::dvec3 b {9., 12., 1.};
    const ::glm::dvec3 c {2., 9., 1.};

    // Creating intersection Point P.
    const ::glm::dvec3 p {7., 9., 1.};

    const ::glm::dvec3 barycentric = geometry::glm::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

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

    const ::glm::dvec3 p2 = geometry::glm::from_barycentric_coord(barycentric, a, b, c);

    this->compare(p, p2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABC3D()
{
    // Second test in 3d.
    const ::glm::dvec3 a {1., 0., 0.};
    const ::glm::dvec3 b {0., 1., 0.};
    const ::glm::dvec3 c {0., 0., 1.};

    // Should be at the center of ABC
    const ::glm::dvec3 p {1. / 3., 1. / 3., 1. / 3.};

    const ::glm::dvec3 barycentric = geometry::glm::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

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

    const ::glm::dvec3 p2 = geometry::glm::from_barycentric_coord(barycentric, a, b, c);

    this->compare(p, p2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABCRealCoords()
{
    // Second test in 3d.
    const ::glm::dvec3 a {-0.5, 0., 3.};
    const ::glm::dvec3 b {0.5, -0.7071, 3.7071};
    const ::glm::dvec3 c {-0.5, -0.7071, 3.7071};

    // Should be at the center of ABC
    const ::glm::dvec3 p {-0.39, -0.30, 3.30};

    const ::glm::dvec3 barycentric = geometry::glm::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

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

    const ::glm::dvec3 p2 = geometry::glm::from_barycentric_coord(barycentric, a, b, c);

    this->compare(p, p2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterOutsideABC()
{
    // Test with a point outside of the triangle.

    // Second test in 3d.
    const ::glm::dvec3 a {0., 0., 0.};
    const ::glm::dvec3 b {0., 1., 0.};
    const ::glm::dvec3 c {1., 0., 1.};

    const ::glm::dvec3 p {10., 20., 30.};

    const ::glm::dvec3 barycentric = geometry::glm::to_barycentric_coord(p, a, b, c);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] > 1.); // v = 20
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] > 1.); //w = 20
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterTetrahedron()
{
    // First test in 3d.
    const ::glm::dvec3 a {0., 0., 0.};
    const ::glm::dvec3 b {1., 0., 0.};
    const ::glm::dvec3 c {0., 1., 0.};
    const ::glm::dvec3 d {0., 0., 1.};

    // Should be at the center of ABC
    const ::glm::dvec3 p {0.25, 0.25, 0.25};

    const ::glm::dvec4 barycentric = geometry::glm::to_barycentric_coord(p, a, b, c, d);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W] + barycentric[H]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w + h = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] >= 0. && barycentric[V] <= 1);
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] >= 0. && barycentric[W] <= 1);
    CPPUNIT_ASSERT_MESSAGE("0 ≤ h ≤ 1", barycentric[H] >= 0. && barycentric[H] <= 1);
    CPPUNIT_ASSERT_MESSAGE("v + w + h ≤ 1", (barycentric[V] + barycentric[W] + barycentric[H]) <= 1.);

    // Convert back to world coordinates.

    const ::glm::dvec3 p2 = geometry::glm::from_barycentric_coord(barycentric, a, b, c, d);

    this->compare(p, p2);

    CPPUNIT_ASSERT_MESSAGE(
        "The two methods verifying if a point is inside a tetrahedron don't provide the same results",
        geometry::glm::is_inside_tetrahedron(
            p,
            a,
            b,
            c,
            d
        ) == geometry::glm::is_inside_tetrahedron(barycentric)
    );
}

//------------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterOutsideTetrahedron()
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

    const bool is_inside_p_in     = geometry::glm::is_inside_tetrahedron(p_in, a, b, c, d);
    const bool is_inside_p_out    = geometry::glm::is_inside_tetrahedron(p_out, a, b, c, d);
    const bool is_inside_p_edge   = geometry::glm::is_inside_tetrahedron(p_edge, a, b, c, d);
    const bool is_inside_p_vertex = geometry::glm::is_inside_tetrahedron(p_vertex, a, b, c, d);

    CPPUNIT_ASSERT_MESSAGE("p_in should be inside though is compute outside", is_inside_p_in);
    CPPUNIT_ASSERT_MESSAGE("p_out should be outside though is compute inside", !is_inside_p_out);
    CPPUNIT_ASSERT_MESSAGE("p_edge should be inside though is compute outside", is_inside_p_edge);
    CPPUNIT_ASSERT_MESSAGE("p_vertex should be inside though is compute outside", is_inside_p_vertex);
}

//-----------------------------------------------------------------------------

} // namespace sight::geometry::glm::ut
