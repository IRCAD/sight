/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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
#include <core/tools/failed.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <service/op.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

#include <filesystem>

TEST_SUITE("sight::module::io::vtk::image_reader_writer")
{
//------------------------------------------------------------------------------

    static void run_image_srv(
        const std::string& _srvname,
        const boost::property_tree::ptree& _cfg,
        const sight::data::object::sptr& _image
)
    {
        sight::service::base::sptr srv = sight::service::add(_srvname);

        CHECK(srv);

        if(srv->is_a("sight::io::service::reader"))
        {
            srv->set_inout(_image, "data");
        }
        else
        {
            srv->set_input(_image, "data");
        }

        CHECK_NOTHROW(srv->set_config(_cfg));
        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().wait());
        CHECK_NOTHROW(srv->update().wait());
        CHECK_NOTHROW(srv->stop().wait());
        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    static boost::property_tree::ptree get_io_configuration(const std::filesystem::path& _file)
    {
        sight::service::config_t reader_srv_cfg;
        reader_srv_cfg.add("file", _file.string());

        return reader_srv_cfg;
    }

//------------------------------------------------------------------------------

    TEST_CASE("vtk_image_reader")
    {
        const std::filesystem::path file = sight::utest_data::dir() / "sight/image/vtk/img.vtk";

        CHECK(
            std::filesystem::exists(file)
        );

        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        // Data expected
        sight::data::image::spacing_t spacing_expected;
        // NOLINTNEXTLINE(modernize-use-std-numbers)
        spacing_expected[0] = 1.732;
        // NOLINTNEXTLINE(modernize-use-std-numbers)
        spacing_expected[1] = 1.732;
        spacing_expected[2] = 3.2;

        sight::data::image::origin_t origin_expected;
        origin_expected[0] = 34.64;
        origin_expected[1] = 86.6;
        origin_expected[2] = 56;

        sight::data::image::size_t size_expected;
        size_expected[0] = 230;
        size_expected[1] = 170;
        size_expected[2] = 58;

        run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image);

        // Data read.
        sight::data::image::spacing_t spacing_read = image->spacing();
        sight::data::image::origin_t origin_read   = image->origin();
        sight::data::image::size_t size_read       = image->size();

        CHECK_EQ(spacing_expected.size(), spacing_read.size());
        CHECK_EQ(origin_expected.size(), origin_read.size());
        CHECK_EQ(size_expected.size(), size_read.size());

        CHECK_EQ(spacing_expected[0], spacing_read[0]);
        CHECK_EQ(spacing_expected[1], spacing_read[1]);
        CHECK_EQ(spacing_expected[2], spacing_read[2]);

        CHECK_EQ(origin_expected[0], origin_read[0]);
        CHECK_EQ(origin_expected[1], origin_read[1]);
        CHECK_EQ(origin_expected[2], origin_read[2]);

        CHECK_EQ(size_expected[0], size_read[0]);
        CHECK_EQ(size_expected[1], size_read[1]);
        CHECK_EQ(size_expected[2], size_read[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("vti_image_reader")
    {
        const std::filesystem::path file = sight::utest_data::dir() / "sight/image/vti/BostonTeapot.vti";

        CHECK(
            std::filesystem::exists(file)
        );

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image);

        // Data expected
        sight::data::image::spacing_t spacing_expected;
        spacing_expected[0] = 1.0;
        spacing_expected[1] = 1.0;
        spacing_expected[2] = 1.0;

        sight::data::image::origin_t origin_expected;
        origin_expected[0] = 1.1;
        origin_expected[1] = 2.2;
        origin_expected[2] = 3.3;

        sight::data::image::size_t size_expected;
        size_expected[0] = 256;
        size_expected[1] = 256;
        size_expected[2] = 178;

        sight::core::type expected_type("int8"); // MHD File image type : MET_CHAR

        // Data read.
        sight::data::image::spacing_t spacing_read = image->spacing();
        sight::data::image::origin_t origin_read   = image->origin();
        sight::data::image::size_t size_read       = image->size();

        CHECK_EQ(spacing_expected.size(), spacing_read.size());
        CHECK_EQ(origin_expected.size(), origin_read.size());
        CHECK_EQ(size_expected.size(), size_read.size());

        CHECK_EQ(spacing_expected[0], spacing_read[0]);
        CHECK_EQ(spacing_expected[1], spacing_read[1]);
        CHECK_EQ(spacing_expected[2], spacing_read[2]);

        CHECK_EQ(origin_expected[0], origin_read[0]);
        CHECK_EQ(origin_expected[1], origin_read[1]);
        CHECK_EQ(origin_expected[2], origin_read[2]);

        CHECK_EQ(size_expected[0], size_read[0]);
        CHECK_EQ(size_expected[1], size_read[1]);
        CHECK_EQ(size_expected[2], size_read[2]);

        CHECK_EQ(expected_type, image->type());
    }

//------------------------------------------------------------------------------

    TEST_CASE("mhd_image_reader")
    {
        const std::filesystem::path file = sight::utest_data::dir() / "sight/image/mhd/BostonTeapot.mhd";

        CHECK(
            std::filesystem::exists(file)
        );

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image);

        // Data expected
        sight::data::image::spacing_t spacing_expected;
        spacing_expected[0] = 1.0;
        spacing_expected[1] = 1.0;
        spacing_expected[2] = 1.0;

        sight::data::image::origin_t origin_expected;
        origin_expected[0] = 1.1;
        origin_expected[1] = 2.2;
        origin_expected[2] = 3.3;

        sight::data::image::size_t size_expected;
        size_expected[0] = 256;
        size_expected[1] = 256;
        size_expected[2] = 178;

        sight::core::type expected_type("int8"); // MHD File image type : MET_CHAR

        // Data read.
        sight::data::image::spacing_t spacing_read = image->spacing();
        sight::data::image::origin_t origin_read   = image->origin();
        sight::data::image::size_t size_read       = image->size();

        CHECK_EQ(spacing_expected.size(), spacing_read.size());
        CHECK_EQ(origin_expected.size(), origin_read.size());
        CHECK_EQ(size_expected.size(), size_read.size());

        CHECK(spacing_expected[0] == spacing_read[0]);
        CHECK(spacing_expected[1] == spacing_read[1]);
        CHECK(spacing_expected[2] == spacing_read[2]);

        CHECK(origin_expected[0] == origin_read[0]);
        CHECK(origin_expected[1] == origin_read[1]);
        CHECK(origin_expected[2] == origin_read[2]);

        CHECK_EQ(size_expected[0], size_read[0]);
        CHECK_EQ(size_expected[1], size_read[1]);
        CHECK_EQ(size_expected[2], size_read[2]);

        CHECK_EQ(expected_type, image->type());
    }

//------------------------------------------------------------------------------

    TEST_CASE("image_reader_extension")
    {
        sight::core::os::temp_file tmp_file;

        std::ofstream o_file(tmp_file, std::ios::out | std::ios::trunc | std::ios::binary);
        o_file.close();

        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        {
            const std::string srvname("sight::module::io::vtk::image_reader");

            sight::service::base::sptr srv = sight::service::add(srvname);

            CHECK(srv);

            srv->set_inout(image, "data");

            CHECK_NOTHROW(srv->set_config(get_io_configuration(tmp_file)));
            CHECK_NOTHROW(srv->configure());
            CHECK_NOTHROW(srv->start().wait());
            CHECK_THROWS_AS(srv->update().get(), sight::core::tools::failed);
            CHECK_NOTHROW(srv->stop().wait());
            sight::service::remove(srv);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("vtk_image_writer")
    {
        // Data to write
        sight::core::type type                                       = sight::core::type::UINT8;
        const sight::data::image::size_t size_expected               = {10, 20, 30};
        const sight::data::image::spacing_t spacing_expected         = {0.24, 1.07, 2.21};
        const sight::data::image::origin_t origin_expected           = {-5.6, 15.16, 11.11};
        const sight::data::image::orientation_t orientation_expected = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64,
                                                                        0.6
        };

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_image(
            image,
            size_expected,
            spacing_expected,
            origin_expected,
            orientation_expected,
            type,
            sight::data::image::rgba,
            0
        );

        // Write to vtk image.
        sight::core::os::temp_dir tmp_dir;
        const auto file = tmp_dir / "tempFile.vtk";

        run_image_srv("sight::module::io::vtk::image_writer", get_io_configuration(file), image);

        // Read image from disk
        sight::data::image::sptr image_from_disk = std::make_shared<sight::data::image>();
        run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_from_disk);

        // Data read
        sight::data::image::spacing_t spacing_read = image->spacing();
        sight::data::image::origin_t origin_read   = image->origin();
        sight::data::image::size_t size_read       = image->size();

        CHECK_EQ(spacing_expected.size(), spacing_read.size());
        CHECK_EQ(origin_expected.size(), origin_read.size());
        CHECK_EQ(size_expected.size(), size_read.size());

        CHECK(spacing_expected[0] == spacing_read[0]);
        CHECK(spacing_expected[1] == spacing_read[1]);
        CHECK(spacing_expected[2] == spacing_read[2]);

        CHECK(origin_expected[0] == origin_read[0]);
        CHECK(origin_expected[1] == origin_read[1]);
        CHECK(origin_expected[2] == origin_read[2]);

        CHECK_EQ(size_expected[0], size_read[0]);
        CHECK_EQ(size_expected[1], size_read[1]);
        CHECK_EQ(size_expected[2], size_read[2]);

        const auto image_dump_lock           = image->dump_lock();
        const auto image_from_disk_dump_lock = image_from_disk->dump_lock();

        const char* const ptr_on_generated_image = static_cast<char*>(image->buffer());
        const char* const ptr_on_read_image      = static_cast<char*>(image_from_disk->buffer());

        CHECK_EQ(image->type(), image_from_disk->type());
        CHECK(
            std::equal(
                ptr_on_generated_image,
                ptr_on_generated_image + image->size_in_bytes(),
                ptr_on_read_image
            )
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("vtk_image_series_writer")
    {
        sight::core::type type = sight::core::type::FLOAT32;
        auto image_series      = std::make_shared<sight::data::image_series>();
        sight::utest_data::generator::image::generate_random_image(image_series, type);

        // Orientation seems to be unsupported by VTK
        image_series->set_orientation({1, 0, 0, 0, 1, 0, 0, 0, 1});

        sight::core::os::temp_dir tmp_dir;
        const auto file = tmp_dir / "imageSeries.vtk";

        // Write image series
        run_image_srv("sight::module::io::vtk::image_series_writer", get_io_configuration(file), image_series);

        // Read image series
        auto image_series2 = std::make_shared<sight::data::image_series>();
        run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_series2);

        image_series2->set_window_center(image_series->window_center());
        image_series2->set_window_width(image_series->window_width());

        CHECK(*image_series == *image_series2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("vti_image_writer")
    {
// FIXME: This test is disabled on linux until vtk is patched upstream.
// see https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=1064762
#ifdef WIN32
        // Data to write
        sight::core::type type                                       = sight::core::type::UINT8;
        const sight::data::image::size_t size_expected               = {10, 20, 30};
        const sight::data::image::spacing_t spacing_expected         = {0.24, 1.07, 2.21};
        const sight::data::image::origin_t origin_expected           = {-5.6, 15.16, 11.11};
        const sight::data::image::orientation_t orientation_expected = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64,
                                                                        0.6
        };

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_image(
            image,
            size_expected,
            spacing_expected,
            origin_expected,
            orientation_expected,
            type,
            sight::data::image::gray_scale,
            0
        );

        // Write to vtk image.
        sight::core::os::temp_dir tmp_dir;
        const auto file = tmp_dir / "tempFile.vti";

        run_image_srv("sight::module::io::vtk::image_writer", get_io_configuration(file), image);

        // Read image from disk
        sight::data::image::sptr image_from_disk = std::make_shared<sight::data::image>();
        run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_from_disk);

        // Data read
        sight::data::image::spacing_t spacing_read         = image->spacing();
        sight::data::image::origin_t origin_read           = image->origin();
        sight::data::image::orientation_t orientation_read = image->orientation();
        sight::data::image::size_t size_read               = image->size();

        CHECK_EQ(spacing_expected.size(), spacing_read.size());
        CHECK_EQ(origin_expected.size(), origin_read.size());
        CHECK_EQ(size_expected.size(), size_read.size());

        CHECK(spacing_expected[0] == spacing_read[0]);
        CHECK(spacing_expected[1] == spacing_read[1]);
        CHECK(spacing_expected[2] == spacing_read[2]);

        CHECK(origin_expected[0] == origin_read[0]);
        CHECK(origin_expected[1] == origin_read[1]);
        CHECK(origin_expected[2] == origin_read[2]);

        for(std::size_t i = 0 ; i < orientation_read.size() ; ++i)
        {
            CHECK(orientation_expected[i] == orientation_read[i]);
        }

        CHECK_EQ(size_expected[0], size_read[0]);
        CHECK_EQ(size_expected[1], size_read[1]);
        CHECK_EQ(size_expected[2], size_read[2]);

        const auto image_dump_lock           = image->dump_lock();
        const auto image_from_disk_dump_lock = image_from_disk->dump_lock();

        const char* const ptr_on_generated_image = static_cast<char*>(image->buffer());
        const char* const ptr_on_read_image      = static_cast<char*>(image_from_disk->buffer());

        CHECK_EQ(image->type(), image_from_disk->type());
        CHECK(
            std::equal(
                ptr_on_generated_image,
                ptr_on_generated_image + image->size_in_bytes(),
                ptr_on_read_image
            )
        );
#endif // ifdef WIN32
    }

//------------------------------------------------------------------------------

    TEST_CASE("mhd_image_writer")
    {
        // Data to write
        sight::core::type type                                       = sight::core::type::UINT8;
        const sight::data::image::size_t size_expected               = {10, 20, 30};
        const sight::data::image::spacing_t spacing_expected         = {0.24, 1.07, 2.21};
        const sight::data::image::origin_t origin_expected           = {-5.6, 15.16, 11.11};
        const sight::data::image::orientation_t orientation_expected = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64,
                                                                        0.6
        };

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_image(
            image,
            size_expected,
            spacing_expected,
            origin_expected,
            orientation_expected,
            type,
            sight::data::image::rgb,
            0
        );

        // Write to vtk image.
        sight::core::os::temp_dir tmp_dir;
        const auto file = tmp_dir / "tempFile.mhd";

        run_image_srv("sight::module::io::vtk::image_writer", get_io_configuration(file), image);

        // Read image from disk
        sight::data::image::sptr image_from_disk = std::make_shared<sight::data::image>();
        run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_from_disk);

        // Data read
        sight::data::image::spacing_t spacing_read = image->spacing();
        sight::data::image::origin_t origin_read   = image->origin();
        sight::data::image::size_t size_read       = image->size();

        CHECK_EQ(spacing_expected.size(), spacing_read.size());
        CHECK_EQ(origin_expected.size(), origin_read.size());
        CHECK_EQ(size_expected.size(), size_read.size());

        CHECK(spacing_expected[0] == spacing_read[0]);
        CHECK(spacing_expected[1] == spacing_read[1]);
        CHECK(spacing_expected[2] == spacing_read[2]);

        CHECK(origin_expected[0] == origin_read[0]);
        CHECK(origin_expected[1] == origin_read[1]);
        CHECK(origin_expected[2] == origin_read[2]);

        CHECK_EQ(size_expected[0], size_read[0]);
        CHECK_EQ(size_expected[1], size_read[1]);
        CHECK_EQ(size_expected[2], size_read[2]);

        const auto image_dump_lock           = image->dump_lock();
        const auto image_from_disk_dump_lock = image_from_disk->dump_lock();

        const char* const ptr_on_generated_image = static_cast<char*>(image->buffer());
        const char* const ptr_on_read_image      = static_cast<char*>(image_from_disk->buffer());

        CHECK_EQ(image->type(), image_from_disk->type());
        CHECK(
            std::equal(
                ptr_on_generated_image,
                ptr_on_generated_image + image->size_in_bytes(),
                ptr_on_read_image
            )
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE("image_writer_extension")
    {
        // Data to write
        const auto type = sight::core::type::UINT8;
        const sight::data::image::size_t size_expected {10, 20, 30};
        const sight::data::image::spacing_t spacing_expected {0.24, 1.07, 2.21};
        const sight::data::image::origin_t origin_expected {-5.6, 15.16, 11.11};
        const sight::data::image::orientation_t orientation_expected = {0.36, 0.48, -0.8, -0.8, 0.6, 0.0, 0.48, 0.64,
                                                                        0.6
        };

        sight::data::image::sptr image = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_image(
            image,
            size_expected,
            spacing_expected,
            origin_expected,
            orientation_expected,
            type,
            sight::data::image::gray_scale,
            0
        );

        // Write to vtk image.
        sight::core::os::temp_dir tmp_dir;
        const auto file = tmp_dir / "tempFile.xxx";

        {
            const std::string srvname("sight::module::io::vtk::image_writer");

            sight::service::base::sptr srv = sight::service::add(srvname);

            CHECK(srv);

            srv->set_input(image, "data");
            CHECK_NOTHROW(srv->set_config(get_io_configuration(file)));
            CHECK_NOTHROW(srv->configure());
            CHECK_NOTHROW(srv->start().wait());
            CHECK_THROWS_AS(srv->update().get(), sight::core::tools::failed);
            CHECK_NOTHROW(srv->stop().wait());
            sight::service::remove(srv);
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
