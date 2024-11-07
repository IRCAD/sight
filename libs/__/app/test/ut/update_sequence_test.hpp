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

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::app::ut
{

class update_sequence_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(update_sequence_test);
CPPUNIT_TEST(basic);
CPPUNIT_TEST(parent);
CPPUNIT_TEST(call_start_stop);
CPPUNIT_TEST(call_slot_while_stopped);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void basic();
    static void parent();
    static void call_start_stop();
    static void call_slot_while_stopped();
};

} // namespace sight::app::ut
