/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "CalibrationTest.hpp"
#include <arlcore/UnitTestArlCore.h>


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ARLcore::ut::CalibrationTest );

namespace ARLcore
{
namespace ut
{

void CalibrationTest::setUp()
{
    // Set up context before running a test.

}
void CalibrationTest::tearDown()
{
    // Clean up after the test run.
}



void CalibrationTest::initIntrinsicCalibrationTest()
{
    CPPUNIT_ASSERT_EQUAL(true, arlCore::testInitExtrinsicCalibration( 100, 1e-6 ) );
}



void CalibrationTest::initExtrinsicCalibrationTest()
{
    CPPUNIT_ASSERT_EQUAL(true,  arlCore::testInitIntrinsicCalibration( 100, 1e-5 ) );
}





void CalibrationTest::intrinsicCalibrationTest()
{
    CPPUNIT_ASSERT_EQUAL(true,  arlCore::testIntrinsicCalibration( 8, 1e-3 ) );
}



void CalibrationTest::extrinsicCalibrationTest()
{
    CPPUNIT_ASSERT_EQUAL(true,  arlCore::testExtrinsicCalibration( 10, 1e-6 ) );
}






} //namespace ut
} //namespace ARLcore
