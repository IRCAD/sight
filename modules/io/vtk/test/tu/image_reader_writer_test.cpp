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

#include "image_reader_writer_test.hpp"

#include <core/os/temp_path.hpp>
#include <core/tools/failed.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <service/op.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/image.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::vtk::ut::image_reader_writer_test);

static const double EPSILON = 0.00001;

namespace sight::module::io::vtk::ut
{

//------------------------------------------------------------------------------

void run_image_srv(
    const std::string& _srvname,
    const boost::property_tree::ptree& _cfg,
    const SPTR(data::object)& _image
)
{
    service::base::sptr srv = service::add(_srvname);

    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + _srvname, srv);

    if(srv->is_a("sight::io::service::reader"))
    {
        srv->set_inout(_image, "data");
    }
    else
    {
        srv->set_input(_image, "data");
    }

    CPPUNIT_ASSERT_NO_THROW(srv->set_config(_cfg));
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::remove(srv);
}

//------------------------------------------------------------------------------

void image_reader_writer_test::setUp()
{
}

//------------------------------------------------------------------------------

void image_reader_writer_test::tearDown()
{
}

//------------------------------------------------------------------------------

boost::property_tree::ptree get_io_configuration(const std::filesystem::path& _file)
{
    service::config_t reader_srv_cfg;
    reader_srv_cfg.add("file", _file.string());

    return reader_srv_cfg;
}

//------------------------------------------------------------------------------

void image_reader_writer_test::test_vtk_image_reader()
{
    const std::filesystem::path file = utest_data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::image::sptr image = std::make_shared<data::image>();

    // Data expected
    data::image::spacing_t spacing_expected;
    spacing_expected[0] = 1.732;
    spacing_expected[1] = 1.732;
    spacing_expected[2] = 3.2;

    data::image::origin_t origin_expected;
    origin_expected[0] = 34.64;
    origin_expected[1] = 86.6;
    origin_expected[2] = 56;

    data::image::size_t size_expected;
    size_expected[0] = 230;
    size_expected[1] = 170;
    size_expected[2] = 58;

    run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image);

    // Data read.
    data::image::spacing_t spacing_read = image->spacing();
    data::image::origin_t origin_read   = image->origin();
    data::image::size_t size_read       = image->size();

    CPPUNIT_ASSERT_EQUAL(spacing_expected.size(), spacing_read.size());
    CPPUNIT_ASSERT_EQUAL(origin_expected.size(), origin_read.size());
    CPPUNIT_ASSERT_EQUAL(size_expected.size(), size_read.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacing_expected[0], spacing_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacing_expected[1], spacing_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacing_expected[2], spacing_read[2], EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", origin_expected[0], origin_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", origin_expected[1], origin_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", origin_expected[2], origin_read[2], EPSILON);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", size_expected[0], size_read[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", size_expected[1], size_read[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", size_expected[2], size_read[2]);
}

//------------------------------------------------------------------------------

void image_reader_writer_test::test_vti_image_reader()
{
    const std::filesystem::path file = utest_data::dir() / "sight/image/vti/BostonTeapot.vti";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::image::sptr image = std::make_shared<data::image>();
    run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image);

    // Data expected
    data::image::spacing_t spacing_expected;
    spacing_expected[0] = 1.0;
    spacing_expected[1] = 1.0;
    spacing_expected[2] = 1.0;

    data::image::origin_t origin_expected;
    origin_expected[0] = 1.1;
    origin_expected[1] = 2.2;
    origin_expected[2] = 3.3;

    data::image::size_t size_expected;
    size_expected[0] = 256;
    size_expected[1] = 256;
    size_expected[2] = 178;

    core::type expected_type("int8"); // MHD File image type : MET_CHAR

    // Data read.
    data::image::spacing_t spacing_read = image->spacing();
    data::image::origin_t origin_read   = image->origin();
    data::image::size_t size_read       = image->size();

    CPPUNIT_ASSERT_EQUAL(spacing_expected.size(), spacing_read.size());
    CPPUNIT_ASSERT_EQUAL(origin_expected.size(), origin_read.size());
    CPPUNIT_ASSERT_EQUAL(size_expected.size(), size_read.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacing_expected[0], spacing_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacing_expected[1], spacing_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacing_expected[2], spacing_read[2], EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", origin_expected[0], origin_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", origin_expected[1], origin_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", origin_expected[2], origin_read[2], EPSILON);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", size_expected[0], size_read[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", size_expected[1], size_read[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", size_expected[2], size_read[2]);

    CPPUNIT_ASSERT_EQUAL(expected_type, image->type());
}

//------------------------------------------------------------------------------
void image_reader_writer_test::test_mhd_image_reader()
{
    const std::filesystem::path file = utest_data::dir() / "sight/image/mhd/BostonTeapot.mhd";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::image::sptr image = std::make_shared<data::image>();
    run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image);

    // Data expected
    data::image::spacing_t spacing_expected;
    spacing_expected[0] = 1.0;
    spacing_expected[1] = 1.0;
    spacing_expected[2] = 1.0;

    data::image::origin_t origin_expected;
    origin_expected[0] = 1.1;
    origin_expected[1] = 2.2;
    origin_expected[2] = 3.3;

    data::image::size_t size_expected;
    size_expected[0] = 256;
    size_expected[1] = 256;
    size_expected[2] = 178;

    core::type expected_type("int8"); // MHD File image type : MET_CHAR

    // Data read.
    data::image::spacing_t spacing_read = image->spacing();
    data::image::origin_t origin_read   = image->origin();
    data::image::size_t size_read       = image->size();

    CPPUNIT_ASSERT_EQUAL(spacing_expected.size(), spacing_read.size());
    CPPUNIT_ASSERT_EQUAL(origin_expected.size(), origin_read.size());
    CPPUNIT_ASSERT_EQUAL(size_expected.size(), size_read.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacing_expected[0], spacing_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacing_expected[1], spacing_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacing_expected[2], spacing_read[2], EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", origin_expected[0], origin_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", origin_expected[1], origin_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", origin_expected[2], origin_read[2], EPSILON);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", size_expected[0], size_read[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", size_expected[1], size_read[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", size_expected[2], size_read[2]);

    CPPUNIT_ASSERT_EQUAL(expected_type, image->type());
}

//------------------------------------------------------------------------------

void image_reader_writer_test::test_image_reader_extension()
{
    core::os::temp_file tmp_file;

    std::ofstream o_file(tmp_file, std::ios::out | std::ios::trunc | std::ios::binary);
    o_file.close();

    data::image::sptr image = std::make_shared<data::image>();

    {
        const std::string srvname("sight::module::io::vtk::image_reader");

        service::base::sptr srv = service::add(srvname);

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->set_inout(image, "data");

        CPPUNIT_ASSERT_NO_THROW(srv->set_config(get_io_configuration(tmp_file)));
        CPPUNIT_ASSERT_NO_THROW(srv->configure());
        CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
        CPPUNIT_ASSERT_THROW(srv->update().get(), core::tools::failed);
        CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
        service::remove(srv);
    }
}

//------------------------------------------------------------------------------
void image_reader_writer_test::test_vtk_image_writer()
{
    // Data to write
    core::type type                         = core::type::UINT8;
    const data::image::size_t size_expected = {10, 20, 30
    };
    const data::image::spacing_t spacing_expected = {0.24, 1.07, 2.21
    };
    const data::image::origin_t origin_expected = {-5.6, 15.16, 11.11
    };

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_image(
        image,
        size_expected,
        spacing_expected,
        origin_expected,
        type,
        data::image::rgba,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmp_dir;
    const auto file = tmp_dir / "tempFile.vtk";

    run_image_srv("sight::module::io::vtk::image_writer", get_io_configuration(file), image);

    // Read image from disk
    data::image::sptr image_from_disk = std::make_shared<data::image>();
    run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_from_disk);

    // Data read
    data::image::spacing_t spacing_read = image->spacing();
    data::image::origin_t origin_read   = image->origin();
    data::image::size_t size_read       = image->size();

    CPPUNIT_ASSERT_EQUAL(spacing_expected.size(), spacing_read.size());
    CPPUNIT_ASSERT_EQUAL(origin_expected.size(), origin_read.size());
    CPPUNIT_ASSERT_EQUAL(size_expected.size(), size_read.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacing_expected[0], spacing_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacing_expected[1], spacing_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacing_expected[2], spacing_read[2], EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", origin_expected[0], origin_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", origin_expected[1], origin_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", origin_expected[2], origin_read[2], EPSILON);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", size_expected[0], size_read[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", size_expected[1], size_read[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", size_expected[2], size_read[2]);

    const auto image_dump_lock           = image->dump_lock();
    const auto image_from_disk_dump_lock = image_from_disk->dump_lock();

    const char* const ptr_on_generated_image = static_cast<char*>(image->buffer());
    const char* const ptr_on_read_image      = static_cast<char*>(image_from_disk->buffer());

    CPPUNIT_ASSERT_EQUAL(image->type(), image_from_disk->type());
    CPPUNIT_ASSERT(
        std::equal(
            ptr_on_generated_image,
            ptr_on_generated_image + image->size_in_bytes(),
            ptr_on_read_image
        )
    );
}

//------------------------------------------------------------------------------

void image_reader_writer_test::test_vtk_image_series_writer()
{
    core::type type   = core::type::FLOAT;
    auto image_series = std::make_shared<data::image_series>();
    utest_data::generator::image::generate_random_image(image_series, type);

    core::os::temp_dir tmp_dir;
    const auto file = tmp_dir / "imageSeries.vtk";

    // Write image series
    run_image_srv("sight::module::io::vtk::image_series_writer", get_io_configuration(file), image_series);

    // Read image series
    auto image_series2 = std::make_shared<data::image_series>();
    run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_series2);

    image_series2->set_window_center(image_series->window_center());
    image_series2->set_window_width(image_series->window_width());

    CPPUNIT_ASSERT(*image_series == *image_series2);
}

//------------------------------------------------------------------------------

void image_reader_writer_test::test_vti_image_writer()
{
    // Data to write
    core::type type                         = core::type::UINT8;
    const data::image::size_t size_expected = {10, 20, 30
    };
    const data::image::spacing_t spacing_expected = {0.24, 1.07, 2.21
    };
    const data::image::origin_t origin_expected = {-5.6, 15.16, 11.11
    };

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_image(
        image,
        size_expected,
        spacing_expected,
        origin_expected,
        type,
        data::image::gray_scale,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmp_dir;
    const auto file = tmp_dir / "tempFile.vti";

    run_image_srv("sight::module::io::vtk::image_writer", get_io_configuration(file), image);

    // Read image from disk
    data::image::sptr image_from_disk = std::make_shared<data::image>();
    run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_from_disk);

    // Data read
    data::image::spacing_t spacing_read = image->spacing();
    data::image::origin_t origin_read   = image->origin();
    data::image::size_t size_read       = image->size();

    CPPUNIT_ASSERT_EQUAL(spacing_expected.size(), spacing_read.size());
    CPPUNIT_ASSERT_EQUAL(origin_expected.size(), origin_read.size());
    CPPUNIT_ASSERT_EQUAL(size_expected.size(), size_read.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacing_expected[0], spacing_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacing_expected[1], spacing_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacing_expected[2], spacing_read[2], EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", origin_expected[0], origin_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", origin_expected[1], origin_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", origin_expected[2], origin_read[2], EPSILON);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", size_expected[0], size_read[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", size_expected[1], size_read[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", size_expected[2], size_read[2]);

    const auto image_dump_lock           = image->dump_lock();
    const auto image_from_disk_dump_lock = image_from_disk->dump_lock();

    const char* const ptr_on_generated_image = static_cast<char*>(image->buffer());
    const char* const ptr_on_read_image      = static_cast<char*>(image_from_disk->buffer());

    CPPUNIT_ASSERT_EQUAL(image->type(), image_from_disk->type());
    CPPUNIT_ASSERT(
        std::equal(
            ptr_on_generated_image,
            ptr_on_generated_image + image->size_in_bytes(),
            ptr_on_read_image
        )
    );
}

//------------------------------------------------------------------------------

void image_reader_writer_test::test_mhd_image_writer()
{
    // Data to write
    core::type type                         = core::type::UINT8;
    const data::image::size_t size_expected = {10, 20, 30
    };
    const data::image::spacing_t spacing_expected = {0.24, 1.07, 2.21
    };
    const data::image::origin_t origin_expected = {-5.6, 15.16, 11.11
    };

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_image(
        image,
        size_expected,
        spacing_expected,
        origin_expected,
        type,
        data::image::rgb,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmp_dir;
    const auto file = tmp_dir / "tempFile.mhd";

    run_image_srv("sight::module::io::vtk::image_writer", get_io_configuration(file), image);

    // Read image from disk
    data::image::sptr image_from_disk = std::make_shared<data::image>();
    run_image_srv("sight::module::io::vtk::image_reader", get_io_configuration(file), image_from_disk);

    // Data read
    data::image::spacing_t spacing_read = image->spacing();
    data::image::origin_t origin_read   = image->origin();
    data::image::size_t size_read       = image->size();

    CPPUNIT_ASSERT_EQUAL(spacing_expected.size(), spacing_read.size());
    CPPUNIT_ASSERT_EQUAL(origin_expected.size(), origin_read.size());
    CPPUNIT_ASSERT_EQUAL(size_expected.size(), size_read.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacing_expected[0], spacing_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacing_expected[1], spacing_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacing_expected[2], spacing_read[2], EPSILON);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", origin_expected[0], origin_read[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", origin_expected[1], origin_read[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", origin_expected[2], origin_read[2], EPSILON);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", size_expected[0], size_read[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", size_expected[1], size_read[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", size_expected[2], size_read[2]);

    const auto image_dump_lock           = image->dump_lock();
    const auto image_from_disk_dump_lock = image_from_disk->dump_lock();

    const char* const ptr_on_generated_image = static_cast<char*>(image->buffer());
    const char* const ptr_on_read_image      = static_cast<char*>(image_from_disk->buffer());

    CPPUNIT_ASSERT_EQUAL(image->type(), image_from_disk->type());
    CPPUNIT_ASSERT(
        std::equal(
            ptr_on_generated_image,
            ptr_on_generated_image + image->size_in_bytes(),
            ptr_on_read_image
        )
    );
}

//------------------------------------------------------------------------------

void image_reader_writer_test::test_image_writer_extension()
{
    // Data to write
    const auto type = core::type::UINT8;
    const data::image::size_t size_expected {10, 20, 30};
    const data::image::spacing_t spacing_expected {0.24, 1.07, 2.21};
    const data::image::origin_t origin_expected {-5.6, 15.16, 11.11};

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generate_image(
        image,
        size_expected,
        spacing_expected,
        origin_expected,
        type,
        data::image::gray_scale,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmp_dir;
    const auto file = tmp_dir / "tempFile.xxx";

    {
        const std::string srvname("sight::module::io::vtk::image_writer");

        service::base::sptr srv = service::add(srvname);

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->set_input(image, "data");
        CPPUNIT_ASSERT_NO_THROW(srv->set_config(get_io_configuration(file)));
        CPPUNIT_ASSERT_NO_THROW(srv->configure());
        CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
        CPPUNIT_ASSERT_THROW(srv->update().get(), core::tools::failed);
        CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
        service::remove(srv);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk::ut
