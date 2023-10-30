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

namespace sight::data::ut
{

/**
 * @brief The point_list_test class
 * This class is used to test data::point_list
 */
class point_list_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(point_list_test);
    CPPUNIT_TEST(copy_test);
    CPPUNIT_TEST(getter_test);
    CPPUNIT_TEST(setter_test);
    CPPUNIT_TEST(push_test);
    CPPUNIT_TEST(remove_test);
    CPPUNIT_TEST(clear_test);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void copy_test();
    static void getter_test();
    static void setter_test();
    static void push_test();
    static void remove_test();
    static void clear_test();
};

} // namespace sight::data::ut
