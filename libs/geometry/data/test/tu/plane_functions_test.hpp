/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

class plane_functions_test : public CPPUNIT_NS::TestFixture
{
private:

    CPPUNIT_TEST_SUITE(plane_functions_test);
    CPPUNIT_TEST(checkGetPlane);
    CPPUNIT_TEST(checkSetValues);
    CPPUNIT_TEST(checkDistance);
    CPPUNIT_TEST(checkNormal);
//    CPPUNIT_TEST( checkIntersect );
    CPPUNIT_TEST(checkIsInHalfSpace);
    CPPUNIT_TEST(checkOffset);
    CPPUNIT_TEST(checkTransform);
    CPPUNIT_TEST(checkOperator);
    CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void checkSetValues();
    static void checkGetPlane();
    static void checkDistance();
    static void checkNormal();
    static void checkIntersect();
    static void checkIsInHalfSpace();
    static void checkOffset();
    static void checkTransform();
    static void checkOperator();
};

} // namespace sight::geometry::data::ut
