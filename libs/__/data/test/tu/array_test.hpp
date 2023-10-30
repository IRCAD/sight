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

namespace sight::data::ut
{

class array_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(array_test);
    CPPUNIT_TEST(allocation);
    CPPUNIT_TEST(resize);
    CPPUNIT_TEST(reallocate);
    CPPUNIT_TEST(copy);
    CPPUNIT_TEST(dump_lock_test);
    CPPUNIT_TEST(buffer_access_test);
    CPPUNIT_TEST(const_array_test);
    CPPUNIT_TEST(empty_iterator_test);
    CPPUNIT_TEST(equality_test);
    CPPUNIT_TEST(swap_test);
    CPPUNIT_TEST(resize_non_owner_test);
    CPPUNIT_TEST(set_buffer_object_null_then_resize_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void allocation();
    static void resize();
    static void reallocate();
    static void copy();
    static void dump_lock_test();
    static void buffer_access_test();
    static void const_array_test();
    static void empty_iterator_test();
    static void equality_test();
    static void swap_test();
    static void resize_non_owner_test();
    static void set_buffer_object_null_then_resize_test();
    static void at_test();
};

} // namespace sight::data::ut
