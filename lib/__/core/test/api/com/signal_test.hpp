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

class signal_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(signal_test);

CPPUNIT_TEST(build_test);
CPPUNIT_TEST(connect_test);
CPPUNIT_TEST(emit_test);
CPPUNIT_TEST(auto_slot_disconnect_test);
CPPUNIT_TEST(block_test);
CPPUNIT_TEST(argument_loss_test);
CPPUNIT_TEST(async_emit_test);
CPPUNIT_TEST(async_argument_loss_test);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void build_test();
    static void connect_test();
    static void emit_test();
    static void auto_slot_disconnect_test();
    static void block_test();
    static void argument_loss_test();
    static void async_emit_test();
    static void async_argument_loss_test();
};

} // namespace sight::core::com::ut
