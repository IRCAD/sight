/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

namespace sight::app::parser::ut
{

class generic_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(generic_test);
CPPUNIT_TEST(boolean_test);
CPPUNIT_TEST(color_test);
CPPUNIT_TEST(integer_test);
CPPUNIT_TEST(real_test);
CPPUNIT_TEST(string_test);
CPPUNIT_TEST(vec_test);
CPPUNIT_TEST_SUITE_END();

public:

    static void boolean_test();
    static void color_test();
    static void integer_test();
    static void real_test();
    static void string_test();
    static void vec_test();
};

} // namespace sight::app::parser::ut
