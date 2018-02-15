/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __IOITK_UT_IOITKTEST_HPP__
#define __IOITK_UT_IOITKTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ioITK
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class IoItkTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( IoItkTest );
CPPUNIT_TEST( testImageWriterJPG );
CPPUNIT_TEST( testImageSeriesWriterJPG );
CPPUNIT_TEST( testSaveLoadInr );
CPPUNIT_TEST( ImageSeriesInrTest );
CPPUNIT_TEST( SeriesDBInrTest );
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void testImageWriterJPG();
    void testImageSeriesWriterJPG();
    void testSaveLoadInr();
    void ImageSeriesInrTest();
    void SeriesDBInrTest();

};

} //namespace ut
} //namespace ioITK

#endif //__IOITK_UT_IOITKTEST_HPP__

