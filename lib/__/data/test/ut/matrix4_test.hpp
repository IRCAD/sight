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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::data::ut
{

class matrix4_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(matrix4_test);
    CPPUNIT_TEST(getter_setter_by_coef);
    CPPUNIT_TEST(getter_setter_by_array);
    CPPUNIT_TEST(row_matrix_accessor);
    CPPUNIT_TEST(equality_test);
    CPPUNIT_TEST(position_test);
    CPPUNIT_TEST(orientation_test);
    CPPUNIT_TEST(value_test);
    CPPUNIT_TEST(initialization_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void getter_setter_by_coef();
    static void getter_setter_by_array();
    static void row_matrix_accessor();
    static void equality_test();
    static void position_test();
    static void orientation_test();
    static void value_test();
    static void initialization_test();
};

} // namespace sight::data::ut
