/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_UT_CAMERATEST_HPP__
#define __ARDATA_UT_CAMERATEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace arData
{
namespace ut
{

class CameraTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( CameraTest );
    CPPUNIT_TEST(paramTest);
    CPPUNIT_TEST(shallowCopyTest);
    CPPUNIT_TEST(deepCopyTest);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void paramTest();
    void shallowCopyTest();
    void deepCopyTest();


};

} //namespace ut
} //namespace arData

#endif //__ARDATA_UT_CAMERATEST_HPP__

