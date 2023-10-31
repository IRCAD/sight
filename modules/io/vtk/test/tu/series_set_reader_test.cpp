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

#include "series_set_reader_test.hpp"

#include <core/tools/system.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <utest_data/data.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::vtk::ut::series_set_reader_test);

static const double EPSILON = 0.00001;

namespace sight::module::io::vtk::ut
{

//------------------------------------------------------------------------------

void series_set_reader_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void series_set_reader_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void series_set_reader_test::test_series_set_reader()
{
    const std::filesystem::path image_file = utest_data::dir() / "sight/image/vtk/img.vtk";
    const std::filesystem::path mesh_file  = utest_data::dir() / "sight/mesh/vtk/sphere.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_file.string() + "' does not exist",
        std::filesystem::exists(image_file)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + mesh_file.string() + "' does not exist",
        std::filesystem::exists(mesh_file)
    );

    service::config_t reader_srv_cfg;
    reader_srv_cfg.add("file", image_file.string());
    service::config_t file2_cfg;
    reader_srv_cfg.add("file", mesh_file.string());
    reader_srv_cfg.add("file", mesh_file.string());

    auto series_set = std::make_shared<data::series_set>();

    service::base::sptr srv = service::add("sight::module::io::vtk::series_set_reader");

    CPPUNIT_ASSERT_MESSAGE("Create series_set_reader failed", srv);

    srv->set_inout(series_set, "data");
    srv->set_config(reader_srv_cfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::remove(srv);

    // Data expected
    const data::image::spacing_t spacing_expected = {1.732, 1.732, 3.2};
    const data::image::origin_t origin_expected   = {34.64, 86.6, 56};
    const data::image::size_t size_expected       = {230, 170, 58};

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    data::image_series::sptr image_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
    data::model_series::sptr model_series = std::dynamic_pointer_cast<data::model_series>(series_set->at(1));
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", image_series);
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", model_series);

    // Data read.
    const data::image::spacing_t spacing_read = image_series->spacing();
    const data::image::spacing_t origin_read  = image_series->origin();
    const data::image::size_t size_read       = image_series->size();

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

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), model_series->get_reconstruction_db().size());

    data::reconstruction::sptr rec1 = model_series->get_reconstruction_db()[0];
    data::reconstruction::sptr rec2 = model_series->get_reconstruction_db()[1];
    data::mesh::sptr mesh1          = rec1->get_mesh();
    data::mesh::sptr mesh2          = rec2->get_mesh();

    CPPUNIT_ASSERT_EQUAL((data::mesh::size_t) 720, mesh1->num_cells());
    CPPUNIT_ASSERT_EQUAL((data::mesh::size_t) 362, mesh1->num_points());

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void series_set_reader_test::test_merge_series_set_reader()
{
    const std::filesystem::path image_file = utest_data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + image_file.string() + "' does not exist",
        std::filesystem::exists(image_file)
    );

    service::config_t reader_srv_cfg;
    reader_srv_cfg.add("file", image_file.string());

    auto image_series = std::make_shared<data::image_series>();
    auto series_set   = std::make_shared<data::series_set>();
    series_set->push_back(image_series);

    service::base::sptr srv = service::add("sight::module::io::vtk::series_set_reader");

    CPPUNIT_ASSERT_MESSAGE("Create series_set_reader failed", srv);

    srv->set_inout(series_set, "data");
    srv->set_config(reader_srv_cfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::remove(srv);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
}

} // namespace sight::module::io::vtk::ut
