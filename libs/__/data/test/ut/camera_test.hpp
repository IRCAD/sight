/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

class camera_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(camera_test);
CPPUNIT_TEST(param_test);
CPPUNIT_TEST(shallow_copy_test);
CPPUNIT_TEST(deep_copy_test);
CPPUNIT_TEST(equality_test);
CPPUNIT_TEST(get_pixel_format_test);
CPPUNIT_TEST(get_pixel_format_name_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void param_test();
    static void shallow_copy_test();
    static void deep_copy_test();
    static void equality_test();
    static void get_pixel_format_test();
    static void get_pixel_format_name_test();
};

} // namespace sight::data::ut
