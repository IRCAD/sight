/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <data/transfer_function.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class transfer_function_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(transfer_function_test);
    CPPUNIT_TEST(constructor_test);
    CPPUNIT_TEST(default_tf_test);
    CPPUNIT_TEST(classic_get_set_test);
    CPPUNIT_TEST(set_tf_data_test);
    CPPUNIT_TEST(shallow_and_deep_copy_test);
    CPPUNIT_TEST(linear_color_test);
    CPPUNIT_TEST(nearest_color_test);
    CPPUNIT_TEST(map_values_test);
    CPPUNIT_TEST(piecewise_function_test);
    CPPUNIT_TEST(equality_test);
    CPPUNIT_TEST(merge_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void constructor_test();
    static void default_tf_test();
    static void classic_get_set_test();
    void using_tf_test();
    static void set_tf_data_test();
    static void shallow_and_deep_copy_test();
    static void linear_color_test();
    static void nearest_color_test();
    static void map_values_test();
    static void piecewise_function_test();
    static void equality_test();
    static void merge_test();

    static data::transfer_function::sptr create_tf_color();
    static void check_tf_color(data::transfer_function::sptr _tf);
};

} // namespace sight::data::ut
