/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

class image_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(image_test);
    CPPUNIT_TEST(test_getter_setter);
    CPPUNIT_TEST(test_allocation);
    CPPUNIT_TEST(test_reallocation);
    CPPUNIT_TEST(test_set_get_pixel);
    CPPUNIT_TEST(test_set_get_pixel_rgba);
    CPPUNIT_TEST(test_iterator);
    CPPUNIT_TEST(test_rgb_iterator);
    CPPUNIT_TEST(test_bgr_iterator);
    CPPUNIT_TEST(test_rgba_iterator);
    CPPUNIT_TEST(test_bgra_iterator);
    CPPUNIT_TEST(benchmark_iterator);
    CPPUNIT_TEST(image_deep_copy);
    CPPUNIT_TEST(empty_iterator_test);
    CPPUNIT_TEST(equality_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void test_getter_setter();
    static void test_allocation();
    static void test_reallocation();
    static void test_set_get_pixel();
    static void test_set_get_pixel_rgba();
    static void test_iterator();
    static void test_rgb_iterator();
    static void test_bgr_iterator();
    static void test_rgba_iterator();
    static void test_bgra_iterator();
    static void benchmark_iterator();
    static void image_deep_copy();
    static void empty_iterator_test();
    static void equality_test();
};

} // namespace sight::data::ut
