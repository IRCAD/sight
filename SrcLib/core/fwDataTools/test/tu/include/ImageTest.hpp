/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_TEST_TU_IMAGETEST_HPP_
#define _FWDATATOOLS_TEST_TU_IMAGETEST_HPP_

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

#endif //_FWDATATOOLS_TEST_TU_IMAGETEST_HPP_
