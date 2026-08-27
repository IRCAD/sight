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

#include <core/notification/observer.hpp>

#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <io/vtk/series_set_reader.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::io::vtk::series_set")
{
    TEST_CASE("import_series_set")
    {
        auto series_set = std::make_shared<sight::data::series_set>();

        const std::filesystem::path image_path(sight::utest_data::dir() / "sight/image/vtk/img.vtk");
        const std::filesystem::path mesh_path(sight::utest_data::dir() / "sight/mesh/vtk/sphere.vtk");

        CHECK_MESSAGE(std::filesystem::exists(image_path), "Missing file: ", image_path.string());
        CHECK_MESSAGE(std::filesystem::exists(mesh_path), "Missing file: ", mesh_path.string());

        std::vector<std::filesystem::path> paths;
        paths.push_back(image_path);
        paths.push_back(mesh_path);
        paths.push_back(mesh_path);

        auto reader = std::make_shared<sight::io::vtk::series_set_reader>();
        reader->set_object(series_set);
        reader->set_files(paths);
        auto read_observer = std::make_shared<sight::core::notification::observer>("Test read");
        reader->read(read_observer);

        CHECK_EQ(series_set->size(), std::size_t(2));

        auto img_series = std::dynamic_pointer_cast<sight::data::image_series>(series_set->at(0));
        CHECK_MESSAGE(img_series, "ImageSeries dynamicCast failed");

        auto model_series = std::dynamic_pointer_cast<sight::data::model_series>(series_set->at(1));
        CHECK_MESSAGE(model_series, "ModelSeries dynamicCast failed");

        auto rec_vect = model_series->get_reconstruction_db();
        CHECK_EQ(rec_vect.size(), std::size_t(2));

        const auto& rec1 = rec_vect.at(0);
        const auto& rec2 = rec_vect.at(1);

        CHECK_EQ(rec1->get_organ_name(), std::string("sphere"));
        CHECK_EQ(rec2->get_organ_name(), std::string("sphere"));

        auto mesh1 = rec1->get_mesh();
        auto mesh2 = rec2->get_mesh();

        CHECK_EQ(mesh1->num_cells(), (sight::data::mesh::size_t) 720);
        CHECK_EQ(mesh1->num_points(), (sight::data::mesh::size_t) 362);

        CHECK(*mesh1 == *mesh2);
    }
} // TEST_SUITE
