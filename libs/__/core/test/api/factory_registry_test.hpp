/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

namespace sight::core::ut
{

class factory_registry_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(factory_registry_test);
CPPUNIT_TEST(pointer_test);
CPPUNIT_TEST(value_test);
CPPUNIT_TEST(arg_test);
CPPUNIT_TEST(thread_safety_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void pointer_test();
    static void value_test();
    static void arg_test();
    static void thread_safety_test();
};

} // namespace sight::core::ut
