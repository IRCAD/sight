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

namespace sight::geometry::data::ut
{

class plane_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(plane_test);
    CPPUNIT_TEST(check_get_plane);
    CPPUNIT_TEST(check_set_plane);
    CPPUNIT_TEST(check_distance);
    CPPUNIT_TEST(check_normal);
    CPPUNIT_TEST(check_intersect);
    CPPUNIT_TEST(check_intersect_ray);
    CPPUNIT_TEST(check_is_in_half_space);
    CPPUNIT_TEST(check_offset);
    CPPUNIT_TEST(check_transform);
    CPPUNIT_TEST(check_operator);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void check_get_plane();
    static void check_set_plane();
    static void check_distance();
    static void check_normal();
    static void check_intersect();
    static void check_intersect_ray();
    static void check_is_in_half_space();
    static void check_offset();
    static void check_transform();
    static void check_operator();
};

} // namespace sight::geometry::data::ut
