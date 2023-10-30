/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

namespace sight::module::filter::mesh::ut
{

/**
 * @brief Test apply the threshold value on the image.
 */
class vtk_mesher_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(vtk_mesher_test);
CPPUNIT_TEST(generate_mesh);
CPPUNIT_TEST(generate_mesh_with_min_reduction);
CPPUNIT_TEST(no_mesh_generated);
CPPUNIT_TEST(update_threshold_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    static void generate_mesh();
    static void generate_mesh_with_min_reduction();
    static void no_mesh_generated();
    static void update_threshold_test();
};

} // namespace sight::module::filter::mesh::ut
