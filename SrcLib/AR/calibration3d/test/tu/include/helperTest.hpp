/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
CPPUNIT_TEST(toolCalibration);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void reprojectionRealDatasetTest1();
    void reprojectionRealDatasetTest2();
    void reprojectionIdentity();
    void toolCalibration();
};

} //namespace ut
} //namespace calibration3d
