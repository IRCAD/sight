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

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

#include <filesystem>

TEST_SUITE("sight::module::io::vtk::series_set_reader")
{
//------------------------------------------------------------------------------

    TEST_CASE("series_set_reader")
    {
        const std::filesystem::path image_file = sight::utest_data::dir() / "sight/image/vtk/img.vtk";
        const std::filesystem::path mesh_file  = sight::utest_data::dir() / "sight/mesh/vtk/sphere.vtk";

        CHECK(
            std::filesystem::exists(image_file)
        );

        CHECK(
            std::filesystem::exists(mesh_file)
        );

        sight::service::config_t reader_srv_cfg;
        reader_srv_cfg.add("file", image_file.string());
        sight::service::config_t file2_cfg;
        reader_srv_cfg.add("file", mesh_file.string());
        reader_srv_cfg.add("file", mesh_file.string());

        auto series_set = std::make_shared<sight::data::series_set>();

        sight::service::base::sptr srv = sight::service::add("sight::module::io::vtk::series_set_reader");

        CHECK(srv);

        srv->set_inout(series_set, "data");
        srv->set_config(reader_srv_cfg);
        srv->configure();
        srv->start().get();
        srv->update().get();
        srv->stop().get();
        sight::service::remove(srv);

        // Data expected
        // NOLINTNEXTLINE(modernize-use-std-numbers)
        const sight::data::image::spacing_t spacing_expected = {1.732, 1.732, 3.2};
        const sight::data::image::origin_t origin_expected   = {34.64, 86.6, 56};
        const sight::data::image::size_t size_expected       = {230, 170, 58};

        CHECK_EQ(std::size_t(2), series_set->size());

        sight::data::image_series::sptr image_series =
            std::dynamic_pointer_cast<sight::data::image_series>(series_set->at(0));
        sight::data::model_series::sptr model_series =
            std::dynamic_pointer_cast<sight::data::model_series>(series_set->at(1));
        CHECK(image_series);
        CHECK(model_series);

        // Data read.
        const sight::data::image::spacing_t spacing_read = image_series->spacing();
        const sight::data::image::spacing_t origin_read  = image_series->origin();
        const sight::data::image::size_t size_read       = image_series->size();

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

        CHECK_EQ(std::size_t(2), model_series->get_reconstruction_db().size());

        sight::data::reconstruction::sptr rec1 = model_series->get_reconstruction_db()[0];
        sight::data::reconstruction::sptr rec2 = model_series->get_reconstruction_db()[1];
        sight::data::mesh::sptr mesh1          = rec1->get_mesh();
        sight::data::mesh::sptr mesh2          = rec2->get_mesh();

        CHECK_EQ((sight::data::mesh::size_t) 720, mesh1->num_cells());
        CHECK_EQ((sight::data::mesh::size_t) 362, mesh1->num_points());

        CHECK(*mesh1 == *mesh2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("merge_series_set_reader")
    {
        const std::filesystem::path image_file = sight::utest_data::dir() / "sight/image/vtk/img.vtk";

        CHECK(
            std::filesystem::exists(image_file)
        );

        sight::service::config_t reader_srv_cfg;
        reader_srv_cfg.add("file", image_file.string());

        auto image_series = std::make_shared<sight::data::image_series>();
        auto series_set   = std::make_shared<sight::data::series_set>();
        series_set->push_back(image_series);

        sight::service::base::sptr srv = sight::service::add("sight::module::io::vtk::series_set_reader");

        CHECK(srv);

        srv->set_inout(series_set, "data");
        srv->set_config(reader_srv_cfg);
        srv->configure();
        srv->start().get();
        srv->update().get();
        srv->stop().get();
        sight::service::remove(srv);

        CHECK_EQ(std::size_t(1), series_set->size());
    }
} // TEST_SUITE
