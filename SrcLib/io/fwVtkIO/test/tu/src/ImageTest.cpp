/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageTest.hpp"

#include <fwVtkIO/ImageReader.hpp>
#include <fwVtkIO/ImageWriter.hpp>
#include <fwVtkIO/MetaImageReader.hpp>
#include <fwVtkIO/MetaImageWriter.hpp>
#include <fwVtkIO/VtiImageReader.hpp>
#include <fwVtkIO/VtiImageWriter.hpp>
#include <fwVtkIO/vtk.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/File.hpp>
#include <fwTest/generator/Image.hpp>

#include <fwTools/System.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwVtkIO::ut::ImageTest );

namespace fwVtkIO
{
namespace ut
{

using namespace boost::assign;

static const double epsilon       = 0.00001;
static const int nbDataTestValues = 1000;

static const ::fwData::Image::SizeType bostonTeapotSize       = list_of(256)(256)(178);
static const ::fwData::Image::SpacingType bostonTeapotSpacing = list_of(1)(1)(1);
static const ::fwData::Image::OriginType bostonTeapotOrigin   = list_of(1.1)(2.2)(3.3);

#define COMPARE_IMAGE_ATTRS_MACRO(expSize, expSpacing, expOrigin, size, spacing, origin)                               \
    {                                                                                                                  \
        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::SpacingType::value_type >(expSpacing[0]),          \
                                      static_cast< ::fwData::Image::SpacingType::value_type >(spacing[0]), epsilon );  \
        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::SpacingType::value_type >(expSpacing[1]),          \
                                      static_cast< ::fwData::Image::SpacingType::value_type >(spacing[1]), epsilon );  \
        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::SpacingType::value_type >(expSpacing[2]),          \
                                      static_cast< ::fwData::Image::SpacingType::value_type >(spacing[2]), epsilon );  \
        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::OriginType::value_type >(expOrigin[0]),            \
                                      static_cast< ::fwData::Image::OriginType::value_type >(origin[0]), epsilon );    \
        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::OriginType::value_type >(expOrigin[1]),            \
                                      static_cast< ::fwData::Image::OriginType::value_type >(origin[1]), epsilon );    \
        CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast< ::fwData::Image::OriginType::value_type >(expOrigin[2]),            \
                                      static_cast< ::fwData::Image::OriginType::value_type >(origin[2]), epsilon );    \
                                                                                                                       \
        CPPUNIT_ASSERT_EQUAL( static_cast< ::fwData::Image::SizeType::value_type >(expSize[0]),                        \
                              static_cast< ::fwData::Image::SizeType::value_type >(size[0]) );                         \
        CPPUNIT_ASSERT_EQUAL( static_cast< ::fwData::Image::SizeType::value_type >(expSize[1]),                        \
                              static_cast< ::fwData::Image::SizeType::value_type >(size[1]) );                         \
        CPPUNIT_ASSERT_EQUAL( static_cast< ::fwData::Image::SizeType::value_type >(expSize[2]),                        \
                              static_cast< ::fwData::Image::SizeType::value_type >(size[2]) );                         \
    }

#define COMPARE_IMAGE_FULL_ATTRS_MACRO(expSize, expSpacing, expOrigin, expNbComponents, size, spacing, origin,         \
                                       nbComponents)                                                                   \
    {                                                                                                                  \
        COMPARE_IMAGE_ATTRS_MACRO(expSize, expSpacing, expOrigin, size, spacing, origin)                               \
        CPPUNIT_ASSERT_EQUAL( static_cast< size_t >(expNbComponents), static_cast< size_t >( nbComponents) );          \
    }

#define WRITER_TEST(writerclass, readerclass, imagetype, filename)                                                     \
    {                                                                                                                  \
        const ::boost::filesystem::path testFile(::fwTools::System::getTemporaryFolder() / filename);                  \
                                                                                                                       \
        ::fwData::Image::sptr image = ::fwData::Image::New();                                                          \
        ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type(imagetype));                            \
                                                                                                                       \
        writerclass::sptr writer = writerclass::New();                                                                 \
        writer->setObject(image);                                                                                      \
        writer->setFile(testFile);                                                                                     \
        writer->write();                                                                                               \
                                                                                                                       \
        CPPUNIT_ASSERT_MESSAGE( "test on <" filename ">  of type <" imagetype "> Failed ",                             \
                                ::boost::filesystem::exists(testFile) );                                               \
                                                                                                                       \
        ::fwData::Image::sptr image2 = ::fwData::Image::New();                                                         \
        readerclass::sptr reader = readerclass::New();                                                                 \
        reader->setObject(image2);                                                                                     \
        reader->setFile(testFile);                                                                                     \
        reader->read();                                                                                                \
                                                                                                                       \
        ::boost::filesystem::remove(testFile);                                                                         \
                                                                                                                       \
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" filename "> of type <" imagetype "> Failed ",                        \
                                      image->getType(), image2->getType() );                                           \
        COMPARE_IMAGE_ATTRS_MACRO(                                                                                     \
            image->getSize(),                                                                                          \
            image->getSpacing(),                                                                                       \
            image->getOrigin(),                                                                                        \
            image2->getSize(),                                                                                         \
            image2->getSpacing(),                                                                                      \
            image2->getOrigin()                                                                                        \
            )                                                                                                          \
    }

// ------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    srand(static_cast<unsigned int>(time(NULL)));
}

// ------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

// ------------------------------------------------------------------------------

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

    #define IMAGE_TO_VTK_TEST(imgtype, vtktypes)                                                                       \
    {                                                                                                                  \
        ::fwData::Image::sptr image = ::fwData::Image::New();                                                          \
        ::fwTest::generator::Image::generateImage(image, size, spacing, origin, ::fwTools::Type(imgtype));             \
                                                                                                                       \
        ::fwDataTools::helper::Image imageHelper(image);                                                               \
                                                                                                                       \
        vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();                             \
        ::fwVtkIO::toVTKImage(image, vtkImage);                                                                        \
                                                                                                                       \
        COMPARE_IMAGE_ATTRS_MACRO(                                                                                     \
            size,                                                                                                      \
            spacing,                                                                                                   \
            origin,                                                                                                    \
                                                                                                                       \
            vtkImage->GetDimensions(),                                                                                 \
            vtkImage->GetSpacing(),                                                                                    \
            vtkImage->GetOrigin()                                                                                      \
            );                                                                                                         \
                                                                                                                       \
        std::set<int> types = vtktypes;                                                                                \
        CPPUNIT_ASSERT_MESSAGE( "Test failed for type " imgtype,                                                       \
                                types.find( vtkImage->GetScalarType() ) != types.end() );                              \
                                                                                                                       \
        char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());                                               \
        char* ptr    = static_cast<char*>(imageHelper.getBuffer());                                                    \
                                                                                                                       \
        CPPUNIT_ASSERT_MESSAGE( "Test failed for type " imgtype,                                                       \
                                std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr) );                              \
    }

    IMAGE_TO_VTK_TEST("int8", list_of(VTK_CHAR)(VTK_SIGNED_CHAR));
    IMAGE_TO_VTK_TEST("uint8", list_of(VTK_UNSIGNED_CHAR));

    IMAGE_TO_VTK_TEST("int16", list_of(VTK_SHORT));
    IMAGE_TO_VTK_TEST("uint16", list_of(VTK_UNSIGNED_SHORT));

    IMAGE_TO_VTK_TEST("int32", list_of(VTK_INT));
    IMAGE_TO_VTK_TEST("uint32", list_of(VTK_UNSIGNED_INT));

    // IMAGE_TO_VTK_TEST("int64" , list_of(VTK_LONG));
    // IMAGE_TO_VTK_TEST("uint64", list_of(VTK_UNSIGNED_LONG));

    IMAGE_TO_VTK_TEST("float", list_of(VTK_FLOAT));
    IMAGE_TO_VTK_TEST("double", list_of(VTK_DOUBLE));

}

// ------------------------------------------------------------------------------

void ImageTest::testFromVtk()
{
    #define IMAGE_FROM_VTK_TEST(imagename, type)                                                                       \
    {                                                                                                                  \
        const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / imagename );                                \
                                                                                                                       \
        CPPUNIT_ASSERT(::boost::filesystem::exists(imagePath));                                                        \
        CPPUNIT_ASSERT(::boost::filesystem::is_regular_file(imagePath));                                               \
                                                                                                                       \
        vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();   \
        reader->SetFileName(imagePath.string().c_str());                                                               \
        reader->Update();                                                                                              \
        reader->UpdateInformation();                                                                                   \
        reader->PropagateUpdateExtent();                                                                               \
        vtkSmartPointer< vtkImageData > vtkImage = vtkImageData::SafeDownCast(reader->GetOutput());                    \
                                                                                                                       \
        CPPUNIT_ASSERT(vtkImage);                                                                                      \
                                                                                                                       \
        ::fwData::Image::sptr image = ::fwData::Image::New();                                                          \
        ::fwVtkIO::fromVTKImage(vtkImage, image);                                                                      \
                                                                                                                       \
        ::fwDataTools::helper::Image imageHelper(image);                                                               \
                                                                                                                       \
        COMPARE_IMAGE_ATTRS_MACRO(                                                                                     \
            vtkImage->GetDimensions(),                                                                                 \
            vtkImage->GetSpacing(),                                                                                    \
            vtkImage->GetOrigin(),                                                                                     \
                                                                                                                       \
            image->getSize(),                                                                                          \
            image->getSpacing(),                                                                                       \
            image->getOrigin()                                                                                         \
            );                                                                                                         \
                                                                                                                       \
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" imagename "> Failed ", ::fwTools::Type(type), image->getType() );    \
                                                                                                                       \
        char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());                                               \
        char* ptr    = static_cast<char*>(imageHelper.getBuffer());                                                    \
                                                                                                                       \
        CPPUNIT_ASSERT( std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr) );                                      \
    }

    #define IMAGE_FROM_VTK_GEN_TEST(nbComponents, type)                                                                \
    {                                                                                                                  \
        vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();                             \
                                                                                                                       \
        CPPUNIT_ASSERT(vtkImage);                                                                                      \
        vtkImage->SetDimensions(64, 64, 2);                                                                            \
        vtkImage->SetSpacing(1.0, 1.0, 0.0);                                                                           \
        int dataType = ::fwVtkIO::TypeTranslator::translate(::fwTools::Type::create(type));                            \
        vtkImage->AllocateScalars(dataType, nbComponents);                                                             \
                                                                                                                       \
        ::fwData::Image::sptr image = ::fwData::Image::New();                                                          \
        ::fwVtkIO::fromVTKImage(vtkImage, image);                                                                      \
                                                                                                                       \
        ::fwDataTools::helper::Image imageHelper(image);                                                               \
        COMPARE_IMAGE_FULL_ATTRS_MACRO(                                                                                \
            vtkImage->GetDimensions(),                                                                                 \
            vtkImage->GetSpacing(),                                                                                    \
            vtkImage->GetOrigin(),                                                                                     \
            vtkImage->GetPointData()->GetScalars()->GetNumberOfComponents(),                                           \
                                                                                                                       \
            image->getSize(),                                                                                          \
            image->getSpacing(),                                                                                       \
            image->getOrigin(),                                                                                        \
            image->getNumberOfComponents()                                                                             \
            );                                                                                                         \
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" type "> Failed ", ::fwTools::Type(type), image->getType() );         \
                                                                                                                       \
        char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());                                               \
        char* ptr    = static_cast<char*>(imageHelper.getBuffer());                                                    \
                                                                                                                       \
        CPPUNIT_ASSERT( std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr) );                                      \
    }

#define GENERATED_IMAGE_FROM_VTK_TEST(type)                                                                            \
    IMAGE_FROM_VTK_TEST("fw4spl/image/vtk/img-" type ".vtk", type)

    IMAGE_FROM_VTK_TEST("fw4spl/image/vtk/img.vtk", "int16");

    GENERATED_IMAGE_FROM_VTK_TEST("int8"  );
    GENERATED_IMAGE_FROM_VTK_TEST("uint8" );

    GENERATED_IMAGE_FROM_VTK_TEST("int16" );
    GENERATED_IMAGE_FROM_VTK_TEST("uint16");

    GENERATED_IMAGE_FROM_VTK_TEST("int32" );
    GENERATED_IMAGE_FROM_VTK_TEST("uint32");

    // GENERATED_IMAGE_FROM_VTK_TEST("int64" );
    // GENERATED_IMAGE_FROM_VTK_TEST("uint64");

    GENERATED_IMAGE_FROM_VTK_TEST("float" );
    GENERATED_IMAGE_FROM_VTK_TEST("double");

    IMAGE_FROM_VTK_GEN_TEST(4, "uint8");
}

// ------------------------------------------------------------------------------

void ImageTest::mhdReaderTest()
{
    const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / "fw4spl/image/mhd/BostonTeapot.mhd" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           ::boost::filesystem::exists(imagePath));

    ::fwData::Image::sptr image             = ::fwData::Image::New();
    ::fwVtkIO::MetaImageReader::sptr reader = ::fwVtkIO::MetaImageReader::New();
    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    COMPARE_IMAGE_ATTRS_MACRO(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,

        image->getSize(),
        image->getSpacing(),
        image->getOrigin()
        );

}

// ------------------------------------------------------------------------------

void ImageTest::mhdWriterTest()
{
    const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / "fw4spl/image/mhd/BostonTeapot.mhd" );
    const ::boost::filesystem::path zRawPath( ::fwTest::Data::dir() / "fw4spl/image/mhd/BostonTeapot.zraw" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           ::boost::filesystem::exists(imagePath));

    CPPUNIT_ASSERT_MESSAGE("The file '" + zRawPath.string() + "' does not exist",
                           ::boost::filesystem::exists(zRawPath));

    const ::boost::filesystem::path testFile(::fwTools::System::getTemporaryFolder() / "BostonTeapot.mhd");
    const ::boost::filesystem::path testZRawFile(::fwTools::System::getTemporaryFolder() / "BostonTeapot.zraw");

    ::fwData::Image::sptr image             = ::fwData::Image::New();
    ::fwVtkIO::MetaImageReader::sptr reader = ::fwVtkIO::MetaImageReader::New();
    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    ::fwVtkIO::MetaImageWriter::sptr writer = ::fwVtkIO::MetaImageWriter::New();
    writer->setObject(image);
    writer->setFile(testFile);
    writer->write();

    CPPUNIT_ASSERT( ::boost::filesystem::exists(testFile) );
    CPPUNIT_ASSERT( ::boost::filesystem::exists(testZRawFile) );

    CPPUNIT_ASSERT( ::fwTest::File::contentEquals(imagePath, testFile) );
    CPPUNIT_ASSERT( ::fwTest::File::contentEquals(zRawPath, testZRawFile) );

    ::boost::filesystem::remove(testFile);
    ::boost::filesystem::remove(testZRawFile);

    WRITER_TEST(::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader, "int8", "imageTest.mhd");
    WRITER_TEST(::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader, "uint8", "imageTest.mhd");
    WRITER_TEST(::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader, "int16", "imageTest.mhd");
    WRITER_TEST(::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader, "uint16", "imageTest.mhd");
    WRITER_TEST(::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader, "int32", "imageTest.mhd");
    WRITER_TEST(::fwVtkIO::MetaImageWriter, ::fwVtkIO::MetaImageReader, "uint32", "imageTest.mhd");
    // WRITER_TEST(::fwVtkIO::MetaImageWriter,::fwVtkIO::MetaImageReader,"int64", "imageTest.mhd");
    // WRITER_TEST(::fwVtkIO::MetaImageWriter,::fwVtkIO::MetaImageReader,"uint64", "imageTest.mhd");

    const ::boost::filesystem::path zFile(::fwTools::System::getTemporaryFolder() / "imagetestfile.zraw");
    ::boost::filesystem::remove(zFile);
}

// ------------------------------------------------------------------------------

void ImageTest::vtiReaderTest()
{
    const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / "fw4spl/image/vti/BostonTeapot.vti" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           ::boost::filesystem::exists(imagePath));

    ::fwData::Image::sptr image            = ::fwData::Image::New();
    ::fwVtkIO::VtiImageReader::sptr reader = ::fwVtkIO::VtiImageReader::New();

    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    COMPARE_IMAGE_ATTRS_MACRO(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,

        image->getSize(),
        image->getSpacing(),
        image->getOrigin()
        );

}

// ------------------------------------------------------------------------------

void ImageTest::vtiWriterTest()
{

    WRITER_TEST(::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader, "int8", "imageTest.vti");
    WRITER_TEST(::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader, "uint8", "imageTest.vti");
    WRITER_TEST(::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader, "int16", "imageTest.vti");
    WRITER_TEST(::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader, "uint16", "imageTest.vti");
    WRITER_TEST(::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader, "int32", "imageTest.vti");
    WRITER_TEST(::fwVtkIO::VtiImageWriter, ::fwVtkIO::VtiImageReader, "uint32", "imageTest.vti");
    // WRITER_TEST(::fwVtkIO::VtiImageWriter,::fwVtkIO::VtiImageReader,"int64", "imageTest.vti");
    // WRITER_TEST(::fwVtkIO::VtiImageWriter,::fwVtkIO::VtiImageReader,"uint64", "imageTest.vti");
}

// ------------------------------------------------------------------------------

void ImageTest::vtkReaderTest()
{
    const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / "fw4spl/image/vtk/img.vtk" );

    CPPUNIT_ASSERT_MESSAGE("The file '" + imagePath.string() + "' does not exist",
                           ::boost::filesystem::exists(imagePath));

    ::fwData::Image::sptr image         = ::fwData::Image::New();
    ::fwVtkIO::ImageReader::sptr reader = ::fwVtkIO::ImageReader::New();

    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    ::fwData::Image::SizeType vtkSize;
    vtkSize += 230, 170, 58;

    ::fwData::Image::SpacingType vtkSpacing;
    vtkSpacing += 1.732, 1.732, 3.2;

    ::fwData::Image::OriginType vtkOrigin;
    vtkOrigin += 34.64, 86.6, 56;

    COMPARE_IMAGE_ATTRS_MACRO(
        vtkSize,
        vtkSpacing,
        vtkOrigin,

        image->getSize(),
        image->getSpacing(),
        image->getOrigin()
        );

    #define VTK_READER_TEST(imagetype)                                                                                 \
    {                                                                                                                  \
        const ::boost::filesystem::path testFile(::fwTest::Data::dir() / "fw4spl/image/vtk/img-" imagetype ".vtk");    \
                                                                                                                       \
        CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",                                  \
                               ::boost::filesystem::exists(testFile));                                                 \
                                                                                                                       \
        ::fwData::Image::sptr image = ::fwData::Image::New();                                                          \
                                                                                                                       \
        ::fwVtkIO::ImageReader::sptr reader = ::fwVtkIO::ImageReader::New();                                           \
        reader->setObject(image);                                                                                      \
        reader->setFile(testFile);                                                                                     \
        reader->read();                                                                                                \
                                                                                                                       \
        vtkSmartPointer< vtkGenericDataObjectReader > vtkreader = vtkSmartPointer< vtkGenericDataObjectReader >::New(); \
        vtkreader->SetFileName(testFile.string().c_str());                                                             \
        vtkreader->Update();                                                                                           \
        vtkSmartPointer< vtkImageData > vtkImage = vtkImageData::SafeDownCast(vtkreader->GetOutput());                 \
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "test on <" "fw4spl/image/vtk/img-" imagetype ".vtk" "> Failed ",                \
                                      ::fwTools::Type(imagetype), image->getType());                                   \
                                                                                                                       \
        COMPARE_IMAGE_ATTRS_MACRO(                                                                                     \
            image->getSize(),                                                                                          \
            image->getSpacing(),                                                                                       \
            image->getOrigin(),                                                                                        \
                                                                                                                       \
            vtkImage->GetDimensions(),                                                                                 \
            vtkImage->GetSpacing(),                                                                                    \
            vtkImage->GetOrigin()                                                                                      \
            );                                                                                                         \
    }

    VTK_READER_TEST("int8"  );
    VTK_READER_TEST("uint8" );
    VTK_READER_TEST("int16" );
    VTK_READER_TEST("uint16");
    VTK_READER_TEST("int32" );
    VTK_READER_TEST("uint32");
    // VTK_READER_TEST("int64" );
    // VTK_READER_TEST("uint64");
    VTK_READER_TEST("float" );
    VTK_READER_TEST("double");
}

// ------------------------------------------------------------------------------

void ImageTest::vtkWriterTest()
{
    WRITER_TEST(::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader, "int8", "imageTest.vtk");
    WRITER_TEST(::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader, "uint8", "imageTest.vtk");
    WRITER_TEST(::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader, "int16", "imageTest.vtk");
    WRITER_TEST(::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader, "uint16", "imageTest.vtk");
    WRITER_TEST(::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader, "int32", "imageTest.vtk");
    WRITER_TEST(::fwVtkIO::ImageWriter, ::fwVtkIO::ImageReader, "uint32", "imageTest.vtk");
    // WRITER_TEST(::fwVtkIO::ImageWriter,::fwVtkIO::ImageReader,"int64", "imageTest.vtk");
    // WRITER_TEST(::fwVtkIO::ImageWriter,::fwVtkIO::ImageReader,"uint64", "imageTest.vtk");
}

// ------------------------------------------------------------------------------

} // namespace ut
} // namespace fwVtkIO

