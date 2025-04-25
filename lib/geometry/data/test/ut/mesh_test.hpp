/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/image.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::geometry::data::ut
{

/**
 * @brief Test many methods to create image.
 */
class mesh_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(mesh_test);
CPPUNIT_TEST(colorize_points_test);
CPPUNIT_TEST(colorize_cells_test);
CPPUNIT_TEST(transform_test);
CPPUNIT_TEST(is_closed_test);
CPPUNIT_TEST(cell_normal_test);
CPPUNIT_TEST(point_normal_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void colorize_points_test();
    static void colorize_cells_test();
    static void transform_test();
    static void is_closed_test();
    static void cell_normal_test();
    static void point_normal_test();
};

} // namespace sight::geometry::data::ut
