/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOVTK_TEST_TU_IMAGEREADERWRITERTEST_HPP_
#define _IOVTK_TEST_TU_IMAGEREADERWRITERTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

/**
 * @brief Test the reader and the writer of Image using VTK.
 */
class ImageReaderWriterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageReaderWriterTest );
    CPPUNIT_TEST( testVtkImageReader );
    CPPUNIT_TEST( testVtiImageReader );
    CPPUNIT_TEST( testMhdImageReader );
    CPPUNIT_TEST( testVtkImageWriter );
    CPPUNIT_TEST( testVtiImageWriter );
    CPPUNIT_TEST( testMhdImageWriter );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testVtkImageReader();
    void testVtiImageReader();
    void testMhdImageReader();
    void testVtkImageWriter();
    void testVtiImageWriter();
    void testMhdImageWriter();
    
private :
    void readImage(const ::fwRuntime::EConfigurationElement::sptr cfg, ::fwData::Image::sptr image);
    

};

#endif //_IOVTK_TEST_TU_IMAGEREADERWRITERTEST_HPP_
