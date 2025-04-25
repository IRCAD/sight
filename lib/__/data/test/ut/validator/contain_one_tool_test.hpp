/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

namespace sight::data::validator::ut
{

/// Test the contain_one_skin validator
class contain_one_tool_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(contain_one_tool_test);
CPPUNIT_TEST(test_validator);
CPPUNIT_TEST(test_validator_with_vector);
CPPUNIT_TEST(test_validator_with_map);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void test_validator();
    static void test_validator_with_vector();
    static void test_validator_with_map();
};

} // namespace sight::data::validator::ut
