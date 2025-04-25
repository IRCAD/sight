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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include <glm/glm.hpp>

namespace sight::geometry::ut
{

class mesh_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(mesh_test);
    CPPUNIT_TEST(compute_barycenter_abc_2d);
    CPPUNIT_TEST(compute_barycenter_abc_3d);
    CPPUNIT_TEST(compute_barycenter_outside_abc);
    CPPUNIT_TEST(compute_barycenter_abc_real_coords);
    CPPUNIT_TEST(compute_barycenter_tetrahedron);
    CPPUNIT_TEST(compute_barycenter_outside_tetrahedron);
    CPPUNIT_TEST_SUITE_END();

    /// Compare value of 2 ::glm::dvec3.
    static void compare(const ::glm::dvec3& _expected, const ::glm::dvec3& _actual);

public:

    void setUp() override;
    void tearDown() override;

    static void check_barycentric_coords();
    /// Computes barrycentric coordinates on a point inside a ABC triangle in 2D (same value for Z).
    static void compute_barycenter_abc_2d();
    /// Computes barrycentric coordinates on a point inside a ABC triangle in 3D.
    static void compute_barycenter_abc_3d();
    /// Computes barrycentric coordinates on a point inside a ABC triangle in 3D (real case).
    static void compute_barycenter_abc_real_coords();
    /// Computes barrycentric coordinates on a point outside a ABC triangle.
    static void compute_barycenter_outside_abc();
    /// Computes barycentric coordinates on a point inside a ABCD Tetrahedron in 3D.
    static void compute_barycenter_tetrahedron();
    /// Computes barycentric coordinates on a point outside a ABCD Tetrahedron.
    static void compute_barycenter_outside_tetrahedron();
};

} // namespace sight::geometry::ut
