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
 * @brief Test the concatenate_matrices service.
 */
class concatenate_matrices_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(concatenate_matrices_test);
CPPUNIT_TEST(identity);
CPPUNIT_TEST(single);
CPPUNIT_TEST(multiply_2);
CPPUNIT_TEST(multiply_4);
CPPUNIT_TEST(multiply_inverse);
CPPUNIT_TEST(auto_connect);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    static void identity();
    static void single();
    static void multiply_2();
    static void multiply_4();
    static void multiply_inverse();
    static void auto_connect();
};

} // namespace sight::module::geometry::ut
