/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

class fiducials_series_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(fiducials_series_test);
CPPUNIT_TEST(shallow_copyTest);
CPPUNIT_TEST(deep_copyTest);
CPPUNIT_TEST(simpleSetterGetterTest);
CPPUNIT_TEST(fiducialSetSetterGetterTest);
CPPUNIT_TEST(referencedImageSetterGetterTest);
CPPUNIT_TEST(fiducialSetterGetterTest);
CPPUNIT_TEST(graphicCoordinatesDataSetterGetterTest);
CPPUNIT_TEST_SUITE_END();

public:

    static void shallow_copyTest();
    static void deep_copyTest();
    static void simpleSetterGetterTest();
    static void fiducialSetSetterGetterTest();
    static void referencedImageSetterGetterTest();
    static void fiducialSetterGetterTest();
    static void graphicCoordinatesDataSetterGetterTest();
};

} // namespace sight::data::ut
