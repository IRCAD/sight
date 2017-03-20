/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CALIBRATION3D_UT_HELPERTEST_HPP__
#define __CALIBRATION3D_UT_HELPERTEST_HPP__

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
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void reprojectionRealDatasetTest1();
    void reprojectionRealDatasetTest2();
    void reprojectionIdentity();
};

} //namespace ut
} //namespace calibration3d

#endif //__CALIBRATION3D_UT_HELPERTEST_HPP__

