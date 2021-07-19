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
public:

    CPPUNIT_TEST_SUITE(SessionTest);
    CPPUNIT_TEST(booleanTest);
    CPPUNIT_TEST(integerTest);
    CPPUNIT_TEST(floatTest);
    CPPUNIT_TEST(stringTest);
    CPPUNIT_TEST(compositeTest);
    CPPUNIT_TEST(meshTest);
    CPPUNIT_TEST(equipmentTest);
    CPPUNIT_TEST(patientTest);
    CPPUNIT_TEST(studyTest);
    CPPUNIT_TEST(activitySeriesTest);
    CPPUNIT_TEST(arrayTest);
    CPPUNIT_TEST(imageTest);
    CPPUNIT_TEST(vectorTest);
    CPPUNIT_TEST(pointTest);
    CPPUNIT_TEST(pointListTest);
    CPPUNIT_TEST(calibrationInfoTest);
    CPPUNIT_TEST(cameraTest);
    CPPUNIT_TEST(cameraSeriesTest);
    CPPUNIT_TEST(colorTest);
    CPPUNIT_TEST(edgeTest);
    CPPUNIT_TEST(portTest);
    CPPUNIT_TEST(nodeTest);
    CPPUNIT_TEST(graphTest);
    CPPUNIT_TEST(histogramTest);
    CPPUNIT_TEST(landmarksTest);
    CPPUNIT_TEST(lineTest);
    CPPUNIT_TEST(listTest);
    CPPUNIT_TEST(materialTest);
    CPPUNIT_TEST(matrix4Test);
    CPPUNIT_TEST(planeTest);
    CPPUNIT_TEST(planeListTest);
    CPPUNIT_TEST(processObjectTest);
    CPPUNIT_TEST(reconstructionTest);
    CPPUNIT_TEST(structureTraitsTest);
    CPPUNIT_TEST(structureTraitsDictionaryTest);
    CPPUNIT_TEST(reconstructionTraitsTest);
    CPPUNIT_TEST(resectionTest);
    CPPUNIT_TEST(resectionDBTest);
    CPPUNIT_TEST(roiTraitsTest);
    CPPUNIT_TEST(seriesTest);
    CPPUNIT_TEST(seriesDBTest);
    CPPUNIT_TEST(tagTest);
    CPPUNIT_TEST(transferFunctionTest);
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
    void compositeTest();
    void meshTest();
    void equipmentTest();
    void patientTest();
    void studyTest();
    void activitySeriesTest();
    void arrayTest();
    void imageTest();
    void vectorTest();
    void pointTest();
    void pointListTest();
    void calibrationInfoTest();
    void cameraTest();
    void cameraSeriesTest();
    void colorTest();
    void edgeTest();
    void portTest();
    void nodeTest();
    void graphTest();
    void histogramTest();
    void landmarksTest();
    void lineTest();
    void listTest();
    void materialTest();
    void matrix4Test();
    void planeTest();
    void planeListTest();
    void processObjectTest();
    void reconstructionTest();
    void structureTraitsTest();
    void structureTraitsDictionaryTest();
    void reconstructionTraitsTest();
    void resectionTest();
    void resectionDBTest();
    void roiTraitsTest();
    void seriesTest();
    void seriesDBTest();
    void tagTest();
    void transferFunctionTest();
};

} // namespace ut

} // namespace sight::io::session
