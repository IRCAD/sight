/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

namespace sight::core::com::ut
{

class slot_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(slot_test);

CPPUNIT_TEST(build_test);
CPPUNIT_TEST(run_test);
CPPUNIT_TEST(call_test);
CPPUNIT_TEST(async_test);
CPPUNIT_TEST(slot_base_test);
CPPUNIT_TEST(exception_test);
CPPUNIT_TEST(worker_swap_test);
CPPUNIT_TEST(sloppiness_test);
CPPUNIT_TEST(no_worker_test);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void build_test();
    static void run_test();
    static void call_test();
    static void async_test();
    static void slot_base_test();
    static void exception_test();
    static void worker_swap_test();
    static void sloppiness_test();
    static void no_worker_test();
};

} // namespace sight::core::com::ut
