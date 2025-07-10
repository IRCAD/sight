/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

namespace sight::filter::image::ut
{

class flip_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(flip_test);
CPPUNIT_TEST(flip_along_x_axis_test);
CPPUNIT_TEST(flip_along_y_axis_test);
CPPUNIT_TEST(flip_along_z_axis_test);
CPPUNIT_TEST(flip_along_multiple_axes_test);
CPPUNIT_TEST(flip_empty_image_test);
CPPUNIT_TEST_SUITE_END();

public:

    static void flip_along_x_axis_test();
    static void flip_along_y_axis_test();
    static void flip_along_z_axis_test();
    static void flip_along_multiple_axes_test();
    static void flip_empty_image_test();
};

} // namespace sight::filter::image::ut
