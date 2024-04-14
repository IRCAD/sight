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

namespace sight::core::tools::ut
{

class numeric_round_cast_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(numeric_round_cast_test);
CPPUNIT_TEST(cast_float_to_int_test);
CPPUNIT_TEST(cast_double_to_int_test);
CPPUNIT_TEST(cast_int_to_float_test);
CPPUNIT_TEST(cast_double_to_float_test);
CPPUNIT_TEST(cast_int_to_int_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void cast_float_to_int_test();
    static void cast_double_to_int_test();
    static void cast_int_to_float_test();
    static void cast_double_to_float_test();
    static void cast_int_to_int_test();
};

} // namespace sight::core::tools::ut
