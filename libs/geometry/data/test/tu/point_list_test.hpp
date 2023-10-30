/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include <data/point_list.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::geometry::data::ut
{

class point_list_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(point_list_test);
    CPPUNIT_TEST(compute_distance);
    CPPUNIT_TEST(transform);
    CPPUNIT_TEST(associate);
    CPPUNIT_TEST(remove_closest_point_nominal);
    CPPUNIT_TEST(remove_closest_point_extreme);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void compute_distance();

    static void transform();

    static void associate();

    static void remove_closest_point_nominal();

    static void remove_closest_point_extreme();
};

} // namespace sight::geometry::data::ut
