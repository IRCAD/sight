/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

namespace sight::data::ut
{

class raw_buffer_tl_test : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(raw_buffer_tl_test);
    CPPUNIT_TEST(push_test);
    CPPUNIT_TEST(get_object_test);
    CPPUNIT_TEST(set_object_test);
    CPPUNIT_TEST(modify_time_test);
    CPPUNIT_TEST(copy_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void push_test();
    static void get_object_test();
    static void set_object_test();
    static void modify_time_test();
    static void copy_test();
};

} // namespace sight::data::ut
