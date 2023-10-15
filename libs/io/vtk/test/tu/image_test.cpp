/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "image_test.hpp"

#include <core/os/temp_path.hpp>

#include <data/image.hpp>

#include <io/vtk/ImageReader.hpp>
#include <io/vtk/ImageWriter.hpp>
#include <io/vtk/MetaImageReader.hpp>
#include <io/vtk/MetaImageWriter.hpp>
#include <io/vtk/VtiImageReader.hpp>
#include <io/vtk/VtiImageWriter.hpp>
#include <io/vtk/vtk.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/File.hpp>
#include <utest_data/generator/image.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::image_test);

namespace sight::io::vtk::ut
{

static const double epsilon = 0.00001;

static const data::image::Size bostonTeapotSize       = {{256, 256, 178}};
static const data::image::Spacing bostonTeapotSpacing = {{1, 1, 1}};
static const data::image::Origin bostonTeapotOrigin   = {{1.1, 2.2, 3.3}};

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
            static_cast<data::image::Spacing::value_type>(expSpacing[i]),
            static_cast<data::image::Spacing::value_type>(spacing[i]),
            epsilon
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::image::Origin::value_type>(expOrigin[i]),
            static_cast<data::image::Origin::value_type>(origin[i]),
            epsilon
        );
        CPPUNIT_ASSERT_EQUAL(
            static_cast<data::image::Size::value_type>(expSize[i]),
            static_cast<data::image::Size::value_type>(size[i])
        );
    }
}

//------------------------------------------------------------------------------

void imageToVTKTest(const core::type imgtype, const std::set<int>& vtk_types)
{
    const data::image::Size size       = {10, 15, 23};
    const data::image::Spacing spacing = {0.85, 2.6, 1.87};
    const data::image::Origin origin   = {-45.6, 25.97, -53.9};

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateImage(
        image,
        size,
        spacing,
        origin,
        imgtype,
        data::image::PixelFormat::GRAY_SCALE
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
    char* ptr    = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT_MESSAGE(
        "Test failed for type " + imgtype.name(),
        std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr)
    );
}

//------------------------------------------------------------------------------

template<typename W, typename R>
void writerTest(const core::type imagetype, const std::string& filename)
{
    core::os::temp_dir tmpDir;
    const auto& testFile = tmpDir / filename;

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(image, imagetype);

    typename W::sptr writer = std::make_shared<W>();
    writer->set_object(image);
    writer->set_file(testFile);
    writer->write();

    CPPUNIT_ASSERT_MESSAGE(
        "test on <" + filename + ">  of type <" + imagetype.name() + "> Failed ",
        std::filesystem::exists(testFile)
    );

    data::image::sptr image2 = std::make_shared<data::image>();
    typename R::sptr reader  = std::make_shared<R>();
    reader->set_object(image2);
    reader->set_file(testFile);
    reader->read();

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "test on <" + filename + "> of type <" + imagetype.name() + "> Failed ",
        image->getType(),
        image2->getType()
    );

    compareImageAttributes(
        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions(),

        image2->size(),
        image2->getSpacing(),
        image2->getOrigin(),
        image2->numDimensions()
    );
}

//------------------------------------------------------------------------------

void imageFromVTKTest(const std::string& imagename, const core::type& type)
{
    const std::filesystem::path imagePath(utest_data::Data::dir()
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

    data::image::sptr image = std::make_shared<data::image>();
    io::vtk::fromVTKImage(vtkImage, image);

    const auto dumpLock = image->dump_lock();

    compareImageAttributes(
        vtkImage->GetDimensions(),
        vtkImage->GetSpacing(),
        vtkImage->GetOrigin(),
        vtkImage->GetDataDimension(),

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + imagename + "> Failed ", type, image->getType());

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr));
}

//------------------------------------------------------------------------------

void testVtkReader(core::type imagetype)
{
    const std::filesystem::path testFile(utest_data::Data::dir()
                                         / ("sight/image/vtk/img-" + imagetype.name() + ".vtk"));

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    data::image::sptr image = std::make_shared<data::image>();

    io::vtk::ImageReader::sptr reader = std::make_shared<io::vtk::ImageReader>();
    reader->set_object(image);
    reader->set_file(testFile);
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
        image->size(),
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

void image_test::setUp()
{
}

// ------------------------------------------------------------------------------

void image_test::tearDown()
{
    // Clean up after the test run.
}

// ------------------------------------------------------------------------------

void image_test::testImageToVtk()
{
    imageToVTKTest(core::type::INT8, {VTK_CHAR, VTK_SIGNED_CHAR});
    imageToVTKTest(core::type::UINT8, {VTK_UNSIGNED_CHAR});

    imageToVTKTest(core::type::INT16, {VTK_SHORT});
    imageToVTKTest(core::type::UINT16, {VTK_UNSIGNED_SHORT});

    imageToVTKTest(core::type::INT32, {VTK_INT});
    imageToVTKTest(core::type::UINT32, {VTK_UNSIGNED_INT});

    // imageToVTKTest("int64" , { VTK_LONG));
    // imageToVTKTest("uint64", { VTK_UNSIGNED_LONG));

    imageToVTKTest(core::type::FLOAT, {VTK_FLOAT});
    imageToVTKTest(core::type::DOUBLE, {VTK_DOUBLE});
}

// ------------------------------------------------------------------------------

void image_test::testFromVtk()
{
    imageFromVTKTest("sight/image/vtk/img.vtk", core::type::INT16);

    imageFromVTKTest("sight/image/vtk/img-int8.vtk", core::type::INT8);
    imageFromVTKTest("sight/image/vtk/img-uint8.vtk", core::type::UINT8);

    imageFromVTKTest("sight/image/vtk/img-int16.vtk", core::type::INT16);
    imageFromVTKTest("sight/image/vtk/img-uint16.vtk", core::type::UINT16);

    imageFromVTKTest("sight/image/vtk/img-int32.vtk", core::type::INT32);
    imageFromVTKTest("sight/image/vtk/img-uint32.vtk", core::type::UINT32);

    //imageFromVTKTest("sight/image/vtk/img-int64.vtk", "int64"  );
    //imageFromVTKTest("sight/image/vtk/img-uint64.vtk", "uint64"  );

    imageFromVTKTest("sight/image/vtk/img-float.vtk", core::type::FLOAT);
    imageFromVTKTest("sight/image/vtk/img-double.vtk", core::type::DOUBLE);

    int nbComponents = 4;
    core::type type  = core::type::UINT8;

    vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();

    CPPUNIT_ASSERT(vtkImage);
    vtkImage->SetDimensions(64, 64, 1);
    vtkImage->SetSpacing(1.0, 1.0, 0.0);
    int dataType = io::vtk::TypeTranslator::translate(type);
    vtkImage->AllocateScalars(dataType, nbComponents);

    data::image::sptr image = std::make_shared<data::image>();
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

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + type.name() + "> Failed ", type, image->getType());

    char* vtkPtr = static_cast<char*>(vtkImage->GetScalarPointer());
    char* ptr    = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->getSizeInBytes(), vtkPtr));
}

// ------------------------------------------------------------------------------

template<typename TYPE>
void fromToTest(data::image::PixelFormat format)
{
    const data::image::Size size       = {10, 20, 0};
    const data::image::Spacing spacing = {1., 1., 0};
    const data::image::Origin origin   = {0., 0., 0.};
    const core::type type              = core::type::get<TYPE>();

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateImage(image, size, spacing, origin, type, format, 0);

    vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(image, vtkImage);

    data::image::sptr image2 = std::make_shared<data::image>();
    io::vtk::fromVTKImage(vtkImage, image2);

    CPPUNIT_ASSERT_EQUAL(image->size()[0], image2->size()[0]);
    CPPUNIT_ASSERT_EQUAL(image->size()[1], image2->size()[1]);
    CPPUNIT_ASSERT_EQUAL(image->size()[2], image2->size()[2]);
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

void image_test::fromToVtkTest()
{
    fromToTest<std::uint8_t>(data::image::GRAY_SCALE);
    fromToTest<std::uint8_t>(data::image::RGB);
    fromToTest<std::uint8_t>(data::image::RGBA);

    fromToTest<std::uint16_t>(data::image::GRAY_SCALE);
    fromToTest<std::uint16_t>(data::image::RGB);
    fromToTest<std::uint16_t>(data::image::RGBA);

    fromToTest<std::uint32_t>(data::image::GRAY_SCALE);
    fromToTest<std::uint32_t>(data::image::RGB);
    fromToTest<std::uint32_t>(data::image::RGBA);

    fromToTest<std::int8_t>(data::image::GRAY_SCALE);
    fromToTest<std::int8_t>(data::image::RGB);
    fromToTest<std::int8_t>(data::image::RGBA);

    fromToTest<std::int16_t>(data::image::GRAY_SCALE);
    fromToTest<std::int16_t>(data::image::RGB);
    fromToTest<std::int16_t>(data::image::RGBA);

    fromToTest<std::int32_t>(data::image::GRAY_SCALE);
    fromToTest<std::int32_t>(data::image::RGB);
    fromToTest<std::int32_t>(data::image::RGBA);

    // uint64 and int64 are not recognized on Windows for VTK conversion.
}

// ------------------------------------------------------------------------------

void image_test::mhdReaderTest()
{
    const std::filesystem::path imagePath(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot.mhd");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    data::image::sptr image               = std::make_shared<data::image>();
    io::vtk::MetaImageReader::sptr reader = std::make_shared<io::vtk::MetaImageReader>();
    reader->set_object(image);
    reader->set_file(imagePath);
    reader->read();

    compareImageAttributes(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,
        bostonTeapotSize.size(),

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );
}

// ------------------------------------------------------------------------------

void image_test::mhdWriterTest()
{
#if VTK_MAJOR_VERSION >= 9
    // VTK9 doesn't produce the same output, floats are not rounded in the header and the compressed buffer is
    // surprisingly bigger
    const std::filesystem::path imagePath(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot-vtk9.mhd");
    const std::filesystem::path zRawPath(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot-vtk9.zraw");
#else
    const std::filesystem::path imagePath(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot.mhd");
    const std::filesystem::path zRawPath(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot.zraw");
#endif

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + zRawPath.string() + "' does not exist",
        std::filesystem::exists(zRawPath)
    );

    core::os::temp_dir tmpDir;
    const auto testFile     = tmpDir / "BostonTeapot.mhd";
    const auto testZRawFile = tmpDir / "BostonTeapot.zraw";

    auto image  = std::make_shared<data::image>();
    auto reader = std::make_shared<io::vtk::MetaImageReader>();
    reader->set_object(image);
    reader->set_file(imagePath);
    reader->read();

    auto writer = std::make_shared<io::vtk::MetaImageWriter>();
    writer->set_object(image);
    writer->set_file(testFile);
    writer->write();

    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
    CPPUNIT_ASSERT(std::filesystem::exists(testZRawFile));

    CPPUNIT_ASSERT(utest_data::File::contentEquals(imagePath, testFile));
    CPPUNIT_ASSERT(utest_data::File::contentEquals(zRawPath, testZRawFile));

    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::INT8, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::UINT8, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::INT16, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::UINT16, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::INT32, "imageTest.mhd");
    writerTest<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::UINT32, "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("int64", "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("uint64", "imageTest.mhd");
}

// ------------------------------------------------------------------------------

void image_test::vtiReaderTest()
{
    const std::filesystem::path imagePath(utest_data::Data::dir() / "sight/image/vti/BostonTeapot.vti");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    data::image::sptr image              = std::make_shared<data::image>();
    io::vtk::VtiImageReader::sptr reader = std::make_shared<io::vtk::VtiImageReader>();

    reader->set_object(image);
    reader->set_file(imagePath);
    reader->read();

    compareImageAttributes(
        bostonTeapotSize,
        bostonTeapotSpacing,
        bostonTeapotOrigin,
        bostonTeapotSize.size(),

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );
}

// ------------------------------------------------------------------------------

void image_test::vtiWriterTest()
{
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::INT8, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::UINT8, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::INT16, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::UINT16, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::INT32, "imageTest.vti");
    writerTest<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::UINT32, "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("int64", "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("uint64", "imageTest.vti");
}

// ------------------------------------------------------------------------------

void image_test::vtkReaderTest()
{
    const std::filesystem::path imagePath(utest_data::Data::dir() / "sight/image/vtk/img.vtk");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imagePath.string() + "' does not exist",
        std::filesystem::exists(imagePath)
    );

    data::image::sptr image           = std::make_shared<data::image>();
    io::vtk::ImageReader::sptr reader = std::make_shared<io::vtk::ImageReader>();

    reader->set_object(image);
    reader->set_file(imagePath);
    reader->read();

    data::image::Size vtkSize {{230, 170, 58}};
    data::image::Spacing vtkSpacing {{1.732, 1.732, 3.2}};
    data::image::Origin vtkOrigin {{34.64, 86.6, 56}};

    compareImageAttributes(
        vtkSize,
        vtkSpacing,
        vtkOrigin,
        3,

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );

    testVtkReader(core::type::INT8);
    testVtkReader(core::type::UINT8);
    testVtkReader(core::type::INT16);
    testVtkReader(core::type::UINT16);
    testVtkReader(core::type::INT32);
    testVtkReader(core::type::UINT32);
    // testVtkReader(std::string("int64"));
    // testVtkReader(std::string("uint64"));
    testVtkReader(core::type::FLOAT);
    testVtkReader(core::type::DOUBLE);
}

// ------------------------------------------------------------------------------

void image_test::vtkWriterTest()
{
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::INT8, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::UINT8, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::INT16, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::UINT16, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::INT32, "imageTest.vtk");
    writerTest<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::UINT32, "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "int64", "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "uint64", "imageTest.vtk");
}

// ------------------------------------------------------------------------------

} // namespace sight::io::vtk::ut
