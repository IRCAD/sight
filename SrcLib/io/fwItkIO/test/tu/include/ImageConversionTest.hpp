/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_TEST_TU_IMAGECONVERSIONTEST_HPP_
#define _FWITKIO_TEST_TU_IMAGECONVERSIONTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Image.hpp>

class ImageConversionTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageConversionTest );
    CPPUNIT_TEST( testConversion );
    CPPUNIT_TEST( stressTest );
    CPPUNIT_TEST( testConversion2D );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();
    // fonctions de tests
    void testConversion();
    void stressTest();
    void testConversion2D();

    template< class TYPE >
    void stressTestForAType();

};

#include "ImageConversionTest.hxx"

#endif // _FWITKIO_TEST_TU_IMAGECONVERSIONTEST_HPP_
