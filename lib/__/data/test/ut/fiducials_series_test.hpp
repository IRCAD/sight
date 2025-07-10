/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

class fiducials_series_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(fiducials_series_test);
CPPUNIT_TEST(shallow_copy_test);
CPPUNIT_TEST(deep_copy_test);
CPPUNIT_TEST(simple_setter_getter_test);
CPPUNIT_TEST(fiducial_set_setter_getter_test);
CPPUNIT_TEST(referenced_image_setter_getter_test);
CPPUNIT_TEST(fiducial_setter_getter_test);
CPPUNIT_TEST(graphic_coordinates_data_setter_getter_test);
CPPUNIT_TEST(static_test);
CPPUNIT_TEST(bounding_box_test);
CPPUNIT_TEST(add_fiducial_test);
CPPUNIT_TEST(query_fiducials_test);
CPPUNIT_TEST(remove_fiducials_test);
CPPUNIT_TEST(modify_fiducials_test);
CPPUNIT_TEST(point_test);
CPPUNIT_TEST_SUITE_END();

public:

    static void shallow_copy_test();
    static void deep_copy_test();
    static void simple_setter_getter_test();
    static void fiducial_set_setter_getter_test();
    static void referenced_image_setter_getter_test();
    static void fiducial_setter_getter_test();
    static void graphic_coordinates_data_setter_getter_test();

    static void static_test();
    static void bounding_box_test();

    static void add_fiducial_test();
    static void query_fiducials_test();
    static void remove_fiducials_test();
    static void modify_fiducials_test();

    static void point_test();
};

} // namespace sight::data::ut
