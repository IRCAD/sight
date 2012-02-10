/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWVTKIO_TEST_TU_IMAGETEST_HPP_
#define _FWVTKIO_TEST_TU_IMAGETEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Image.hpp>
#include <fwData/Array.hpp>

/**
 * @brief Test many methods to create image.
 */
class ImageTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageTest );
    CPPUNIT_TEST( testImageToVtk );
    CPPUNIT_TEST( testFromVtk );

    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testImageToVtk();
    void testFromVtk();
};

#endif //_FWVTKIO_TEST_TU_IMAGETEST_HPP_
