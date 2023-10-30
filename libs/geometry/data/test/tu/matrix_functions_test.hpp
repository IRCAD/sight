/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

namespace sight::geometry::data::ut
{

class matrix_functions_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(matrix_functions_test);
    CPPUNIT_TEST(check_mult_vec_matrix);
    CPPUNIT_TEST(check_rotation_matrix);
    CPPUNIT_TEST(check_inverse);
    CPPUNIT_TEST(multiplication_test);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void check_mult_vec_matrix();
    static void check_rotation_matrix();
    static void check_inverse();
    static void multiplication_test();
};

} // namespace sight::geometry::data::ut
