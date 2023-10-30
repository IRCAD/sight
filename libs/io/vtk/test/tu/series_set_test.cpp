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

#include "series_set_test.hpp"

#include <core/memory/buffer_manager.hpp>
#include <core/memory/buffer_object.hpp>

#include <data/array.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/series.hpp>

#include <io/vtk/SeriesSetReader.hpp>

#include <utest_data/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::series_set_test);

namespace sight::io::vtk::ut
{

//------------------------------------------------------------------------------

void series_set_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void series_set_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void series_set_test::test_import_series_set()
{
    auto series_set = std::make_shared<data::series_set>();

    const std::filesystem::path image_path(utest_data::dir() / "sight/image/vtk/img.vtk");
    const std::filesystem::path mesh_path(utest_data::dir() / "sight/mesh/vtk/sphere.vtk");

    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + image_path.string(), std::filesystem::exists(image_path));
    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + mesh_path.string(), std::filesystem::exists(mesh_path));

    std::vector<std::filesystem::path> paths;
    paths.push_back(image_path);
    paths.push_back(mesh_path);
    paths.push_back(mesh_path);

    io::vtk::series_set_reader::sptr reader = std::make_shared<io::vtk::series_set_reader>();
    reader->set_object(series_set);
    reader->set_files(paths);
    reader->read();

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    data::image_series::sptr img_series = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", img_series);

    data::model_series::sptr model_series = std::dynamic_pointer_cast<data::model_series>(series_set->at(1));
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", model_series);

    data::model_series::reconstruction_vector_t rec_vect = model_series->get_reconstruction_db();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), rec_vect.size());

    data::reconstruction::sptr rec1 = rec_vect.at(0);
    data::reconstruction::sptr rec2 = rec_vect.at(1);

    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec1->get_organ_name());
    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec2->get_organ_name());

    data::mesh::sptr mesh1 = rec1->get_mesh();
    data::mesh::sptr mesh2 = rec2->get_mesh();

    CPPUNIT_ASSERT_EQUAL(mesh1->num_cells(), (data::mesh::size_t) 720);
    CPPUNIT_ASSERT_EQUAL(mesh1->num_points(), (data::mesh::size_t) 362);

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

bool is_loaded(core::memory::buffer_object::sptr _bo)
{
    core::memory::buffer_manager::csptr manager                     = core::memory::buffer_manager::get();
    const core::memory::buffer_manager::buffer_info_map_t map_infos = manager->get_buffer_infos().get();

    auto iter = map_infos.find(_bo->get_buffer_pointer());
    CPPUNIT_ASSERT_MESSAGE("buffer_info not found.", iter != map_infos.end());
    const core::memory::buffer_info& info = iter->second;

    return info.loaded;
}

} // namespace sight::io::vtk::ut
