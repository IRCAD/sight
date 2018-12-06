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

#ifndef __FWVTKIO_UT_IMAGETEST_HPP__
#define __FWVTKIO_UT_IMAGETEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Image.hpp>
#include <fwData/Array.hpp>

namespace fwVtkIO
{
namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class ImageTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ImageTest );
CPPUNIT_TEST( testImageToVtk );
CPPUNIT_TEST( testFromVtk );

CPPUNIT_TEST( mhdReaderTest );
CPPUNIT_TEST( mhdWriterTest );
CPPUNIT_TEST( vtiReaderTest );
CPPUNIT_TEST( vtiWriterTest );
CPPUNIT_TEST( vtkReaderTest );
CPPUNIT_TEST( vtkWriterTest );

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testImageToVtk();
    void testFromVtk();

    void mhdReaderTest();
    void mhdWriterTest();
    void vtiReaderTest();
    void vtiWriterTest();
    void vtkReaderTest();
    void vtkWriterTest();

};

} // namespace ut
} // namespace fwVtkIO

#endif //__FWVTKIO_UT_IMAGETEST_HPP__
