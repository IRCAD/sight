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

namespace sight::io::zip::ut
{

class archive_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(archive_test);
CPPUNIT_TEST(new_test);
CPPUNIT_TEST(singleton_test);
CPPUNIT_TEST(open_test);
CPPUNIT_TEST(raw_test);
CPPUNIT_TEST(archive_format_to_string_test);
CPPUNIT_TEST(string_to_archive_format);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void new_test();
    static void singleton_test();
    static void open_test();
    static void raw_test();
    static void archive_format_to_string_test();
    static void string_to_archive_format();
};

} // namespace sight::io::zip::ut
