/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

namespace sight::geometry::glm
{
namespace ut
{

// To avoid using indexes in our vectors (for mathematical clarity).
// x, y , z.
#define X 0
#define Y 1
#define Z 2
// u, v, w.
#define U X
#define V Y
#define W Z
#define H 3

const static double s_EPSILON = 10e-9;

CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::geometry::glm::ut::MeshFunctionsTest );

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
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Conversion world-barycentric-world error.", _expected[X], _actual[X],
                                         s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Conversion world-barycentric-world error.", _expected[Y], _actual[Y],
                                         s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Conversion world-barycentric-world error.", _expected[Z], _actual[Z],
                                         s_EPSILON);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABC2D()
{
    // With the same Z it correspond to a 2d case

    // Creating triangle points A, B, C.
    const ::glm::dvec3 A {9., 6., 1.};
    const ::glm::dvec3 B {9., 12., 1.};
    const ::glm::dvec3 C {2., 9., 1.};

    // Creating intersection Point P.
    const ::glm::dvec3 P {7., 9., 1.};

    const ::glm::dvec3 barycentric = geometry::glm::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", std::isgreaterequal(barycentric[V], 0.) &&  std::islessequal(barycentric[V],
                                                                                                     1. ));
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", std::isgreaterequal( barycentric[W], 0.) && std::islessequal(barycentric[W],
                                                                                                     1. ) );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", std::islessequal(barycentric[V] + barycentric[W],  1.) );

    // Convert back to world coordinates.

    const ::glm::dvec3 P2 = geometry::glm::fromBarycentricCoord(barycentric, A, B, C);

    this->compare(P, P2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABC3D()
{
    // Second test in 3d.
    const ::glm::dvec3 A {1., 0., 0.};
    const ::glm::dvec3 B {0., 1., 0.};
    const ::glm::dvec3 C {0., 0., 1.};

    // Should be at the center of ABC
    const ::glm::dvec3 P {1./3., 1./3., 1./3.};

    const ::glm::dvec3 barycentric = geometry::glm::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", std::isgreaterequal(barycentric[V], 0.) &&  std::islessequal(barycentric[V],
                                                                                                     1. ));
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", std::isgreaterequal( barycentric[W], 0.) && std::islessequal(barycentric[W],
                                                                                                     1. ) );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", std::islessequal(barycentric[V] + barycentric[W],  1.) );

    // Convert back to world coordinates.

    const ::glm::dvec3 P2 = geometry::glm::fromBarycentricCoord(barycentric, A, B, C);

    this->compare(P, P2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABCRealCoords()
{

    // Second test in 3d.
    const ::glm::dvec3 A {-0.5, 0., 3.};
    const ::glm::dvec3 B {0.5, -0.7071, 3.7071};
    const ::glm::dvec3 C {-0.5, -0.7071, 3.7071};

    // Should be at the center of ABC
    const ::glm::dvec3 P {-0.39, -0.30, 3.30};

    const ::glm::dvec3 barycentric = geometry::glm::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", std::isgreaterequal(barycentric[V], 0.) &&  std::islessequal(barycentric[V],
                                                                                                     1. ));
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", std::isgreaterequal( barycentric[W], 0.) && std::islessequal(barycentric[W],
                                                                                                     1. ) );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", std::islessequal(barycentric[V] + barycentric[W],  1.) );

    // Convert back to world coordinates.

    const ::glm::dvec3 P2 = geometry::glm::fromBarycentricCoord(barycentric, A, B, C);

    this->compare(P, P2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterOutsideABC()
{
    // Test with a point outside of the triangle.

    // Second test in 3d.
    const ::glm::dvec3 A {0., 0., 0.};
    const ::glm::dvec3 B {0., 1., 0.};
    const ::glm::dvec3 C {1., 0., 1.};

    const ::glm::dvec3 P {10., 20., 30.};

    const ::glm::dvec3 barycentric = geometry::glm::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] > 1.); // v = 20
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] > 1. ); //w = 20

}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterTetrahedron()
{
    // First test in 3d.
    const ::glm::dvec3 A {0., 0., 0.};
    const ::glm::dvec3 B {1., 0., 0.};
    const ::glm::dvec3 C {0., 1., 0.};
    const ::glm::dvec3 D {0., 0., 1.};

    // Should be at the center of ABC
    const ::glm::dvec3 P {0.25, 0.25, 0.25};

    const ::glm::dvec4 barycentric = geometry::glm::toBarycentricCoord(P, A, B, C, D);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]+ barycentric[H]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w + h = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] >= 0. && barycentric[V] <= 1 );
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] >= 0. && barycentric[W] <= 1 );
    CPPUNIT_ASSERT_MESSAGE("0 ≤ h ≤ 1", barycentric[H] >= 0. && barycentric[H] <= 1 );
    CPPUNIT_ASSERT_MESSAGE("v + w + h ≤ 1", (barycentric[V] + barycentric[W] + barycentric[H]) <= 1. );

    // Convert back to world coordinates.

    const ::glm::dvec3 P2 = geometry::glm::fromBarycentricCoord(barycentric, A, B, C, D);

    this->compare(P, P2);

    CPPUNIT_ASSERT_MESSAGE(
        "The two methods verifying if a point is inside a tetrahedron don't provide the same results", geometry::glm::isInsideTetrahedron(
            P, A, B, C,
            D) == geometry::glm::isInsideTetrahedron(barycentric) );
}

//------------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterOutsideTetrahedron()
{
    // Test with a point outside of the tetrahedron.

    // Second test in 3d.
    const ::glm::dvec3 A {0., 0., 0.};
    const ::glm::dvec3 B {1., 0., 0.};
    const ::glm::dvec3 C {0., 1., 0.};
    const ::glm::dvec3 D {0., 0., 1.};

    const ::glm::dvec3 Pin {0.25, 0.25, 0.25};
    const ::glm::dvec3 Pout {1., 2., 3.};
    const ::glm::dvec3 Pedge {0.5, 0., 0.};
    const ::glm::dvec3 Pvertex {0., 0., 0.};

    const bool isInsidePin     = geometry::glm::isInsideTetrahedron(Pin, A, B, C, D);
    const bool isInsidePout    = geometry::glm::isInsideTetrahedron(Pout, A, B, C, D);
    const bool isInsidePedge   = geometry::glm::isInsideTetrahedron(Pedge, A, B, C, D);
    const bool isInsidePvertex = geometry::glm::isInsideTetrahedron(Pvertex, A, B, C, D);

    CPPUNIT_ASSERT_MESSAGE("Pin should be inside though is compute outside", isInsidePin);
    CPPUNIT_ASSERT_MESSAGE("Pout should be outside though is compute inside", !isInsidePout);
    CPPUNIT_ASSERT_MESSAGE("Pedge should be inside though is compute outside", isInsidePedge);
    CPPUNIT_ASSERT_MESSAGE("Pvertex should be inside though is compute outside", isInsidePvertex);
}

//-----------------------------------------------------------------------------
} // namespace ut
} // namespace sight::geometry::glm
