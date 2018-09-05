/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_UT_CAMERASERIESTEST_HPP__
#define __ARDATA_UT_CAMERASERIESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace arData
{
namespace ut
{

class CameraSeriesTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( CameraSeriesTest );
    CPPUNIT_TEST(cameraTest);
    CPPUNIT_TEST(shallowCopyTest);
    CPPUNIT_TEST(deepCopyTest);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void cameraTest();
    void shallowCopyTest();
    void deepCopyTest();


};

} //namespace ut
} //namespace arData

#endif //__ARDATA_UT_CAMERASERIESTEST_HPP__

