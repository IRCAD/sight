/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <doctest/doctest.h>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

static const double EPSILON = 0.00001;

static const sight::data::image::size_t BOSTON_TEAPOT_SIZE       = {{256, 256, 178}};
static const sight::data::image::spacing_t BOSTON_TEAPOT_SPACING = {{1, 1, 1}};
static const sight::data::image::origin_t BOSTON_TEAPOT_ORIGIN   = {{1.1, 2.2, 3.3}};

//------------------------------------------------------------------------------

template<typename exp_size_t, typename exp_spacing_t, typename exp_origin_t, typename exp_dim_t,
         typename size_t, typename spacing_t, typename origin_t, typename dim_t>
static void compare_image_attributes(
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
    CHECK_EQ(
        static_cast<std::size_t>(_exp_dim),
        static_cast<std::size_t>(_dim)
    );

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(_dim) ; ++i)
    {
        CHECK_EQ(
            static_cast<sight::data::image::spacing_t::value_type>(_exp_spacing[i]),
            doctest::Approx(static_cast<sight::data::image::spacing_t::value_type>(_spacing[i])).epsilon(EPSILON)
        );
        CHECK_EQ(
            static_cast<sight::data::image::origin_t::value_type>(_exp_origin[i]),
            doctest::Approx(static_cast<sight::data::image::origin_t::value_type>(_origin[i])).epsilon(EPSILON)
        );
        CHECK_EQ(
            static_cast<sight::data::image::size_t::value_type>(_exp_size[i]),
            static_cast<sight::data::image::size_t::value_type>(_size[i])
        );
    }
}

//------------------------------------------------------------------------------

static void image_to_vtk(const sight::core::type _imgtype, const std::set<int>& _vtk_types)
{
    const sight::data::image::size_t size               = {10, 15, 23};
    const sight::data::image::spacing_t spacing         = {0.85, 2.6, 1.87};
    const sight::data::image::origin_t origin           = {-45.6, 25.97, -53.9};
    const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};

    sight::data::image::sptr image = std::make_shared<sight::data::image>();
    sight::utest_data::generator::image::generate_image(
        image,
        size,
        spacing,
        origin,
        orientation,
        _imgtype,
        sight::data::image::pixel_format_t::gray_scale
    );

    const auto dump_lock = image->dump_lock();

    vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();
    sight::io::vtk::to_vtk_image(image, vtk_image);

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
    CHECK_MESSAGE(
        types.find(vtk_image->GetScalarType()) != types.end(),
        "Test failed for type ",
        _imgtype.name()
    );

    char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
    char* ptr     = static_cast<char*>(image->buffer());

    CHECK_MESSAGE(
        std::equal(ptr, ptr + image->size_in_bytes(), vtk_ptr),
        "Test failed for type ",
        _imgtype.name()
    );
}

//------------------------------------------------------------------------------

template<typename W, typename R>
static void writer(const sight::core::type _imagetype, const std::string& _filename)
{
    sight::core::os::temp_dir tmp_dir;
    const auto& test_file = tmp_dir / _filename;

    sight::data::image::sptr image = std::make_shared<sight::data::image>();
    sight::utest_data::generator::image::generate_random_image(image, _imagetype);

    typename W::sptr writer = std::make_shared<W>();
    writer->set_object(image);
    writer->set_file(test_file);
    auto write_observer = std::make_shared<sight::core::progress::observer>("Test write");
    writer->write(write_observer);

    CHECK_MESSAGE(
        std::filesystem::exists(test_file),
        "test on <",
        _filename,
        ">  of type <",
        _imagetype.name(),
        "> Failed "
    );

    sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
    typename R::sptr reader         = std::make_shared<R>();
    reader->set_object(image2);
    reader->set_file(test_file);
    auto read_observer = std::make_shared<sight::core::progress::observer>("Test read");
    reader->read(read_observer);

    CHECK_MESSAGE(
        image->type() == image2->type(),
        "test on <",
        _filename,
        "> of type <",
        _imagetype.name(),
        "> Failed "
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

static void image_from_vtk(const std::string& _imagename, const sight::core::type& _type)
{
    const std::filesystem::path image_path(sight::utest_data::dir()
                                           / std::filesystem::path(_imagename));

    CHECK(std::filesystem::exists(image_path));
    CHECK(std::filesystem::is_regular_file(image_path));

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(image_path.string().c_str());
    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();
    vtkSmartPointer<vtkImageData> vtk_image = vtkImageData::SafeDownCast(reader->GetOutput());

    CHECK(vtk_image);

    sight::data::image::sptr image = std::make_shared<sight::data::image>();
    sight::io::vtk::from_vtk_image(vtk_image, image);

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

    CHECK_MESSAGE(_type == image->type(), "test on <", _imagename, "> Failed ");

    char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
    char* ptr     = static_cast<char*>(image->buffer());

    CHECK(std::equal(ptr, ptr + image->size_in_bytes(), vtk_ptr));
}

//------------------------------------------------------------------------------

static void test_vtk_reader(sight::core::type _imagetype)
{
    const std::filesystem::path test_file(sight::utest_data::dir()
                                          / ("sight/image/vtk/img-" + _imagetype.name() + ".vtk"));

    CHECK_MESSAGE(
        std::filesystem::exists(test_file),
        "The file '",
        test_file.string(),
        "' does not exist"
    );

    sight::data::image::sptr image = std::make_shared<sight::data::image>();

    auto observer                             = std::make_shared<sight::core::progress::observer>("Test read");
    sight::io::vtk::image_reader::sptr reader = std::make_shared<sight::io::vtk::image_reader>();
    reader->set_object(image);
    reader->set_file(test_file);
    reader->read(observer);

    vtkSmartPointer<vtkGenericDataObjectReader> vtk_reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    vtk_reader->SetFileName(test_file.string().c_str());
    vtk_reader->Update();
    vtkSmartPointer<vtkImageData> vtk_image = vtkImageData::SafeDownCast(vtk_reader->GetOutput());
    CHECK_MESSAGE(
        _imagetype == image->type(),
        "test on <sight/image/vtk/img-",
        _imagetype.name(),
        ".vtk> Failed "
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

TEST_SUITE("sight::io::vtk::image")
{
// ------------------------------------------------------------------------------

    TEST_CASE("image_to_vtk")
    {
        image_to_vtk(sight::core::type::INT8, {VTK_CHAR, VTK_SIGNED_CHAR});
        image_to_vtk(sight::core::type::UINT8, {VTK_UNSIGNED_CHAR});

        image_to_vtk(sight::core::type::INT16, {VTK_SHORT});
        image_to_vtk(sight::core::type::UINT16, {VTK_UNSIGNED_SHORT});

        image_to_vtk(sight::core::type::INT32, {VTK_INT});
        image_to_vtk(sight::core::type::UINT32, {VTK_UNSIGNED_INT});

        // image_to_vtk("int64" , { VTK_LONG));
        // image_to_vtk("uint64", { VTK_UNSIGNED_LONG));

        image_to_vtk(sight::core::type::FLOAT, {VTK_FLOAT});
        image_to_vtk(sight::core::type::DOUBLE, {VTK_DOUBLE});
    }

// ------------------------------------------------------------------------------

    TEST_CASE("from_vtk")
    {
        image_from_vtk("sight/image/vtk/img.vtk", sight::core::type::INT16);

        image_from_vtk("sight/image/vtk/img-int8.vtk", sight::core::type::INT8);
        image_from_vtk("sight/image/vtk/img-uint8.vtk", sight::core::type::UINT8);

        image_from_vtk("sight/image/vtk/img-int16.vtk", sight::core::type::INT16);
        image_from_vtk("sight/image/vtk/img-uint16.vtk", sight::core::type::UINT16);

        image_from_vtk("sight/image/vtk/img-int32.vtk", sight::core::type::INT32);
        image_from_vtk("sight/image/vtk/img-uint32.vtk", sight::core::type::UINT32);

        //image_from_vtk("sight/image/vtk/img-int64.vtk", "int64"  );
        //image_from_vtk("sight/image/vtk/img-uint64.vtk", "uint64"  );

        image_from_vtk("sight/image/vtk/img-float.vtk", sight::core::type::FLOAT);
        image_from_vtk("sight/image/vtk/img-double.vtk", sight::core::type::DOUBLE);

        int nb_components      = 4;
        sight::core::type type = sight::core::type::UINT8;

        vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();

        CHECK(vtk_image);
        vtk_image->SetDimensions(64, 64, 1);
        vtk_image->SetSpacing(1.0, 1.0, 0.0);
        int data_type = sight::io::vtk::type_translator::translate(type);
        vtk_image->AllocateScalars(data_type, nb_components);

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::io::vtk::from_vtk_image(vtk_image, image);

        const auto dump_lock = image->dump_lock();
        CHECK_EQ(
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
        CHECK_MESSAGE(type == image->type(), "test on <", type.name(), "> Failed ");

        char* vtk_ptr = static_cast<char*>(vtk_image->GetScalarPointer());
        char* ptr     = static_cast<char*>(image->buffer());

        CHECK(std::equal(ptr, ptr + image->size_in_bytes(), vtk_ptr));
    }

// ------------------------------------------------------------------------------

    template<typename TYPE>
    static void from_to(enum sight::data::image::pixel_format_t _format)
    {
        const sight::data::image::size_t size               = {10, 20, 0};
        const sight::data::image::spacing_t spacing         = {1., 1., 0};
        const sight::data::image::origin_t origin           = {0., 0., 0.};
        const sight::data::image::orientation_t orientation = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64, 0.6};
        const sight::core::type type                        = sight::core::type::get<TYPE>();

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_image(
            image,
            size,
            spacing,
            origin,
            orientation,
            type,
            _format,
            0
        );

        vtkSmartPointer<vtkImageData> vtk_image = vtkSmartPointer<vtkImageData>::New();
        sight::io::vtk::to_vtk_image(image, vtk_image);

        sight::data::image::sptr image2 = std::make_shared<sight::data::image>();
        sight::io::vtk::from_vtk_image(vtk_image, image2);

        CHECK_EQ(image->size()[0], image2->size()[0]);
        CHECK_EQ(image->size()[1], image2->size()[1]);
        CHECK_EQ(image->size()[2], image2->size()[2]);
        CHECK_EQ(image->type(), image2->type());
        CHECK_EQ(image->num_components(), image2->num_components());
        CHECK_EQ(image->pixel_format(), image2->pixel_format());

        const auto image_dump_lock  = image->dump_lock();
        const auto image_2dump_lock = image2->dump_lock();

        auto itr       = image->begin<TYPE>();
        auto itr2      = image2->begin<TYPE>();
        const auto end = image->end<TYPE>();

        std::size_t count = 0;
        for( ; itr != end ; ++itr, ++itr2, ++count)
        {
            CHECK_MESSAGE(
                *itr == *itr2,
                "[",
                type.name(),
                "] pixel[",
                std::to_string(count),
                "]"
            );
        }
    }

// ------------------------------------------------------------------------------

    TEST_CASE("from_to_vtk")
    {
        from_to<std::uint8_t>(sight::data::image::gray_scale);
        from_to<std::uint8_t>(sight::data::image::rgb);
        from_to<std::uint8_t>(sight::data::image::rgba);

        from_to<std::uint16_t>(sight::data::image::gray_scale);
        from_to<std::uint16_t>(sight::data::image::rgb);
        from_to<std::uint16_t>(sight::data::image::rgba);

        from_to<std::uint32_t>(sight::data::image::gray_scale);
        from_to<std::uint32_t>(sight::data::image::rgb);
        from_to<std::uint32_t>(sight::data::image::rgba);

        from_to<std::int8_t>(sight::data::image::gray_scale);
        from_to<std::int8_t>(sight::data::image::rgb);
        from_to<std::int8_t>(sight::data::image::rgba);

        from_to<std::int16_t>(sight::data::image::gray_scale);
        from_to<std::int16_t>(sight::data::image::rgb);
        from_to<std::int16_t>(sight::data::image::rgba);

        from_to<std::int32_t>(sight::data::image::gray_scale);
        from_to<std::int32_t>(sight::data::image::rgb);
        from_to<std::int32_t>(sight::data::image::rgba);

        // uint64 and int64 are not recognized on Windows for VTK conversion.
    }

// ------------------------------------------------------------------------------

    TEST_CASE("mhd_reader")
    {
        const std::filesystem::path image_path(sight::utest_data::dir() / "sight/image/mhd/BostonTeapot.mhd");

        CHECK_MESSAGE(
            std::filesystem::exists(image_path),
            "The file '",
            image_path.string(),
            "' does not exist"
        );

        sight::data::image::sptr image                 = std::make_shared<sight::data::image>();
        sight::io::vtk::meta_image_reader::sptr reader = std::make_shared<sight::io::vtk::meta_image_reader>();
        reader->set_object(image);
        reader->set_file(image_path);
        auto observer = std::make_shared<sight::core::progress::observer>("Test read");
        reader->read(observer);

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

    TEST_CASE("mhd_writer")
    {
#if VTK_MAJOR_VERSION >= 9
        const std::filesystem::path image_path(sight::utest_data::dir() / "sight/image/mhd/BostonTeapot-vtk9.mhd");
        const std::filesystem::path z_raw_path(sight::utest_data::dir() / "sight/image/mhd/BostonTeapot-vtk9.zraw");
#else
        const std::filesystem::path image_path(sight::utest_data::dir() / "sight/image/mhd/BostonTeapot.mhd");
        const std::filesystem::path z_raw_path(sight::utest_data::dir() / "sight/image/mhd/BostonTeapot.zraw");
#endif

        CHECK_MESSAGE(
            std::filesystem::exists(image_path),
            "The file '",
            image_path.string(),
            "' does not exist"
        );

        CHECK_MESSAGE(
            std::filesystem::exists(z_raw_path),
            "The file '",
            z_raw_path.string(),
            "' does not exist"
        );

        sight::core::os::temp_dir tmp_dir;
        const auto test_file       = tmp_dir / "BostonTeapot.mhd";
        const auto test_z_raw_file = tmp_dir / "BostonTeapot.zraw";

        auto image  = std::make_shared<sight::data::image>();
        auto reader = std::make_shared<sight::io::vtk::meta_image_reader>();
        reader->set_object(image);
        reader->set_file(image_path);
        auto read_observer = std::make_shared<sight::core::progress::observer>("Test read");
        reader->read(read_observer);

        auto writer = std::make_shared<sight::io::vtk::meta_image_writer>();
        writer->set_object(image);
        writer->set_file(test_file);
        auto write_observer = std::make_shared<sight::core::progress::observer>("Test read");
        writer->write(write_observer);

        CHECK(std::filesystem::exists(test_file));
        CHECK(std::filesystem::exists(test_z_raw_file));

        CHECK(sight::utest_data::file::content_equals(image_path, test_file));
        CHECK(sight::utest_data::file::content_equals(z_raw_path, test_z_raw_file));

        ::writer<sight::io::vtk::meta_image_writer, sight::io::vtk::meta_image_reader>(
            sight::core::type::INT8,
            "imageTest.mhd"
        );
        ::writer<sight::io::vtk::meta_image_writer, sight::io::vtk::meta_image_reader>(
            sight::core::type::UINT8,
            "imageTest.mhd"
        );
        ::writer<sight::io::vtk::meta_image_writer, sight::io::vtk::meta_image_reader>(
            sight::core::type::INT16,
            "imageTest.mhd"
        );
        ::writer<sight::io::vtk::meta_image_writer, sight::io::vtk::meta_image_reader>(
            sight::core::type::UINT16,
            "imageTest.mhd"
        );
        ::writer<sight::io::vtk::meta_image_writer, sight::io::vtk::meta_image_reader>(
            sight::core::type::INT32,
            "imageTest.mhd"
        );
        ::writer<sight::io::vtk::meta_image_writer, sight::io::vtk::meta_image_reader>(
            sight::core::type::UINT32,
            "imageTest.mhd"
        );
        // writer< sight::io::vtk::MetaImageWriter,::sight::io::vtk::MetaImageReader>("int64", "imageTest.mhd");
        // writer< sight::io::vtk::MetaImageWriter,::sight::io::vtk::MetaImageReader>("uint64", "imageTest.mhd");
    }

// ------------------------------------------------------------------------------

    TEST_CASE("vti_reader")
    {
        const std::filesystem::path image_path(sight::utest_data::dir() / "sight/image/vti/BostonTeapot.vti");

        CHECK_MESSAGE(
            std::filesystem::exists(image_path),
            "The file '",
            image_path.string(),
            "' does not exist"
        );

        sight::data::image::sptr image                = std::make_shared<sight::data::image>();
        sight::io::vtk::vti_image_reader::sptr reader = std::make_shared<sight::io::vtk::vti_image_reader>();

        reader->set_object(image);
        reader->set_file(image_path);
        auto read_observer = std::make_shared<sight::core::progress::observer>("Test read");
        reader->read(read_observer);

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

// FIXME: This test is disabled on linux until vtk is patched upstream.
// see https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=1064762
#ifdef WIN32
    TEST_CASE("vti_writer")
    {
        writer<sight::io::vtk::vti_image_writer, sight::io::vtk::vti_image_reader>(
            sight::core::type::INT8,
            "imageTest.vti"
        );
        writer<sight::io::vtk::vti_image_writer, sight::io::vtk::vti_image_reader>(
            sight::core::type::UINT8,
            "imageTest.vti"
        );
        writer<sight::io::vtk::vti_image_writer, sight::io::vtk::vti_image_reader>(
            sight::core::type::INT16,
            "imageTest.vti"
        );
        writer<sight::io::vtk::vti_image_writer, sight::io::vtk::vti_image_reader>(
            sight::core::type::UINT16,
            "imageTest.vti"
        );
        writer<sight::io::vtk::vti_image_writer, sight::io::vtk::vti_image_reader>(
            sight::core::type::INT32,
            "imageTest.vti"
        );
        writer<sight::io::vtk::vti_image_writer, sight::io::vtk::vti_image_reader>(
            sight::core::type::UINT32,
            "imageTest.vti"
        );
    }
#endif

// ------------------------------------------------------------------------------

    TEST_CASE("vtk_reader")
    {
        const std::filesystem::path image_path(sight::utest_data::dir() / "sight/image/vtk/img.vtk");

        CHECK_MESSAGE(
            std::filesystem::exists(image_path),
            "The file '",
            image_path.string(),
            "' does not exist"
        );

        auto image  = std::make_shared<sight::data::image>();
        auto reader = std::make_shared<sight::io::vtk::image_reader>();

        reader->set_object(image);
        reader->set_file(image_path);
        auto read_observer = std::make_shared<sight::core::progress::observer>("Test read");
        reader->read(read_observer);

        sight::data::image::size_t vtk_size {{230, 170, 58}};
        // NOLINTNEXTLINE(modernize-use-std-numbers)
        sight::data::image::spacing_t vtk_spacing {{1.732, 1.732, 3.2}};
        sight::data::image::origin_t vtk_origin {{34.64, 86.6, 56}};

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

        test_vtk_reader(sight::core::type::INT8);
        test_vtk_reader(sight::core::type::UINT8);
        test_vtk_reader(sight::core::type::INT16);
        test_vtk_reader(sight::core::type::UINT16);
        test_vtk_reader(sight::core::type::INT32);
        test_vtk_reader(sight::core::type::UINT32);
        test_vtk_reader(sight::core::type::FLOAT);
        test_vtk_reader(sight::core::type::DOUBLE);
    }

// ------------------------------------------------------------------------------

    TEST_CASE("vtk_writer")
    {
        writer<sight::io::vtk::image_writer, sight::io::vtk::image_reader>(sight::core::type::INT8, "imageTest.vtk");
        writer<sight::io::vtk::image_writer, sight::io::vtk::image_reader>(sight::core::type::UINT8, "imageTest.vtk");
        writer<sight::io::vtk::image_writer, sight::io::vtk::image_reader>(sight::core::type::INT16, "imageTest.vtk");
        writer<sight::io::vtk::image_writer, sight::io::vtk::image_reader>(sight::core::type::UINT16, "imageTest.vtk");
        writer<sight::io::vtk::image_writer, sight::io::vtk::image_reader>(sight::core::type::INT32, "imageTest.vtk");
        writer<sight::io::vtk::image_writer, sight::io::vtk::image_reader>(sight::core::type::UINT32, "imageTest.vtk");
    }

// ------------------------------------------------------------------------------
} // TEST_SUITE
