/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST_UT_IMAGETEST_HPP__
#define __FWTEST_UT_IMAGETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Image.hpp>

namespace fwTest
{
namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class ImageTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImageTest );
CPPUNIT_TEST( generator3DTest );
CPPUNIT_TEST( generator2DTest );
CPPUNIT_TEST( deepCopyTest );
CPPUNIT_TEST( stressTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();


    void generator3DTest();
    void generator2DTest();
    void deepCopyTest();
    void stressTest();

private:
    void stressTestWithType(::fwTools::Type type, int nbTest);


};

} //namespace ut
} //namespace fwTest

#endif //__FWTEST_UT_IMAGETEST_HPP__
