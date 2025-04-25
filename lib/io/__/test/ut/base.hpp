/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

namespace sight::io::ut
{

class base_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(base_test);
CPPUNIT_TEST(file_test);
CPPUNIT_TEST(files_test);
CPPUNIT_TEST(folder_test);
CPPUNIT_TEST(resource_test);
CPPUNIT_TEST(open_location_dialog_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void file_test();
    static void files_test();
    static void folder_test();
    static void resource_test();
    static void open_location_dialog_test();
};

} // namespace sight::io::ut
