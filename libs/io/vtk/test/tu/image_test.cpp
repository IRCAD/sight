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

template<typename exp_size_t, typename exp_spacing_t, typename exp_origin_t, typename exp_dim_t,
         typename size_t, typename spacing_t, typename origin_t, typename dim_t>
void compare_image_attributes(
    const exp_size_t& _exp_size,
    const exp_spacing_t& _exp_spacing,
    const exp_origin_t& _exp_origin,
    exp_dim_t _exp_dim,
    const size_t& _size,
    const spacing_t& _spacing,
    const origin_t& _origin,
    dim_t _dim
)
{
    CPPUNIT_ASSERT_EQUAL(
        static_cast<std::size_t>(_exp_dim),
        static_cast<std::size_t>(_dim)
    );

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(_dim) ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::image::Spacing::value_type>(_exp_spacing[i]),
            static_cast<data::image::Spacing::value_type>(_spacing[i]),
            epsilon
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::image::Origin::value_type>(_exp_origin[i]),
            static_cast<data::image::Origin::value_type>(_origin[i]),
            epsilon
        );
        CPPUNIT_ASSERT_EQUAL(
            static_cast<data::image::Size::value_type>(_exp_size[i]),
            static_cast<data::image::Size::value_type>(_size[i])
        );
    }
}

//------------------------------------------------------------------------------

void image_to_vtk_test(const core::type _imgtype, const std::set<int>& _vtk_types)
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
        _imgtype,
        data::image::PixelFormat::GRAY_SCALE
    );

    const auto dump_lock = image->dump_lock();

    vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(image, vtk_image);

    compare_image_attributes(
        size,
        spacing,
        origin,
        image->numDimensions(),

        vtk_image->GetDimensions(),
        vtk_image->GetSpacing(),
        vtk_image->GetOrigin(),
        vtk_image->GetDataDimension()

    );

    std::set<int> types = _vtk_types;
    CPPUNIT_ASSERT_MESSAGE(
        "Test failed for type " + _imgtype.name(),
        types.find(vtk_image->GetScalarType()) != types.end()
    );

    char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
    char* ptr     = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT_MESSAGE(
        "Test failed for type " + _imgtype.name(),
        std::equal(ptr, ptr + image->getSizeInBytes(), vtk_ptr)
    );
}

//------------------------------------------------------------------------------

template<typename W, typename R>
void writer_test(const core::type _imagetype, const std::string& _filename)
{
    core::os::temp_dir tmp_dir;
    const auto& test_file = tmp_dir / _filename;

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(image, _imagetype);

    typename W::sptr writer = std::make_shared<W>();
    writer->set_object(image);
    writer->set_file(test_file);
    writer->write();

    CPPUNIT_ASSERT_MESSAGE(
        "test on <" + _filename + ">  of type <" + _imagetype.name() + "> Failed ",
        std::filesystem::exists(test_file)
    );

    data::image::sptr image2 = std::make_shared<data::image>();
    typename R::sptr reader  = std::make_shared<R>();
    reader->set_object(image2);
    reader->set_file(test_file);
    reader->read();

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "test on <" + _filename + "> of type <" + _imagetype.name() + "> Failed ",
        image->getType(),
        image2->getType()
    );

    compare_image_attributes(
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

void image_from_vtk_test(const std::string& _imagename, const core::type& _type)
{
    const std::filesystem::path image_path(utest_data::Data::dir()
                                           / std::filesystem::path(_imagename));

    CPPUNIT_ASSERT(std::filesystem::exists(image_path));
    CPPUNIT_ASSERT(std::filesystem::is_regular_file(image_path));

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(image_path.string().c_str());
    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();
    vtkSmartPointer<vtkImageData> vtk_image = vtkImageData::SafeDownCast(reader->GetOutput());

    CPPUNIT_ASSERT(vtk_image);

    data::image::sptr image = std::make_shared<data::image>();
    io::vtk::from_vtk_image(vtk_image, image);

    const auto dump_lock = image->dump_lock();

    compare_image_attributes(
        vtk_image->GetDimensions(),
        vtk_image->GetSpacing(),
        vtk_image->GetOrigin(),
        vtk_image->GetDataDimension(),

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + _imagename + "> Failed ", _type, image->getType());

    char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
    char* ptr     = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->getSizeInBytes(), vtk_ptr));
}

//------------------------------------------------------------------------------

void test_vtk_reader(core::type _imagetype)
{
    const std::filesystem::path test_file(utest_data::Data::dir()
                                          / ("sight/image/vtk/img-" + _imagetype.name() + ".vtk"));

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    data::image::sptr image = std::make_shared<data::image>();

    io::vtk::ImageReader::sptr reader = std::make_shared<io::vtk::ImageReader>();
    reader->set_object(image);
    reader->set_file(test_file);
    reader->read();

    vtkSmartPointer<vtkGenericDataObjectReader> vtk_reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    vtk_reader->SetFileName(test_file.string().c_str());
    vtk_reader->Update();
    vtkSmartPointer<vtkImageData> vtk_image = vtkImageData::SafeDownCast(vtk_reader->GetOutput());
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "test on <" "sight/image/vtk/img-" + _imagetype.name() + ".vtk" "> Failed ",
        _imagetype,
        image->getType()
    );

    compare_image_attributes(
        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions(),

        vtk_image->GetDimensions(),
        vtk_image->GetSpacing(),
        vtk_image->GetOrigin(),
        vtk_image->GetDataDimension()
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
    image_to_vtk_test(core::type::INT8, {VTK_CHAR, VTK_SIGNED_CHAR});
    image_to_vtk_test(core::type::UINT8, {VTK_UNSIGNED_CHAR});

    image_to_vtk_test(core::type::INT16, {VTK_SHORT});
    image_to_vtk_test(core::type::UINT16, {VTK_UNSIGNED_SHORT});

    image_to_vtk_test(core::type::INT32, {VTK_INT});
    image_to_vtk_test(core::type::UINT32, {VTK_UNSIGNED_INT});

    // imageToVTKTest("int64" , { VTK_LONG));
    // imageToVTKTest("uint64", { VTK_UNSIGNED_LONG));

    image_to_vtk_test(core::type::FLOAT, {VTK_FLOAT});
    image_to_vtk_test(core::type::DOUBLE, {VTK_DOUBLE});
}

// ------------------------------------------------------------------------------

void image_test::testFromVtk()
{
    image_from_vtk_test("sight/image/vtk/img.vtk", core::type::INT16);

    image_from_vtk_test("sight/image/vtk/img-int8.vtk", core::type::INT8);
    image_from_vtk_test("sight/image/vtk/img-uint8.vtk", core::type::UINT8);

    image_from_vtk_test("sight/image/vtk/img-int16.vtk", core::type::INT16);
    image_from_vtk_test("sight/image/vtk/img-uint16.vtk", core::type::UINT16);

    image_from_vtk_test("sight/image/vtk/img-int32.vtk", core::type::INT32);
    image_from_vtk_test("sight/image/vtk/img-uint32.vtk", core::type::UINT32);

    //imageFromVTKTest("sight/image/vtk/img-int64.vtk", "int64"  );
    //imageFromVTKTest("sight/image/vtk/img-uint64.vtk", "uint64"  );

    image_from_vtk_test("sight/image/vtk/img-float.vtk", core::type::FLOAT);
    image_from_vtk_test("sight/image/vtk/img-double.vtk", core::type::DOUBLE);

    int nb_components = 4;
    core::type type   = core::type::UINT8;

    vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();

    CPPUNIT_ASSERT(vtk_image);
    vtk_image->SetDimensions(64, 64, 1);
    vtk_image->SetSpacing(1.0, 1.0, 0.0);
    int data_type = io::vtk::TypeTranslator::translate(type);
    vtk_image->AllocateScalars(data_type, nb_components);

    data::image::sptr image = std::make_shared<data::image>();
    io::vtk::from_vtk_image(vtk_image, image);

    const auto dump_lock = image->dump_lock();
    CPPUNIT_ASSERT_EQUAL(
        static_cast<std::size_t>(vtk_image->GetPointData()->GetScalars()->GetNumberOfComponents()),
        static_cast<std::size_t>(image->numComponents())
    );
    compare_image_attributes(
        vtk_image->GetDimensions(),
        vtk_image->GetSpacing(),
        vtk_image->GetOrigin(),
        vtk_image->GetDataDimension(),

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + type.name() + "> Failed ", type, image->getType());

    char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
    char* ptr     = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->getSizeInBytes(), vtk_ptr));
}

// ------------------------------------------------------------------------------

template<typename TYPE>
void from_to_test(data::image::PixelFormat _format)
{
    const data::image::Size size       = {10, 20, 0};
    const data::image::Spacing spacing = {1., 1., 0};
    const data::image::Origin origin   = {0., 0., 0.};
    const core::type type              = core::type::get<TYPE>();

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateImage(image, size, spacing, origin, type, _format, 0);

    vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(image, vtk_image);

    data::image::sptr image2 = std::make_shared<data::image>();
    io::vtk::from_vtk_image(vtk_image, image2);

    CPPUNIT_ASSERT_EQUAL(image->size()[0], image2->size()[0]);
    CPPUNIT_ASSERT_EQUAL(image->size()[1], image2->size()[1]);
    CPPUNIT_ASSERT_EQUAL(image->size()[2], image2->size()[2]);
    CPPUNIT_ASSERT_EQUAL(image->getType(), image2->getType());
    CPPUNIT_ASSERT_EQUAL(image->numComponents(), image2->numComponents());
    CPPUNIT_ASSERT_EQUAL(image->getPixelFormat(), image2->getPixelFormat());

    const auto image_dump_lock  = image->dump_lock();
    const auto image2_dump_lock = image2->dump_lock();

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
    from_to_test<std::uint8_t>(data::image::GRAY_SCALE);
    from_to_test<std::uint8_t>(data::image::RGB);
    from_to_test<std::uint8_t>(data::image::RGBA);

    from_to_test<std::uint16_t>(data::image::GRAY_SCALE);
    from_to_test<std::uint16_t>(data::image::RGB);
    from_to_test<std::uint16_t>(data::image::RGBA);

    from_to_test<std::uint32_t>(data::image::GRAY_SCALE);
    from_to_test<std::uint32_t>(data::image::RGB);
    from_to_test<std::uint32_t>(data::image::RGBA);

    from_to_test<std::int8_t>(data::image::GRAY_SCALE);
    from_to_test<std::int8_t>(data::image::RGB);
    from_to_test<std::int8_t>(data::image::RGBA);

    from_to_test<std::int16_t>(data::image::GRAY_SCALE);
    from_to_test<std::int16_t>(data::image::RGB);
    from_to_test<std::int16_t>(data::image::RGBA);

    from_to_test<std::int32_t>(data::image::GRAY_SCALE);
    from_to_test<std::int32_t>(data::image::RGB);
    from_to_test<std::int32_t>(data::image::RGBA);

    // uint64 and int64 are not recognized on Windows for VTK conversion.
}

// ------------------------------------------------------------------------------

void image_test::mhdReaderTest()
{
    const std::filesystem::path image_path(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot.mhd");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_path.string() + "' does not exist",
        std::filesystem::exists(image_path)
    );

    data::image::sptr image               = std::make_shared<data::image>();
    io::vtk::MetaImageReader::sptr reader = std::make_shared<io::vtk::MetaImageReader>();
    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    compare_image_attributes(
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
    const std::filesystem::path image_path(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot-vtk9.mhd");
    const std::filesystem::path z_raw_path(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot-vtk9.zraw");
#else
    const std::filesystem::path imagePath(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot.mhd");
    const std::filesystem::path zRawPath(utest_data::Data::dir() / "sight/image/mhd/BostonTeapot.zraw");
#endif

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_path.string() + "' does not exist",
        std::filesystem::exists(image_path)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + z_raw_path.string() + "' does not exist",
        std::filesystem::exists(z_raw_path)
    );

    core::os::temp_dir tmp_dir;
    const auto test_file       = tmp_dir / "BostonTeapot.mhd";
    const auto test_z_raw_file = tmp_dir / "BostonTeapot.zraw";

    auto image  = std::make_shared<data::image>();
    auto reader = std::make_shared<io::vtk::MetaImageReader>();
    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    auto writer = std::make_shared<io::vtk::MetaImageWriter>();
    writer->set_object(image);
    writer->set_file(test_file);
    writer->write();

    CPPUNIT_ASSERT(std::filesystem::exists(test_file));
    CPPUNIT_ASSERT(std::filesystem::exists(test_z_raw_file));

    CPPUNIT_ASSERT(utest_data::File::contentEquals(image_path, test_file));
    CPPUNIT_ASSERT(utest_data::File::contentEquals(z_raw_path, test_z_raw_file));

    writer_test<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::INT8, "imageTest.mhd");
    writer_test<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::UINT8, "imageTest.mhd");
    writer_test<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::INT16, "imageTest.mhd");
    writer_test<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::UINT16, "imageTest.mhd");
    writer_test<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::INT32, "imageTest.mhd");
    writer_test<io::vtk::MetaImageWriter, io::vtk::MetaImageReader>(core::type::UINT32, "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("int64", "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("uint64", "imageTest.mhd");
}

// ------------------------------------------------------------------------------

void image_test::vtiReaderTest()
{
    const std::filesystem::path image_path(utest_data::Data::dir() / "sight/image/vti/BostonTeapot.vti");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_path.string() + "' does not exist",
        std::filesystem::exists(image_path)
    );

    data::image::sptr image              = std::make_shared<data::image>();
    io::vtk::VtiImageReader::sptr reader = std::make_shared<io::vtk::VtiImageReader>();

    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    compare_image_attributes(
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
    writer_test<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::INT8, "imageTest.vti");
    writer_test<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::UINT8, "imageTest.vti");
    writer_test<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::INT16, "imageTest.vti");
    writer_test<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::UINT16, "imageTest.vti");
    writer_test<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::INT32, "imageTest.vti");
    writer_test<io::vtk::VtiImageWriter, io::vtk::VtiImageReader>(core::type::UINT32, "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("int64", "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("uint64", "imageTest.vti");
}

// ------------------------------------------------------------------------------

void image_test::vtkReaderTest()
{
    const std::filesystem::path image_path(utest_data::Data::dir() / "sight/image/vtk/img.vtk");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_path.string() + "' does not exist",
        std::filesystem::exists(image_path)
    );

    data::image::sptr image           = std::make_shared<data::image>();
    io::vtk::ImageReader::sptr reader = std::make_shared<io::vtk::ImageReader>();

    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    data::image::Size vtk_size {{230, 170, 58}};
    data::image::Spacing vtk_spacing {{1.732, 1.732, 3.2}};
    data::image::Origin vtk_origin {{34.64, 86.6, 56}};

    compare_image_attributes(
        vtk_size,
        vtk_spacing,
        vtk_origin,
        3,

        image->size(),
        image->getSpacing(),
        image->getOrigin(),
        image->numDimensions()
    );

    test_vtk_reader(core::type::INT8);
    test_vtk_reader(core::type::UINT8);
    test_vtk_reader(core::type::INT16);
    test_vtk_reader(core::type::UINT16);
    test_vtk_reader(core::type::INT32);
    test_vtk_reader(core::type::UINT32);
    // testVtkReader(std::string("int64"));
    // testVtkReader(std::string("uint64"));
    test_vtk_reader(core::type::FLOAT);
    test_vtk_reader(core::type::DOUBLE);
}

// ------------------------------------------------------------------------------

void image_test::vtkWriterTest()
{
    writer_test<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::INT8, "imageTest.vtk");
    writer_test<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::UINT8, "imageTest.vtk");
    writer_test<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::INT16, "imageTest.vtk");
    writer_test<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::UINT16, "imageTest.vtk");
    writer_test<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::INT32, "imageTest.vtk");
    writer_test<io::vtk::ImageWriter, io::vtk::ImageReader>(core::type::UINT32, "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "int64", "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "uint64", "imageTest.vtk");
}

// ------------------------------------------------------------------------------

} // namespace sight::io::vtk::ut
