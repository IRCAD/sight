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

namespace sight::core::tools::ut
{

class compare_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(compare_test);
CPPUNIT_TEST(double_container_test);
CPPUNIT_TEST(float_container_test);
CPPUNIT_TEST(mixed_container_test);
CPPUNIT_TEST(sandbox_test);
CPPUNIT_TEST(infinite_test);
CPPUNIT_TEST(nan_test);
CPPUNIT_TEST(pointer_test);
CPPUNIT_TEST(map_test);
CPPUNIT_TEST(unordered_set_test);
CPPUNIT_TEST(pair_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void double_container_test();
    static void float_container_test();
    static void mixed_container_test();

    static void sandbox_test();
    static void infinite_test();
    static void nan_test();

    static void pointer_test();
    static void map_test();
    static void unordered_set_test();
    static void pair_test();
};

} // namespace sight::core::tools::ut
