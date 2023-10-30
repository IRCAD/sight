/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

/**
 * @brief The point_list_test class
 * This class is used to test data::point_list
 */
class landmarks_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(landmarks_test);
    CPPUNIT_TEST(copy_test);
    CPPUNIT_TEST(group_test);
    CPPUNIT_TEST(points_test);
    CPPUNIT_TEST(equality_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    /// test deep and shallow copy
    static void copy_test();

    /// test to add/remove groups and change properties
    static void group_test();

    /// test to add/ remove points in group
    static void points_test();

    static void equality_test();
};

} // namespace sight::data::ut
