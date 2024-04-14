/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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
 * @brief Test the weighted_mean_matrix service.
 */
class weighted_mean_matrix_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(weighted_mean_matrix_test);
CPPUNIT_TEST(pass_through_test);
CPPUNIT_TEST(translation_test);
CPPUNIT_TEST(full_test_weight50);
CPPUNIT_TEST(full_test_weight30);
CPPUNIT_TEST(full_test_weight70);
CPPUNIT_TEST(full_test_weight10);
CPPUNIT_TEST(full_test_weight100);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    static void pass_through_test();
    static void translation_test();
    // Full test with weight changes
    // Note that weights are named here with a percentage, but in fact it is between 0.1 and 1.
    static void full_test_weight50();
    static void full_test_weight30();
    static void full_test_weight70();
    static void full_test_weight10();
    static void full_test_weight100();
};

} // namespace sight::module::geometry::ut
