/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

namespace sight::io::session
{

namespace ut
{

class SessionTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SessionTest);
CPPUNIT_TEST(booleanTest);
CPPUNIT_TEST(integerTest);
CPPUNIT_TEST(floatTest);
CPPUNIT_TEST(stringTest);
CPPUNIT_TEST(circularTest);
CPPUNIT_TEST(compositeTest);
CPPUNIT_TEST(meshTest);
CPPUNIT_TEST(equipmentTest);
CPPUNIT_TEST(patientTest);
CPPUNIT_TEST(studyTest);
CPPUNIT_TEST(seriesTest);
CPPUNIT_TEST(activitySeriesTest);
CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    void setUp();
    void tearDown();

    // Tests
    void booleanTest();
    void integerTest();
    void floatTest();
    void stringTest();
    void circularTest();
    void compositeTest();
    void meshTest();
    void equipmentTest();
    void patientTest();
    void studyTest();
    void seriesTest();
    void activitySeriesTest();
};

} // namespace ut

} // namespace sight::io::session
