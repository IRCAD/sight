/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ImageTest.hpp"

#include <fwVtkIO/ImageReader.hpp>
#include <fwVtkIO/ImageWriter.hpp>
#include <fwVtkIO/MetaImageReader.hpp>
#include <fwVtkIO/MetaImageWriter.hpp>
#include <fwVtkIO/VtiImageReader.hpp>
#include <fwVtkIO/VtiImageWriter.hpp>
#include <fwVtkIO/vtk.hpp>

#include <core/tools/System.hpp>

#include <data/Image.hpp>

#include <utestData/Data.hpp>
#include <utestData/File.hpp>
#include <utestData/generator/Image.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwVtkIO::ut::ImageTest );

namespace fwVtkIO
{
namespace ut
{

static const double epsilon = 0.00001;

static const data::Image::Size bostonTeapotSize       = {{ 256, 256, 178 }};
static const data::Image::Spacing bostonTeapotSpacing = {{ 1, 1, 1 }};
static const data::Image::Origin bostonTeapotOrigin   = {{ 1.1, 2.2, 3.3 }};

//------------------------------------------------------------------------------

template< typename ExpSizeType, typename ExpSpacingType, typename ExpOriginType, typename ExpDimType,
          typename SizeType, typename SpacingType, typename OriginType, typename DimType>
void compareImageAttributes(const ExpSizeType& expSize,
                            const ExpSpacingType& expSpacing,
                            const ExpOriginType& expOrigin,
                            ExpDimType expDim,
                            const SizeType& size,
                            const SpacingType& spacing,
                            const OriginType& origin,
                            DimType dim)
{
    CPPUNIT_ASSERT_EQUAL( static_cast< size_t >(expDim),
                          static_cast< size_t >(dim) );

    for(size_t i = 0; i < static_cast< size_t >(dim); ++i)
    {

        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< data::Image::Spacing::value_type >(expSpacing[i]),
                                      static_cast< data::Image::Spacing::value_type >(spacing[i]), epsilon );
        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< data::Image::Origin::value_type >(expOrigin[i]),
                                      static_cast< data::Image::Origin::value_type >(origin[i]), epsilon );
        CPPUNIT_ASSERT_EQUAL( static_cast< data::Image::Size::value_type >(expSize[i]),
                              static_cast< data::Image::Size::value_type >(size[i]) );
    }
}

//------------------------------------------------------------------------------

void imageToVTKTest(const std::string& imgtype, const std::set<int>& vtktypes)
{
    const data::Image::Size size       = {10, 15, 23};
    const data::Image::Spacing spacing = {0.85, 2.6, 1.87};
    const data::Image::Origin origin   = {-45.6, 25.97, -53.9};

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(image, size, spacing, origin, core::tools::Type(
                                                   imgtype), data::Image::PixelFormat::GRAY_SCALE);

    const auto dumpLock = image->lock();

    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage(image, vtkImage);

    compareImageAttributes(
        size,
        spacing,
        origin,
        image->getNumberOfDimensions(),

        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension()

        );

    std::set<int> types = vtktypes;
    CPPUNIT_ASSERT_MESSAGE( "Test failed for type " + imgtype, types.find( vtkImage->GetScalarType() ) != types.end() );

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->getBuffer());

    CPPUNIT_ASSERT_MESSAGE( "Test failed for type " + imgtype, std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr) );
}

//------------------------------------------------------------------------------

template<typename W, typename R>
void writerTest(const std::string& imagetype, const std::string& filename)
{
    const std::filesystem::path testFile(core::tools::System::getTemporaryFolder() /
                                         std::filesystem::path(filename));

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::tools::Type(imagetype));

    typename W::sptr writer = W::New();
    writer->setObject(image);
    writer->setFile(testFile);
    writer->write();

    CPPUNIT_ASSERT_MESSAGE( "test on <" + filename + ">  of type <" + imagetype + "> Failed ",
                            std::filesystem::exists(testFile) );

    data::Image::sptr image2 = data::Image::New();
    typename R::sptr reader  = R::New();
    reader->setObject(image2);
    reader->setFile(testFile);
    reader->read();

    std::filesystem::remove(testFile);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" + filename + "> of type <" + imagetype + "> Failed ",
                                  image->getType(), image2->getType() );

    compareImageAttributes(
        image->getSize2(),
        image->getSpacing2(),
        image->getOrigin2(),
        image->getNumberOfDimensions(),

        image2->getSize2(),
        image2->getSpacing2(),
        image2->getOrigin2(),
        image2->getNumberOfDimensions()
        );
}

//------------------------------------------------------------------------------

void imageFromVTKTest(const std::string& imagename, const std::string& type)
{
    const std::filesystem::path imagePath( utestData::Data::dir() /
                                           std::filesystem::path(imagename) );

    CPPUNIT_ASSERT(std::filesystem::exists(imagePath));
    CPPUNIT_ASSERT(std::filesystem::is_regular_file(imagePath));

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName(imagePath.string().c_str());
    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();
    vtkSmartPointer< vtkImageData > vtkImage = vtkImageData::SafeDownCast(reader->GetOutput());

    CPPUNIT_ASSERT(vtkImage);

    data::Image::sptr image = data::Image::New();
    ::fwVtkIO::fromVTKImage(vtkImage, image);

    const auto dumpLock = image->lock();

    compareImageAttributes(
        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension(),

        image->getSize2(),
        image->getSpacing2(),
        image->getOrigin2(),
        image->getNumberOfDimensions()
        );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" + imagename + "> Failed ", core::tools::Type(type), image->getType() );

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->getBuffer());

    CPPUNIT_ASSERT( std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr) );
}

//------------------------------------------------------------------------------

void testVtkReader(std::string imagetype)
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/image/vtk/img-" + imagetype + ".vtk"));

    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           std::filesystem::exists(testFile));

    data::Image::sptr image = data::Image::New();

    ::fwVtkIO::ImageReader::sptr reader = ::fwVtkIO::ImageReader::New();
    reader->setObject(image);
    reader->setFile(testFile);
    reader->read();

    vtkSmartPointer< vtkGenericDataObjectReader > vtkreader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    vtkreader->SetFileName(testFile.string().c_str());
    vtkreader->Update();
    vtkSmartPointer< vtkImageData > vtkImage = vtkImageData::SafeDownCast(vtkreader->GetOutput());
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" "sight/image/vtk/img-" + imagetype + ".vtk" "> Failed ",
                                  core::tools::Type(imagetype), image->getType());

    compareImageAttributes(
        image->getSize2(),
        image->getSpacing2(),
        image->getOrigin2(),
        image->getNumberOfDimensions(),

        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension()
        );
}

// ------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

// ------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

// ------------------------------------------------------------------------------

void ImageTest::testImageToVtk()
{
    imageToVTKTest("int8", { VTK_CHAR, VTK_SIGNED_CHAR });
    imageToVTKTest("uint8", { VTK_UNSIGNED_CHAR});

    imageToVTKTest("int16", { VTK_SHORT });
    imageToVTKTest("uint16", { VTK_UNSIGNED_SHORT });

    imageToVTKTest("int32", { VTK_INT });
    imageToVTKTest("uint32", { VTK_UNSIGNED_INT });

    // imageToVTKTest("int64" , { VTK_LONG));
    // imageToVTKTest("uint64", { VTK_UNSIGNED_LONG));

    imageToVTKTest("float", { VTK_FLOAT });
    imageToVTKTest("double", { VTK_DOUBLE });

}

// ------------------------------------------------------------------------------

void ImageTest::testFromVtk()
{
    imageFromVTKTest("sight/image/vtk/img.vtk", "int16");

    imageFromVTKTest("sight/image/vtk/img-int8.vtk", "int8");
    imageFromVTKTest("sight/image/vtk/img-uint8.vtk", "uint8");

    imageFromVTKTest("sight/image/vtk/img-int16.vtk", "int16"  );
    imageFromVTKTest("sight/image/vtk/img-uint16.vtk", "uint16"  );

    imageFromVTKTest("sight/image/vtk/img-int32.vtk", "int32"  );
    imageFromVTKTest("sight/image/vtk/img-uint32.vtk", "uint32"  );

    //imageFromVTKTest("sight/image/vtk/img-int64.vtk", "int64"  );
    //imageFromVTKTest("sight/image/vtk/img-uint64.vtk", "uint64"  );

    imageFromVTKTest("sight/image/vtk/img-float.vtk", "float"  );
    imageFromVTKTest("sight/image/vtk/img-double.vtk", "double"  );

    int nbComponents = 4;
    std::string type = "uint8";

    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();

    CPPUNIT_ASSERT(vtkImage);
    vtkImage->SetDimensions(64, 64, 1);
    vtkImage->SetSpacing(1.0, 1.0, 0.0);
    int dataType = ::fwVtkIO::TypeTranslator::translate(core::tools::Type::create(type));
    vtkImage->AllocateScalars(dataType, nbComponents);

    data::Image::sptr image = data::Image::New();
    ::fwVtkIO::fromVTKImage(vtkImage, image);

    const auto dumpLock = image->lock();
    CPPUNIT_ASSERT_EQUAL( static_cast< size_t >(vtkImage->GetPointData()->GetScalars()->GetNumberOfComponents()),
                          static_cast< size_t >( image->getNumberOfComponents()) );
    compareImageAttributes(
        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension(),

        image->getSize2(),
        image->getSpacing2(),
        image->getOrigin2(),
        image->getNumberOfDimensions()
        );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" + type + "> Failed ", core::tools::Type(type), image->getType() );

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->getBuffer());

    CPPUNIT_ASSERT( std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr) );
}

// ------------------------------------------------------------------------------

template<typename TYPE>
void fromToTest(data::Image::PixelFormat format)
{
    const data::Image::Size size       = {10, 20, 0};
    const data::Image::Spacing spacing = {1., 1., 0};
    const data::Image::Origin origin   = {0., 0., 0.};
    const core::tools::Type type       = core::tools::Type::create<TYPE>();

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(image, size, spacing, origin, type, format);
    utestData::generator::Image::randomizeImage(image);

    vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage(image, vtkImage);

    data::Image::sptr image2 = data::Image::New();
    ::fwVtkIO::fromVTKImage(vtkImage, image2);

    CPPUNIT_ASSERT_EQUAL( image->getSize2()[0], image2->getSize2()[0] );
    CPPUNIT_ASSERT_EQUAL( image->getSize2()[1], image2->getSize2()[1] );
    CPPUNIT_ASSERT_EQUAL( image->getSize2()[2], image2->getSize2()[2] );
    CPPUNIT_ASSERT_EQUAL( image->getType(), image2->getType());
    CPPUNIT_ASSERT_EQUAL( image->getNumberOfComponents(), image2->getNumberOfComponents() );
    CPPUNIT_ASSERT_EQUAL( image->getPixelFormat(), image2->getPixelFormat() );

    const auto imageDumpLock  = image->lock();
    const auto image2DumpLock = image2->lock();

    auto itr       = image->begin< TYPE >();
    auto itr2      = image2->begin< TYPE >();
    const auto end = image->end< TYPE >();

    size_t count = 0;
    for (; itr != end; ++itr, ++itr2, ++count)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("[" + type.string() + "] pixel[" + std::to_string(count) + "]", *itr, *itr2);
    }
}

// ------------------------------------------------------------------------------

void ImageTest::fromToVtkTest()
{
    fromToTest<std::uint8_t>(data::Image::GRAY_SCALE);
    fromToTest<std::uint8_t>(data::Image::RGB);
    fromToTest<std::uint8_t>(data::Image::RGBA);

    fromToTest<std::uint16_t>(data::Image::GRAY_SCALE);
    fromToTest<std::uint16_t>(data::Image::RGB);
    fromToTest<std::uint16_t>(data::Image::RGBA);

    fromToTest<std::uint32_t>(data::Image::GRAY_SCALE);
    fromToTest<std::uint32_t>(data::Image::RGB);
    fromToTest<std::uint32_t>(data::Image::RGBA);

    fromToTest<std::int8_t>(data::Image::GRAY_SCALE);
    fromToTest<std::int8_t>(data::Image::RGB);
    fromToTest<std::int8_t>(data::Image::RGBA);

    fromToTest<std::int16_t>(data::Image::GRAY_SCALE);
    fromToTest<std::int16_t>(data::Image::RGB);
    fromToTest<std::int16_t>(data::Image::RGBA);

    fromToTest<std::int32_t>(data::Image::GRAY_SCALE);
    fromToTest<std::int32_t>(data::Image::RGB);
    fromToTest<std::int32_t>(data::Image::RGBA);

    // uint64 and int64 are not recognized on Windows for VTK conversion.
}

// ------------------------------------------------------------------------------

void ImageTest::mhdReaderTest()
{
    const std::filesystem::path imagePath( utestData::Data::dir() / "sight/image/mhd/BostonTeapot.mhd" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           std::filesystem::exists(imagePath));

    data::Image::sptr image = data::Image::New();
    ::fwVtkIO::MetaImageReader::sptr reader = ::fwVtkIO::MetaImageReader::New();
    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    compareImageAttributes(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,
        bostonTeapotSize.size(),

        image->getSize2(),
        image->getSpacing2(),
        image->getOrigin2(),
        image->getNumberOfDimensions()
        );

}

// ------------------------------------------------------------------------------

void ImageTest::mhdWriterTest()
{
    const std::filesystem::path imagePath( utestData::Data::dir() / "sight/image/mhd/BostonTeapot.mhd" );
    const std::filesystem::path zRawPath( utestData::Data::dir() / "sight/image/mhd/BostonTeapot.zraw" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           std::filesystem::exists(imagePath));

    CPPUNIT_ASSERT_MESSAGE("The file '" + zRawPath.string() + "' does not exist",
                           std::filesystem::exists(zRawPath));

    const std::filesystem::path testFile(core::tools::System::getTemporaryFolder() / "BostonTeapot.mhd");
    const std::filesystem::path testZRawFile(core::tools::System::getTemporaryFolder() / "BostonTeapot.zraw");

    data::Image::sptr image = data::Image::New();
    ::fwVtkIO::MetaImageReader::sptr reader = ::fwVtkIO::MetaImageReader::New();
    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    ::fwVtkIO::MetaImageWriter::sptr writer = ::fwVtkIO::MetaImageWriter::New();
    writer->setObject(image);
    writer->setFile(testFile);
    writer->write();

    CPPUNIT_ASSERT( std::filesystem::exists(testFile) );
    CPPUNIT_ASSERT( std::filesystem::exists(testZRawFile) );

    CPPUNIT_ASSERT( utestData::File::contentEquals(imagePath, testFile) );
    CPPUNIT_ASSERT( utestData::File::contentEquals(zRawPath, testZRawFile) );

    std::filesystem::remove(testFile);
    std::filesystem::remove(testZRawFile);

    writerTest< ::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader>("int8", "imageTest.mhd");
    writerTest< ::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader>("uint8", "imageTest.mhd");
    writerTest< ::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader>("int16", "imageTest.mhd");
    writerTest< ::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader>("uint16", "imageTest.mhd");
    writerTest< ::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader>("int32", "imageTest.mhd");
    writerTest< ::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader>("uint32", "imageTest.mhd");
    // writerTest< ::fwVtkIO::MetaImageWriter,::fwVtkIO::MetaImageReader>("int64", "imageTest.mhd");
    // writerTest< ::fwVtkIO::MetaImageWriter,::fwVtkIO::MetaImageReader>("uint64", "imageTest.mhd");

    const std::filesystem::path zFile(core::tools::System::getTemporaryFolder() / "imagetestfile.zraw");
    std::filesystem::remove(zFile);
}

// ------------------------------------------------------------------------------

void ImageTest::vtiReaderTest()
{
    const std::filesystem::path imagePath( utestData::Data::dir() / "sight/image/vti/BostonTeapot.vti" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           std::filesystem::exists(imagePath));

    data::Image::sptr image = data::Image::New();
    ::fwVtkIO::VtiImageReader::sptr reader = ::fwVtkIO::VtiImageReader::New();

    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    compareImageAttributes(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,
        bostonTeapotSize.size(),

        image->getSize2(),
        image->getSpacing2(),
        image->getOrigin2(),
        image->getNumberOfDimensions()
        );

}

// ------------------------------------------------------------------------------

void ImageTest::vtiWriterTest()
{

    writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>( "int8", "imageTest.vti");
    writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>( "uint8", "imageTest.vti");
    writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>( "int16", "imageTest.vti");
    writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>( "uint16", "imageTest.vti");
    writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>( "int32", "imageTest.vti");
    writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>( "uint32", "imageTest.vti");
    // writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>("int64", "imageTest.vti");
    // writerTest< ::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader>("uint64", "imageTest.vti");
}

// ------------------------------------------------------------------------------

void ImageTest::vtkReaderTest()
{
    const std::filesystem::path imagePath( utestData::Data::dir() / "sight/image/vtk/img.vtk" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           std::filesystem::exists(imagePath));

    data::Image::sptr image = data::Image::New();
    ::fwVtkIO::ImageReader::sptr reader = ::fwVtkIO::ImageReader::New();

    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    data::Image::Size vtkSize {{ 230, 170, 58 }};
    data::Image::Spacing vtkSpacing {{ 1.732, 1.732, 3.2 }};
    data::Image::Origin vtkOrigin {{ 34.64, 86.6, 56 }};

    compareImageAttributes(
        vtkSize,
        vtkSpacing,
        vtkOrigin,
        3,

        image->getSize2(),
        image->getSpacing2(),
        image->getOrigin2(),
        image->getNumberOfDimensions()
        );

    testVtkReader(std::string("int8"));
    testVtkReader(std::string("uint8"));
    testVtkReader(std::string("int16"));
    testVtkReader(std::string("uint16"));
    testVtkReader(std::string("int32"));
    testVtkReader(std::string("uint32"));
    // testVtkReader(std::string("int64"));
    // testVtkReader(std::string("uint64"));
    testVtkReader(std::string("float"));
    testVtkReader(std::string("double"));
}

// ------------------------------------------------------------------------------

void ImageTest::vtkWriterTest()
{
    writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "int8", "imageTest.vtk");
    writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "uint8", "imageTest.vtk");
    writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "int16", "imageTest.vtk");
    writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "uint16", "imageTest.vtk");
    writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "int32", "imageTest.vtk");
    writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "uint32", "imageTest.vtk");
    // writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "int64", "imageTest.vtk");
    // writerTest< ::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader>( "uint64", "imageTest.vtk");
}

// ------------------------------------------------------------------------------

} // namespace ut
} // namespace fwVtkIO
