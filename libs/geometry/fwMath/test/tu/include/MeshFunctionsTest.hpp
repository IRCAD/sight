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

#pragma once

#include <fwMath/IntrasecTypes.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <glm/glm.hpp>

namespace fwMath
{
namespace ut
{

class MeshFunctionsTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( MeshFunctionsTest );
    CPPUNIT_TEST( computeBarycenterABC2D);
    CPPUNIT_TEST( computeBarycenterABC3D);
    CPPUNIT_TEST( computeBarycenterOutsideABC);
    CPPUNIT_TEST( computeBarycenterABCRealCoords);
    CPPUNIT_TEST( computeBarycenterTetrahedron);
    CPPUNIT_TEST( computeBarycenterOutsideTetrahedron);
    CPPUNIT_TEST_SUITE_END();

    /// Compare value of 2 ::glm::dvec3.
    void compare(const ::glm::dvec3& _expected, const ::glm::dvec3& _actual);

public:

    void setUp();
    void tearDown();

    void checkBarycentricCoords();
    /// Computes barrycentric coordinates on a point inside a ABC triangle in 2D (same value for Z).
    void computeBarycenterABC2D();
    /// Computes barrycentric coordinates on a point inside a ABC triangle in 3D.
    void computeBarycenterABC3D();
    /// Computes barrycentric coordinates on a point inside a ABC triangle in 3D (real case).
    void computeBarycenterABCRealCoords();
    /// Computes barrycentric coordinates on a point outside a ABC triangle.
    void computeBarycenterOutsideABC();
    /// Computes barycentric coordinates on a point inside a ABCD Tetrahedron in 3D.
    void computeBarycenterTetrahedron();
    /// Computes barycentric coordinates on a point outside a ABCD Tetrahedron.
    void computeBarycenterOutsideTetrahedron();

};

} // namespace ut
} // namespace fwMath
