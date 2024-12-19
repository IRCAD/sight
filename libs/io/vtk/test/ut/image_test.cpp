/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <io/vtk/image_reader.hpp>
#include <io/vtk/image_writer.hpp>
#include <io/vtk/meta_image_reader.hpp>
#include <io/vtk/meta_image_writer.hpp>
#include <io/vtk/vti_image_reader.hpp>
#include <io/vtk/vti_image_writer.hpp>
#include <io/vtk/vtk.hpp>

#include <utest_data/data.hpp>
#include <utest_data/file.hpp>
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

static const double EPSILON = 0.00001;

static const data::image::size_t BOSTON_TEAPOT_SIZE       = {{256, 256, 178}};
static const data::image::spacing_t BOSTON_TEAPOT_SPACING = {{1, 1, 1}};
static const data::image::origin_t BOSTON_TEAPOT_ORIGIN   = {{1.1, 2.2, 3.3}};

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
            static_cast<data::image::spacing_t::value_type>(_exp_spacing[i]),
            static_cast<data::image::spacing_t::value_type>(_spacing[i]),
            EPSILON
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::image::origin_t::value_type>(_exp_origin[i]),
            static_cast<data::image::origin_t::value_type>(_origin[i]),
            EPSILON
        );
        CPPUNIT_ASSERT_EQUAL(
            static_cast<data::image::size_t::value_type>(_exp_size[i]),
            static_cast<data::image::size_t::value_type>(_size[i])
        );
    }
}

//------------------------------------------------------------------------------

void image_to_vtk_test(const core::type _imgtype, const std::set<int>& _vtk_types)
{
    const data::image::size_t size               = {10, 15, 23};
    const data::image::spacing_t spacing         = {0.85, 2.6, 1.87};
    const data::image::origin_t origin           = {-45.6, 25.97, -53.9};
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        _imgtype,
        data::image::pixel_format_t::gray_scale
    );

    const auto dump_lock = image->dump_lock();

    vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(image, vtk_image);

    compare_image_attributes(
        size,
        spacing,
        origin,
        image->num_dimensions(),

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
        std::equal(ptr, ptr + image->size_in_bytes(), vtk_ptr)
    );
}

//------------------------------------------------------------------------------

template<typename W, typename R>
void writer_test(const core::type _imagetype, const std::string& _filename)
{
    core::os::temp_dir tmp_dir;
    const auto& test_file = tmp_dir / _filename;

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(image, _imagetype);

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
        image->type(),
        image2->type()
    );

    compare_image_attributes(
        image->size(),
        image->spacing(),
        image->origin(),
        image->num_dimensions(),

        image2->size(),
        image2->spacing(),
        image2->origin(),
        image2->num_dimensions()
    );
}

//------------------------------------------------------------------------------

void image_from_vtk_test(const std::string& _imagename, const core::type& _type)
{
    const std::filesystem::path image_path(utest_data::dir()
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
        image->spacing(),
        image->origin(),
        image->num_dimensions()
    );

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + _imagename + "> Failed ", _type, image->type());

    char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
    char* ptr     = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->size_in_bytes(), vtk_ptr));
}

//------------------------------------------------------------------------------

void test_vtk_reader(core::type _imagetype)
{
    const std::filesystem::path test_file(utest_data::dir()
                                          / ("sight/image/vtk/img-" + _imagetype.name() + ".vtk"));

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    data::image::sptr image = std::make_shared<data::image>();

    io::vtk::image_reader::sptr reader = std::make_shared<io::vtk::image_reader>();
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
        image->type()
    );

    compare_image_attributes(
        image->size(),
        image->spacing(),
        image->origin(),
        image->num_dimensions(),

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

void image_test::test_image_to_vtk()
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

void image_test::test_from_vtk()
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
    int data_type = io::vtk::type_translator::translate(type);
    vtk_image->AllocateScalars(data_type, nb_components);

    data::image::sptr image = std::make_shared<data::image>();
    io::vtk::from_vtk_image(vtk_image, image);

    const auto dump_lock = image->dump_lock();
    CPPUNIT_ASSERT_EQUAL(
        static_cast<std::size_t>(vtk_image->GetPointData()->GetScalars()->GetNumberOfComponents()),
        static_cast<std::size_t>(image->num_components())
    );
    compare_image_attributes(
        vtk_image->GetDimensions(),
        vtk_image->GetSpacing(),
        vtk_image->GetOrigin(),
        vtk_image->GetDataDimension(),

        image->size(),
        image->spacing(),
        image->origin(),
        image->num_dimensions()
    );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test on <" + type.name() + "> Failed ", type, image->type());

    char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
    char* ptr     = static_cast<char*>(image->buffer());

    CPPUNIT_ASSERT(std::equal(ptr, ptr + image->size_in_bytes(), vtk_ptr));
}

// ------------------------------------------------------------------------------

template<typename TYPE>
void from_to_test(enum data::image::pixel_format_t _format)
{
    const data::image::size_t size               = {10, 20, 0};
    const data::image::spacing_t spacing         = {1., 1., 0};
    const data::image::origin_t origin           = {0., 0., 0.};
    const data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
    const core::type type                        = core::type::get<TYPE>();

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_image(image, size, spacing, origin, orientation, type, _format, 0);

    vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(image, vtk_image);

    data::image::sptr image2 = std::make_shared<data::image>();
    io::vtk::from_vtk_image(vtk_image, image2);

    CPPUNIT_ASSERT_EQUAL(image->size()[0], image2->size()[0]);
    CPPUNIT_ASSERT_EQUAL(image->size()[1], image2->size()[1]);
    CPPUNIT_ASSERT_EQUAL(image->size()[2], image2->size()[2]);
    CPPUNIT_ASSERT_EQUAL(image->type(), image2->type());
    CPPUNIT_ASSERT_EQUAL(image->num_components(), image2->num_components());
    CPPUNIT_ASSERT_EQUAL(image->pixel_format(), image2->pixel_format());

    const auto image_dump_lock  = image->dump_lock();
    const auto image_2dump_lock = image2->dump_lock();

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

void image_test::from_to_vtk_test()
{
    from_to_test<std::uint8_t>(data::image::gray_scale);
    from_to_test<std::uint8_t>(data::image::rgb);
    from_to_test<std::uint8_t>(data::image::rgba);

    from_to_test<std::uint16_t>(data::image::gray_scale);
    from_to_test<std::uint16_t>(data::image::rgb);
    from_to_test<std::uint16_t>(data::image::rgba);

    from_to_test<std::uint32_t>(data::image::gray_scale);
    from_to_test<std::uint32_t>(data::image::rgb);
    from_to_test<std::uint32_t>(data::image::rgba);

    from_to_test<std::int8_t>(data::image::gray_scale);
    from_to_test<std::int8_t>(data::image::rgb);
    from_to_test<std::int8_t>(data::image::rgba);

    from_to_test<std::int16_t>(data::image::gray_scale);
    from_to_test<std::int16_t>(data::image::rgb);
    from_to_test<std::int16_t>(data::image::rgba);

    from_to_test<std::int32_t>(data::image::gray_scale);
    from_to_test<std::int32_t>(data::image::rgb);
    from_to_test<std::int32_t>(data::image::rgba);

    // uint64 and int64 are not recognized on Windows for VTK conversion.
}

// ------------------------------------------------------------------------------

void image_test::mhd_reader_test()
{
    const std::filesystem::path image_path(utest_data::dir() / "sight/image/mhd/BostonTeapot.mhd");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_path.string() + "' does not exist",
        std::filesystem::exists(image_path)
    );

    data::image::sptr image                 = std::make_shared<data::image>();
    io::vtk::meta_image_reader::sptr reader = std::make_shared<io::vtk::meta_image_reader>();
    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    compare_image_attributes(
        BOSTON_TEAPOT_SIZE,
        BOSTON_TEAPOT_SPACING,
        BOSTON_TEAPOT_ORIGIN,
        BOSTON_TEAPOT_SIZE.size(),

        image->size(),
        image->spacing(),
        image->origin(),
        image->num_dimensions()
    );
}

// ------------------------------------------------------------------------------

void image_test::mhd_writer_test()
{
#if VTK_MAJOR_VERSION >= 9
    // VTK9 doesn't produce the same output, floats are not rounded in the header and the compressed buffer is
    // surprisingly bigger
    const std::filesystem::path image_path(utest_data::dir() / "sight/image/mhd/BostonTeapot-vtk9.mhd");
    const std::filesystem::path z_raw_path(utest_data::dir() / "sight/image/mhd/BostonTeapot-vtk9.zraw");
#else
    const std::filesystem::path imagePath(utest_data::dir() / "sight/image/mhd/BostonTeapot.mhd");
    const std::filesystem::path zRawPath(utest_data::dir() / "sight/image/mhd/BostonTeapot.zraw");
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
    auto reader = std::make_shared<io::vtk::meta_image_reader>();
    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    auto writer = std::make_shared<io::vtk::meta_image_writer>();
    writer->set_object(image);
    writer->set_file(test_file);
    writer->write();

    CPPUNIT_ASSERT(std::filesystem::exists(test_file));
    CPPUNIT_ASSERT(std::filesystem::exists(test_z_raw_file));

    CPPUNIT_ASSERT(utest_data::file::content_equals(image_path, test_file));
    CPPUNIT_ASSERT(utest_data::file::content_equals(z_raw_path, test_z_raw_file));

    writer_test<io::vtk::meta_image_writer, io::vtk::meta_image_reader>(core::type::INT8, "imageTest.mhd");
    writer_test<io::vtk::meta_image_writer, io::vtk::meta_image_reader>(core::type::UINT8, "imageTest.mhd");
    writer_test<io::vtk::meta_image_writer, io::vtk::meta_image_reader>(core::type::INT16, "imageTest.mhd");
    writer_test<io::vtk::meta_image_writer, io::vtk::meta_image_reader>(core::type::UINT16, "imageTest.mhd");
    writer_test<io::vtk::meta_image_writer, io::vtk::meta_image_reader>(core::type::INT32, "imageTest.mhd");
    writer_test<io::vtk::meta_image_writer, io::vtk::meta_image_reader>(core::type::UINT32, "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("int64", "imageTest.mhd");
    // writerTest< io::vtk::MetaImageWriter,::io::vtk::MetaImageReader>("uint64", "imageTest.mhd");
}

// ------------------------------------------------------------------------------

void image_test::vti_reader_test()
{
    const std::filesystem::path image_path(utest_data::dir() / "sight/image/vti/BostonTeapot.vti");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_path.string() + "' does not exist",
        std::filesystem::exists(image_path)
    );

    data::image::sptr image                = std::make_shared<data::image>();
    io::vtk::vti_image_reader::sptr reader = std::make_shared<io::vtk::vti_image_reader>();

    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    compare_image_attributes(
        BOSTON_TEAPOT_SIZE,
        BOSTON_TEAPOT_SPACING,
        BOSTON_TEAPOT_ORIGIN,
        BOSTON_TEAPOT_SIZE.size(),

        image->size(),
        image->spacing(),
        image->origin(),
        image->num_dimensions()
    );
}

// ------------------------------------------------------------------------------

void image_test::vti_writer_test()
{
    writer_test<io::vtk::vti_image_writer, io::vtk::vti_image_reader>(core::type::INT8, "imageTest.vti");
    writer_test<io::vtk::vti_image_writer, io::vtk::vti_image_reader>(core::type::UINT8, "imageTest.vti");
    writer_test<io::vtk::vti_image_writer, io::vtk::vti_image_reader>(core::type::INT16, "imageTest.vti");
    writer_test<io::vtk::vti_image_writer, io::vtk::vti_image_reader>(core::type::UINT16, "imageTest.vti");
    writer_test<io::vtk::vti_image_writer, io::vtk::vti_image_reader>(core::type::INT32, "imageTest.vti");
    writer_test<io::vtk::vti_image_writer, io::vtk::vti_image_reader>(core::type::UINT32, "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("int64", "imageTest.vti");
    // writerTest< io::vtk::VtiImageWriter, io::vtk::VtiImageReader>("uint64", "imageTest.vti");
}

// ------------------------------------------------------------------------------

void image_test::vtk_reader_test()
{
    const std::filesystem::path image_path(utest_data::dir() / "sight/image/vtk/img.vtk");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_path.string() + "' does not exist",
        std::filesystem::exists(image_path)
    );

    data::image::sptr image            = std::make_shared<data::image>();
    io::vtk::image_reader::sptr reader = std::make_shared<io::vtk::image_reader>();

    reader->set_object(image);
    reader->set_file(image_path);
    reader->read();

    data::image::size_t vtk_size {{230, 170, 58}};
    // NOLINTNEXTLINE(modernize-use-std-numbers)
    data::image::spacing_t vtk_spacing {{1.732, 1.732, 3.2}};
    data::image::origin_t vtk_origin {{34.64, 86.6, 56}};

    compare_image_attributes(
        vtk_size,
        vtk_spacing,
        vtk_origin,
        3,

        image->size(),
        image->spacing(),
        image->origin(),
        image->num_dimensions()
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

void image_test::vtk_writer_test()
{
    writer_test<io::vtk::image_writer, io::vtk::image_reader>(core::type::INT8, "imageTest.vtk");
    writer_test<io::vtk::image_writer, io::vtk::image_reader>(core::type::UINT8, "imageTest.vtk");
    writer_test<io::vtk::image_writer, io::vtk::image_reader>(core::type::INT16, "imageTest.vtk");
    writer_test<io::vtk::image_writer, io::vtk::image_reader>(core::type::UINT16, "imageTest.vtk");
    writer_test<io::vtk::image_writer, io::vtk::image_reader>(core::type::INT32, "imageTest.vtk");
    writer_test<io::vtk::image_writer, io::vtk::image_reader>(core::type::UINT32, "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "int64", "imageTest.vtk");
    // writerTest< io::vtk::ImageWriter, io::vtk::ImageReader>( "uint64", "imageTest.vtk");
}

// ------------------------------------------------------------------------------

} // namespace sight::io::vtk::ut
