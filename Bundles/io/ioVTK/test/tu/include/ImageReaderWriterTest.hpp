/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include <cppunit/extensions/HelperMacros.h>

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
CPPUNIT_TEST( testBitmapImageWriter );
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
    void testBitmapImageWriter();
    void testVtkImageWriter();
    void testVtkImageSeriesWriter();
    void testVtiImageWriter();
    void testMhdImageWriter();
    void testImageWriterExtension();

};

} //namespace ut
} //namespace ioVTK
