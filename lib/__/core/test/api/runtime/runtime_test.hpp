/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

namespace sight::core::runtime::ut
{

/**
 * @brief   Test Runtime : read the modules in CommonLib
 */
class runtime_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(runtime_test);
CPPUNIT_TEST(test_library);
CPPUNIT_TEST(test_module);
CPPUNIT_TEST(test_operations);
CPPUNIT_TEST_SUITE_END();

public:

    runtime_test();

    // interface
    void setUp() override;
    void tearDown() override;

    static void test_library();
    static void test_module();
    static void test_operations();
};

} // namespace sight::core::runtime::ut
