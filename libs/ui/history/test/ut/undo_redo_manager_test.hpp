/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

namespace sight::ui::history::ut
{

class undo_redo_manager_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(undo_redo_manager_test);
CPPUNIT_TEST(manager_enqueue_test);
CPPUNIT_TEST(manager_memory_size_test);
CPPUNIT_TEST(manager_command_count_test);
CPPUNIT_TEST(manager_clear_queue_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void manager_enqueue_test();

    static void manager_memory_size_test();

    static void manager_command_count_test();

    static void manager_clear_queue_test();
};

} // namespace sight::ui::history::ut
