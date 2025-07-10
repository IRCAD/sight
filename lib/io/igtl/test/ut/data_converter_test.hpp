/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

namespace sight::io::igtl::detail::ut
{

class data_converter_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(data_converter_test);
CPPUNIT_TEST(mesh_converter_test);
CPPUNIT_TEST(image_converter_test);
CPPUNIT_TEST(matrix_converter_test);
CPPUNIT_TEST(point_list_converter_test);
CPPUNIT_TEST(string_converter_test);
CPPUNIT_TEST(line_converter_test);
CPPUNIT_TEST(scalar_converter_test);
CPPUNIT_TEST(map_converter_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void image_converter_test();
    static void matrix_converter_test();
    static void point_list_converter_test();
    static void string_converter_test();
    static void line_converter_test();
    static void scalar_converter_test();
    static void mesh_converter_test();
    static void map_converter_test();
};

} // namespace sight::io::igtl::detail::ut
