/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
 * This class is used to test data::point
 */
class point_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(point_test);
    CPPUNIT_TEST(copyTest);
    CPPUNIT_TEST(getterTest);
    CPPUNIT_TEST(setterTest);
    CPPUNIT_TEST(labelTest);
    CPPUNIT_TEST(equalityTest);
    CPPUNIT_TEST_SUITE_END();

public:

    /// Does nothing
    void setUp() override;
    void tearDown() override;

    /// Tests copies
    static void copyTest();
    /// Tests getter
    static void getterTest();
    ///Tests setter
    static void setterTest();
    /// Tests labels of data::point
    static void labelTest();

    static void equalityTest();
};

} // namespace sight::data::ut
