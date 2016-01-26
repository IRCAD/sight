/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_UT_IMAGETEST_HPP__
#define __FWDATATOOLS_UT_IMAGETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Image.hpp>


namespace fwDataTools
{
namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class ImageTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImageTest );
CPPUNIT_TEST( roiApplyTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();


    void roiApplyTest();

};

} //namespace ut
} //namespace fwDataTools

#endif //__FWDATATOOLS_UT_IMAGETEST_HPP__
