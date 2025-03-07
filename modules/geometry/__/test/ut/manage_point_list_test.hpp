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

namespace sight::module::geometry::ut
{

/**
 * @brief Test the manage_point_list service.
 */
class manage_point_list_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(manage_point_list_test);
CPPUNIT_TEST(update_test);
CPPUNIT_TEST(pick_test);
CPPUNIT_TEST(max_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    static void update_test();
    static void pick_test();
    static void max_test();
};

} // namespace sight::module::geometry::ut
