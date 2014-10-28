/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_TEST_TU_IMAGEREADERWRITERTEST_HPP__
#define __IOVTK_TEST_TU_IMAGEREADERWRITERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwData
{
    class Image;
}

namespace ioVTK
{
namespace ut
{

/**
 * @brief Test the reader and the writer of Image using VTK.
 */
class ImageReaderWriterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageReaderWriterTest );
    CPPUNIT_TEST( testVtkImageReader );
    CPPUNIT_TEST( testVtiImageReader );
    CPPUNIT_TEST( testMhdImageReader );
    CPPUNIT_TEST( testImageReaderExtension);
    CPPUNIT_TEST( testVtkImageWriter );
    CPPUNIT_TEST( testVtkImageSeriesWriter );
    CPPUNIT_TEST( testVtiImageWriter );
    CPPUNIT_TEST( testMhdImageWriter );
    CPPUNIT_TEST( testImageWriterExtension );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testVtkImageReader();
    void testVtiImageReader();
    void testMhdImageReader();
    void testImageReaderExtension();
    void testVtkImageWriter();
    void testVtkImageSeriesWriter();
    void testVtiImageWriter();
    void testMhdImageWriter();
    void testImageWriterExtension();


private :
    void runImageSrv(
            const std::string &srvtype,
            const std::string &srv,
            const SPTR(::fwRuntime::EConfigurationElement)& cfg,
            const SPTR(::fwData::Object)& image);


};

} //namespace ut
} //namespace ioVTK

#endif //__IOVTK_TEST_TU_IMAGEREADERWRITERTEST_HPP__

