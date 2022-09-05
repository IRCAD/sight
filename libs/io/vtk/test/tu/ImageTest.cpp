/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/tools/System.hpp>

#include <data/Image.hpp>

#include <io/vtk/ImageReader.hpp>
#include <io/vtk/ImageWriter.hpp>
#include <io/vtk/MetaImageReader.hpp>
#include <io/vtk/MetaImageWriter.hpp>
#include <io/vtk/VtiImageReader.hpp>
#include <io/vtk/VtiImageWriter.hpp>
#include <io/vtk/vtk.hpp>

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
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::ImageTest);

namespace sight::io::vtk::ut
{

static const double epsilon = 0.00001;

static const data::Image::Size bostonTeapotSize       = {{256, 256, 178}};
static const data::Image::Spacing bostonTeapotSpacing = {{1, 1, 1}};
static const data::Image::Origin bostonTeapotOrigin   = {{1.1, 2.2, 3.3}};

//------------------------------------------------------------------------------

template<typename ExpSizeType, typename ExpSpacingType, typename ExpOriginType, typename ExpDimType,
         typename SizeType, typename SpacingType, typename OriginType, typename DimType>
void compareImageAttributes(
    const ExpSizeType& expSize,
    const ExpSpacingType& expSpacing,
    const ExpOriginType& expOrigin,
    ExpDimType expDim,
    const SizeType& size,
    const SpacingType& spacing,
    const OriginType& origin,
    DimType dim
)
{
    CPPUNIT_ASSERT_EQUAL(
        static_cast<std::size_t>(expDim),
        static_cast<std::size_t>(dim)
    );

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(dim) ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::Image::Spacing::value_type>(expSpacing[i]),
            static_cast<data::Image::Spacing::value_type>(spacing[i]),
            epsilon
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::Image::Origin::value_type>(expOrigin[i]),
            static_cast<data::Image::Origin::value_type>(origin[i]),
            epsilon
        );
        CPPUNIT_ASSERT_EQUAL(
            static_cast<data::Image::Size::value_type>(expSize[i]),
            static_cast<data::Image::Size::value_type>(size[i])
        );
    }
}

//------------------------------------------------------------------------------

void imageToVTKTest(const core::Type imgtype, const std::set<int>& vtk_types)
{
    const data::Image::Size size       = {10, 15, 23};
    const data::Image::Spacing spacing = {0.85, 2.6, 1.87};
    const data::Image::Origin origin   = {-45.6, 25.97, -53.9};

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(
        image,
        size,
        spacing,
        origin,
        imgtype,
        data::Image::PixelFormat::GRAY_SCALE
    );

    const auto dumpLock = image->dump_lock();

    vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(image, vtkImage);

    compareImageAttributes(
        size,
        spacing,
        origin,
        image->numDimensions(),

        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension()

    );

    std::set<int> types = vtk_types;
    CPPUNIT_ASSERT_MESSAGE(
        "Test failed for type " + imgtype.name(),
        types.find(vtkImage->GetScalarType()) != types.end()
    );

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->getBuffer());

    CPPUNIT_ASSERT_MESSAGE(
        "Test failed for type " + imgtype.name(),
        std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr)
    );
}

//------------------------------------------------------------------------------

template<typename W, typename R>
void writerTest(const core::Type imagetype, const std::string& filename)
{
    const std::filesystem::path testFile(core::tools::System::getTemporaryFolder()
                                         / std::filesystem::path(filename));

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, imagetype);

    typename W::sptr writer = W::New();
    writer->setObject(image);
    writer->setFile(testFile);
    writer->write();

    CPPUNIT_ASSERT_MESSAGE(
        "test on <" + filename + ">  of type <" + imagetype.name() + "> Failed ",
        std::filesystem::exists(testFile)
    );

    data::Image::sptr image2 = data::Image::New();
    typename R::sptr reader  = R::New();
    reader->setObject(image2);
    reader->setFile(testFile);
    reader->read();

    std::filesystem::remove(testFile);

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "test on <" + filename + "> of type <" + imagetype.name() + "> Failed ",
        image->getType(),
        image2->getType()
    );

    compareImageAttributes(
        image->getSize(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions(),

        image2->getSize(),
        image2->getSpacing(),
        image2->getOrigin(),
        image2->numDimensions()
    );
}

//------------------------------------------------------------------------------

void imageFromVTKTest(const std::string& imagename, const core::Type& type)
{
    const std::filesystem::path imagePath(utestData::Data::dir()
                                          / std::filesystem::path(imagename));

    CPPUNIT_ASSERT(std::filesystem::exists(imagePath));
    CPPUNIT_ASSERT(std::filesystem::is_regular_file(imagePath));

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(imagePath.string().c_str());
    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();
    vtkSmartPointer<vtkImageData> vtkImage = vtkImageData::SafeDownCast(reader->GetOutput());

    CPPUNIT_ASSERT(vtkImage);

    data::Image::sptr image = data::Image::New();
    io::vtk::fromVTKImage(vtkImage, image);

    const auto dumpLock = image->dump_lock();

    compareImageAttributes(
        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension(),

        image->getSize(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + imagename + "> Failed ", type, image->getType());

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->getBuffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr));
}

//------------------------------------------------------------------------------

void testVtkReader(core::Type imagetype)
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/image/vtk/img-" + imagetype.name() + ".vtk"));

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    data::Image::sptr image = data::Image::New();

    io::vtk::ImageReader::sptr reader = io::vtk::ImageReader::New();
    reader->setObject(image);
    reader->setFile(testFile);
    reader->read();

    vtkSmartPointer<vtkGenericDataObjectReader> vtk_reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    vtk_reader->SetFileName(testFile.string().c_str());
    vtk_reader->Update();
    vtkSmartPointer<vtkImageData> vtkImage = vtkImageData::SafeDownCast(vtk_reader->GetOutput());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "test on <" "sight/image/vtk/img-" + imagetype.name() + ".vtk" "> Failed ",
        imagetype,
        image->getType()
    );

    compareImageAttributes(
        image->getSize(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions(),

        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension()
    );
}

// ------------------------------------------------------------------------------

void ImageTest::setUp()
{
}

// ------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

// ------------------------------------------------------------------------------

void ImageTest::testImageToVtk()
{
    imageToVTKTest(core::Type::INT8, {VTK_CHAR, VTK_SIGNED_CHAR});
    imageToVTKTest(core::Type::UINT8, {VTK_UNSIGNED_CHAR});

    imageToVTKTest(core::Type::INT16, {VTK_SHORT});
    imageToVTKTest(core::Type::UINT16, {VTK_UNSIGNED_SHORT});

    imageToVTKTest(core::Type::INT32, {VTK_INT});
    imageToVTKTest(core::Type::UINT32, {VTK_UNSIGNED_INT});

    // imageToVTKTest("int64" , { VTK_LONG));
    // imageToVTKTest("uint64", { VTK_UNSIGNED_LONG));

    imageToVTKTest(core::Type::FLOAT, {VTK_FLOAT});
    imageToVTKTest(core::Type::DOUBLE, {VTK_DOUBLE});
}

// ------------------------------------------------------------------------------

void ImageTest::testFromVtk()
{
    imageFromVTKTest("sight/image/vtk/img.vtk", core::Type::INT16);

    imageFromVTKTest("sight/image/vtk/img-int8.vtk", core::Type::INT8);
    imageFromVTKTest("sight/image/vtk/img-uint8.vtk", core::Type::UINT8);

    imageFromVTKTest("sight/image/vtk/img-int16.vtk", core::Type::INT16);
    imageFromVTKTest("sight/image/vtk/img-uint16.vtk", core::Type::UINT16);

    imageFromVTKTest("sight/image/vtk/img-int32.vtk", core::Type::INT32);
    imageFromVTKTest("sight/image/vtk/img-uint32.vtk", core::Type::UINT32);

    //imageFromVTKTest("sight/image/vtk/img-int64.vtk", "int64"  );
    //imageFromVTKTest("sight/image/vtk/img-uint64.vtk", "uint64"  );

    imageFromVTKTest("sight/image/vtk/img-float.vtk", core::Type::FLOAT);
    imageFromVTKTest("sight/image/vtk/img-double.vtk", core::Type::DOUBLE);

    int nbComponents = 4;
    core::Type type  = core::Type::UINT8;

    vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();

    CPPUNIT_ASSERT(vtkImage);
    vtkImage->SetDimensions(64, 64, 1);
    vtkImage->SetSpacing(1.0, 1.0, 0.0);
    int dataType = io::vtk::TypeTranslator::translate(type);
    vtkImage->AllocateScalars(dataType, nbComponents);

    data::Image::sptr image = data::Image::New();
    io::vtk::fromVTKImage(vtkImage, image);

    const auto dumpLock = image->dump_lock();
    CPPUNIT_ASSERT_EQUAL(
        static_cast<std::size_t>(vtkImage->GetPointData()->GetScalars()->GetNumberOfComponents()),
        static_cast<std::size_t>(image->numComponents())
    );
    compareImageAttributes(
        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension(),

        image->getSize(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + type.name() + "> Failed ", type, image->getType());

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->getBuffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr));
}

// ------------------------------------------------------------------------------

template<typename TYPE>
void fromToTest(data::Image::PixelFormat format)
{
    const data::Image::Size size       = {10, 20, 0};
    const data::Image::Spacing spacing = {1., 1., 0};
    const data::Image::Origin origin   = {0., 0., 0.};
    const core::Type type              = core::Type::get<TYPE>();

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(image, size, spacing, origin, type, format);
    utestData::generator::Image::randomizeImage(image);

    vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(image, vtkImage);

    data::Image::sptr image2 = data::Image::New();
    io::vtk::fromVTKImage(vtkImage, image2);

    CPPUNIT_ASSERT_EQUAL(image->getSize()[0], image2->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(image->getSize()[1], image2->getSize()[1]);
    CPPUNIT_ASSERT_EQUAL(image->getSize()[2], image2->getSize()[2]);
    CPPUNIT_ASSERT_EQUAL(image->getType(), image2->getType());
    CPPUNIT_ASSERT_EQUAL(image->numComponents(), image2->numComponents());
    CPPUNIT_ASSERT_EQUAL(image->getPixelFormat(), image2->getPixelFormat());

    const auto imageDumpLock  = image->dump_lock();
    const auto image2DumpLock = image2->dump_lock();

    auto itr       = image->begin<TYPE>();
    auto itr2      = image2->begin<TYPE>();
    const auto end = image->end<TYPE>();

    std::size_t count = 0;
    for( ; itr != end ; ++itr, ++itr2, ++count)
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "[" + type.name() + "] pixel[" + std::to_string(count) + "]",
            *itr,
            *itr2
        );
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
    const std::filesystem::path imagePath(utestData::Data::dir() / "sight/image/mhd/BostonTeapot.mhd");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    data::Image::sptr image               = data::Image::New();
    io::vtk::MetaImageReader::sptr reader = io::vtk::MetaImageReader::New();
    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    compareImageAttributes(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,
        bostonTeapotSize.size(),

        image->getSize(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );
}

// ------------------------------------------------------------------------------

void ImageTest::mhdWriterTest()
{
#if VTK_MAJOR_VERSION >= 9
    // VTK9 doesn't produce the same output, floats are not rounded in the header and the compressed buffer is
    // surprisingly bigger
    const std::filesystem::path imagePath(utestData::Data::dir() / "sight/image/mhd/BostonTeapot-vtk9.mhd");
    const std::filesystem::path zRawPath(utestData::Data::dir() / "sight/image/mhd/BostonTeapot-vtk9.zraw");
#else
    const std::filesystem::path imagePath(utestData::Data::dir() / "sight/image/mhd/BostonTeapot.mhd");
    const std::filesystem::path zRawPath(utestData::Data::dir() / "sight/image/mhd/BostonTeapot.zraw");
#endif

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + zRawPath.string() + "' does not exist",
        std::filesystem::exists(zRawPath)
    );

    const std::filesystem::path testFile(core::tools::System::getTemporaryFolder() / "BostonTeapot.mhd");
    const std::filesystem::path testZRawFile(core::tools::System::getTemporaryFolder() / "BostonTeapot.zraw");

    data::Image::sptr image               = data::Image::New();
    io::vtk::MetaImageReader::sptr reader = io::vtk::MetaImageReader::New();
    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    io::vtk::MetaImageWriter::sptr writer = io::vtk::MetaImageWriter::New();
    writer->setObject(image);
    writer->setFile(testFile);
    writer->write();

    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
    CPPUNIT_ASSERT(std::filesystem::exists(testZRawFile));

    CPPUNIT_ASSERT(utestData::File::contentEquals(imagePath, testFile));
    CPPUNIT_ASSERT(utestData::File::contentEquals(zRawPath, testZRawFile));

    std::filesystem::remove(testFile);
    std::filesystem::remove(testZRawFile);

    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::Type::INT8, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::Type::UINT8, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::Type::INT16, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::Type::UINT16, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::Type::INT32, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::Type::UINT32, "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("int64", "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("uint64", "imageTest.mhd");

    const std::filesystem::path zFile(core::tools::System::getTemporaryFolder() / "imagetestfile.zraw");
    std::filesystem::remove(zFile);
}

// ------------------------------------------------------------------------------

void ImageTest::vtiReaderTest()
{
    const std::filesystem::path imagePath(utestData::Data::dir() / "sight/image/vti/BostonTeapot.vti");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    data::Image::sptr image              = data::Image::New();
    io::vtk::VtiImageReader::sptr reader = io::vtk::VtiImageReader::New();

    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    compareImageAttributes(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,
        bostonTeapotSize.size(),

        image->getSize(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );
}

// ------------------------------------------------------------------------------

void ImageTest::vtiWriterTest()
{
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::Type::INT8, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::Type::UINT8, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::Type::INT16, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::Type::UINT16, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::Type::INT32, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::Type::UINT32, "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("int64", "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("uint64", "imageTest.vti");
}

// ------------------------------------------------------------------------------

void ImageTest::vtkReaderTest()
{
    const std::filesystem::path imagePath(utestData::Data::dir() / "sight/image/vtk/img.vtk");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    data::Image::sptr image           = data::Image::New();
    io::vtk::ImageReader::sptr reader = io::vtk::ImageReader::New();

    reader->setObject(image);
    reader->setFile(imagePath);
    reader->read();

    data::Image::Size vtkSize {{230, 170, 58}};
    data::Image::Spacing vtkSpacing {{1.732, 1.732, 3.2}};
    data::Image::Origin vtkOrigin {{34.64, 86.6, 56}};

    compareImageAttributes(
        vtkSize,
        vtkSpacing,
        vtkOrigin,
        3,

        image->getSize(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );

    testVtkReader(core::Type::INT8);
    testVtkReader(core::Type::UINT8);
    testVtkReader(core::Type::INT16);
    testVtkReader(core::Type::UINT16);
    testVtkReader(core::Type::INT32);
    testVtkReader(core::Type::UINT32);
    // testVtkReader(std::string("int64"));
    // testVtkReader(std::string("uint64"));
    testVtkReader(core::Type::FLOAT);
    testVtkReader(core::Type::DOUBLE);
}

// ------------------------------------------------------------------------------

void ImageTest::vtkWriterTest()
{
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::Type::INT8, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::Type::UINT8, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::Type::INT16, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::Type::UINT16, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::Type::INT32, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::Type::UINT32, "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "int64", "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "uint64", "imageTest.vtk");
}

// ------------------------------------------------------------------------------

} // namespace sight::io::vtk::ut
