/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARLCORE_UT_CALIBRATIONTEST_HPP__
#define __ARLCORE_UT_CALIBRATIONTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ARLcore
{
namespace ut
{

class CalibrationTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( CalibrationTest );
CPPUNIT_TEST( initIntrinsicCalibrationTest );
CPPUNIT_TEST( initExtrinsicCalibrationTest );
CPPUNIT_TEST( intrinsicCalibrationTest     );
CPPUNIT_TEST( extrinsicCalibrationTest     );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void initIntrinsicCalibrationTest();
    void initExtrinsicCalibrationTest();
    void intrinsicCalibrationTest();
    void extrinsicCalibrationTest();


};

} //namespace ut
} //namespace ARLCORE
#endif //__ARLCORE_UT_CALIBRATIONTEST_HPP__

