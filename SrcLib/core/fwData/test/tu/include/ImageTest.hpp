/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_IMAGETEST_HPP__
#define __FWDATA_UT_IMAGETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
namespace ut
{

class ImageTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageTest );
    CPPUNIT_TEST( testGetterSetter );
    CPPUNIT_TEST( testAllocation );
    CPPUNIT_TEST( testReallocation );
    CPPUNIT_TEST( testPixelType );
    CPPUNIT_TEST( testSetGetPixel );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testGetterSetter();
    void testAllocation();
    void testReallocation();
    void testPixelType();
    void testSetGetPixel();
};

} //namespace ut
} //namespace fwData
#endif //__FWDATA_UT_IMAGETEST_HPP__
