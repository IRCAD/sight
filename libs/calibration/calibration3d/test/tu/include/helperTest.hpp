/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

namespace calibration3d
{
namespace ut
{

class helperTest : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( helperTest );
CPPUNIT_TEST(reprojectionIdentity);
CPPUNIT_TEST(reprojectionRealDatasetTest1);
CPPUNIT_TEST(reprojectionRealDatasetTest2);
CPPUNIT_TEST(toolCalibrationBasic);
CPPUNIT_TEST(toolCalibration);
CPPUNIT_TEST(generateCharucoDict);
CPPUNIT_TEST(chessboardDetectionTest);
CPPUNIT_TEST(chessboardDetectionScaleTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void reprojectionRealDatasetTest1();
    void reprojectionRealDatasetTest2();
    void reprojectionIdentity();
    void toolCalibrationBasic();
    void toolCalibration();
    void generateCharucoDict();
    void chessboardDetectionTest();
    void chessboardDetectionScaleTest();
};

} //namespace ut
} //namespace calibration3d
