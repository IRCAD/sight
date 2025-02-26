/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

namespace sight::geometry::ut
{

class line_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(line_test);
    CPPUNIT_TEST(get_closest_point);
    CPPUNIT_TEST(get_closest_points);
    CPPUNIT_TEST(intersect1);
    CPPUNIT_TEST(intersect2);
    CPPUNIT_TEST(intersect_box);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    static void get_closest_point();
    static void get_closest_points();
    static void intersect1();
    static void intersect2();
    static void intersect_box();
};

} // namespace sight::geometry::ut
