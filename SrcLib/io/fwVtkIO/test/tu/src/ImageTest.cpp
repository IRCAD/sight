/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkGenericDataObjectReader.h>

#include <fwTools/System.hpp>

#include <fwDataTools/Image.hpp>

#include <fwTest/Data.hpp>

#include <vtkIO/ImageWriter.hpp>
#include <vtkIO/ImageReader.hpp>
#include <vtkIO/vtk.hpp>

#include "ImageTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ImageTest );

static double epsilon = 0.00001;
//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::testImageToVtk()
{
    const size_t dim = 3;
    ::fwData::Image::SizeType size(dim);
    size[0] = 10;
    size[1] = 15;
    size[2] = 23;
    ::fwData::Image::SpacingType spacing(dim);
    spacing[0] = 0.85;
    spacing[1] = 2.6;
    spacing[2] = 1.87;
    ::fwData::Image::OriginType origin(dim);
    origin[0] = -45.6;
    origin[1] = 25.97;
    origin[2] = -53.9;
    ::fwTools::Type type("uint32");

    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateImage(image, size, spacing, origin, type);

    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::vtkIO::toVTKImage(image, vtkImage);

    CPPUNIT_ASSERT_DOUBLES_EQUAL( spacing[0], vtkImage->GetSpacing()[0], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( spacing[1], vtkImage->GetSpacing()[1], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( spacing[2], vtkImage->GetSpacing()[2], epsilon );

    CPPUNIT_ASSERT_DOUBLES_EQUAL( origin[0], vtkImage->GetOrigin()[0], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( origin[1], vtkImage->GetOrigin()[1], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( origin[2], vtkImage->GetOrigin()[2], epsilon );

    CPPUNIT_ASSERT_DOUBLES_EQUAL( size[0], static_cast< ::fwData::Image::SizeType::value_type >(vtkImage->GetDimensions()[0]), epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( size[1], static_cast< ::fwData::Image::SizeType::value_type >(vtkImage->GetDimensions()[1]), epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( size[2], static_cast< ::fwData::Image::SizeType::value_type >(vtkImage->GetDimensions()[2]), epsilon );

    char *vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char *ptr = static_cast<char*>(image->getBuffer());

    CPPUNIT_ASSERT_EQUAL( static_cast<int>(image->getType().sizeOf()), vtkImage->GetScalarSize());
    
    CPPUNIT_ASSERT( std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr) );

}

//------------------------------------------------------------------------------


void ImageTest::testFromVtk()
{
    const ::boost::filesystem::path image_path( ::fwTest::Data::dir() / "fw4spl/image/vtk/img.vtk" );

    CPPUNIT_ASSERT(::boost::filesystem::exists(image_path));
    CPPUNIT_ASSERT(::boost::filesystem::is_regular_file(image_path));

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName(image_path.string().c_str());
    reader->Update();
    vtkImageData* vtkImage = vtkImageData::SafeDownCast(reader->GetOutput());

    CPPUNIT_ASSERT(vtkImage);

    ::fwData::Image::NewSptr image;
    ::vtkIO::fromVTKImage(vtkImage, image);

    ::fwData::Image::SizeType size = image->getSize();
    ::fwData::Image::SpacingType spacing = image->getSpacing();
    ::fwData::Image::OriginType origin = image->getOrigin();

    CPPUNIT_ASSERT_DOUBLES_EQUAL( vtkImage->GetSpacing()[0], spacing[0], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( vtkImage->GetSpacing()[1], spacing[1], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( vtkImage->GetSpacing()[2], spacing[2], epsilon );

    CPPUNIT_ASSERT_DOUBLES_EQUAL( vtkImage->GetOrigin()[0], origin[0], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( vtkImage->GetOrigin()[1], origin[1], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( vtkImage->GetOrigin()[2], origin[2], epsilon );

    CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::SizeType::value_type >(vtkImage->GetDimensions()[0]), size[0], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::SizeType::value_type >(vtkImage->GetDimensions()[1]), size[1], epsilon );
    CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::SizeType::value_type >(vtkImage->GetDimensions()[2]), size[2], epsilon );


}


