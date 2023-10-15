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

namespace sight::io::session::ut
{

class SessionTest : public CPPUNIT_NS::TestFixture
{
public:

    CPPUNIT_TEST_SUITE(SessionTest);

    CPPUNIT_TEST(booleanTest);
    CPPUNIT_TEST(integerTest);
    CPPUNIT_TEST(floatTest);
    CPPUNIT_TEST(stringTest);
    CPPUNIT_TEST(compositeTest);
    CPPUNIT_TEST(meshTest);
    CPPUNIT_TEST(seriesTest);
    CPPUNIT_TEST(activityTest);
    CPPUNIT_TEST(arrayTest);
    CPPUNIT_TEST(imageTest);
    CPPUNIT_TEST(vectorTest);
    CPPUNIT_TEST(pointTest);
    CPPUNIT_TEST(pointListTest);
    CPPUNIT_TEST(calibrationInfoTest);
    CPPUNIT_TEST(cameraTest);
    CPPUNIT_TEST(colorTest);
    CPPUNIT_TEST(landmarksTest);
    CPPUNIT_TEST(lineTest);
    CPPUNIT_TEST(materialTest);
    CPPUNIT_TEST(matrix4_test);
    CPPUNIT_TEST(planeTest);
    CPPUNIT_TEST(planeListTest);
    CPPUNIT_TEST(reconstructionTest);
    CPPUNIT_TEST(structureTraitsTest);
    CPPUNIT_TEST(structureTraitsDictionaryTest);
    CPPUNIT_TEST(resectionTest);
    CPPUNIT_TEST(resectionDBTest);
    CPPUNIT_TEST(transferFunctionTest);
    CPPUNIT_TEST(dicomSeriesTest);
    CPPUNIT_TEST(imageSeriesTest);
    CPPUNIT_TEST(modelSeriesTest);
    CPPUNIT_TEST(activitySetTest);
    CPPUNIT_TEST(cameraSetTest);
    CPPUNIT_TEST(seriesSetTest);
    CPPUNIT_TEST(setTest);

    CPPUNIT_TEST(customSerializerTest);

    CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    void setUp() override;
    void tearDown() override;

    // Tests
    static void booleanTest();
    static void integerTest();
    static void floatTest();
    static void stringTest();
    static void compositeTest();
    static void meshTest();
    static void seriesTest();
    static void activityTest();
    static void arrayTest();
    static void imageTest();
    static void vectorTest();
    static void pointTest();
    static void pointListTest();
    static void calibrationInfoTest();
    static void cameraTest();
    static void colorTest();
    static void landmarksTest();
    static void lineTest();
    static void materialTest();
    static void matrix4_test();
    static void planeTest();
    static void planeListTest();
    static void reconstructionTest();
    static void structureTraitsTest();
    static void structureTraitsDictionaryTest();
    static void resectionTest();
    static void resectionDBTest();
    static void transferFunctionTest();
    static void dicomSeriesTest();
    static void imageSeriesTest();
    static void modelSeriesTest();
    static void activitySetTest();
    static void cameraSetTest();
    static void seriesSetTest();
    static void setTest();

    static void customSerializerTest();
};

} // namespace sight::io::session::ut
