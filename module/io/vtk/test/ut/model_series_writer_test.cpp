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

#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <utest_data/generator/series_set.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <filesystem>
#include <string>
#include <vector>

using file_container_t = std::vector<std::string>;
namespace fs           = std::filesystem;

//------------------------------------------------------------------------------
static void run_model_series_srv(
    const std::string& _impl,
    const boost::property_tree::ptree& _cfg,
    const sight::data::object::sptr& _obj
)
{
    sight::service::base::sptr srv = sight::service::add(_impl);

    CHECK_MESSAGE(srv, "Failed to create service ", _impl);

    if(srv->is_a("sight::io::service::reader"))
    {
        srv->set_inout(_obj, "data.read");
    }
    else
    {
        srv->set_input(_obj, "data.write");
    }

    CHECK_NOTHROW(srv->set_config(_cfg));
    CHECK_NOTHROW(srv->configure());
    CHECK_NOTHROW(srv->start().get());
    CHECK_NOTHROW(srv->update().get());
    CHECK_NOTHROW(srv->stop().get());
    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

static boost::property_tree::ptree get_io_cfg_from_folder(const fs::path& _file)
{
    sight::service::config_t srv_cfg;
    srv_cfg.add("path.<xmlattr>.folder", _file.string());

    return srv_cfg;
}

//------------------------------------------------------------------------------

static boost::property_tree::ptree get_io_cfg_from_files(const file_container_t& _files)
{
    sight::service::config_t srv_cfg;
    srv_cfg.add("path.<xmlattr>.file", boost::algorithm::join(_files, ";"));

    return srv_cfg;
}

TEST_SUITE("sight::module::io::vtk::model_series_writer")
{
//------------------------------------------------------------------------------

    namespace point = sight::data::iterator::point;
    namespace cell  = sight::data::iterator::cell;

//------------------------------------------------------------------------------

    TEST_CASE("write_meshes")
    {
        sight::data::model_series::sptr model_series = sight::utest_data::generator::series_set::create_model_series(5);

        const std::vector<std::string> all_extensions = {"vtk", "vtp", "obj", "ply", "stl"};

        sight::core::os::temp_dir tmp_dir;

        for(const auto& ext : all_extensions)
        {
            const auto& ext_dir = tmp_dir / ext;
            fs::create_directories(ext_dir);

            auto cfg = get_io_cfg_from_folder(ext_dir);
            cfg.add("extension", ext);

            run_model_series_srv(
                "sight::module::io::vtk::model_series_writer",
                cfg,
                model_series
            );

            file_container_t files;
            for(fs::directory_iterator it(ext_dir) ; it != fs::directory_iterator() ; ++it)
            {
                if(it->path().extension() == "." + ext)
                {
                    files.push_back(it->path().string());
                }
            }

            // Ensure reading order (modelSeries generator will prefix each file with a number).
            std::ranges::sort(files);

            CHECK_EQ(model_series->get_reconstruction_db().size(), files.size());

            auto series_set = std::make_shared<sight::data::series_set>();

            run_model_series_srv(
                "sight::module::io::vtk::series_set_reader",
                get_io_cfg_from_files(files),
                series_set
            );

            CHECK_EQ(static_cast<std::size_t>(1), series_set->size());

            sight::data::model_series::sptr read_series =
                std::dynamic_pointer_cast<sight::data::model_series>(series_set->at(0));
            CHECK(read_series);

            using rec_vec_t = sight::data::model_series::reconstruction_vector_t;
            const rec_vec_t& read_recs = read_series->get_reconstruction_db();
            CHECK_EQ(files.size(), read_recs.size());

            const rec_vec_t& ref_recs = model_series->get_reconstruction_db();
            auto it_ref               = ref_recs.begin();
            auto it_read              = read_recs.begin();

            for( ; it_ref != ref_recs.end() ; ++it_ref, ++it_read)
            {
                sight::data::mesh::csptr ref_mesh  = (*it_ref)->get_mesh();
                sight::data::mesh::csptr read_mesh = (*it_read)->get_mesh();

                const auto reflock        = ref_mesh->dump_lock();
                const auto read_mesh_lock = read_mesh->dump_lock();

                CHECK_EQ(ref_mesh->num_points(), read_mesh->num_points());
                CHECK_EQ(ref_mesh->num_cells(), read_mesh->num_cells());

                // Don't test internal structures for obj, ply and stl, since some of them are missing.
                if(ext != "obj" && ext != "ply" && ext != "stl")
                {
                    const auto ref_points  = ref_mesh->czip_range<point::xyz, point::nxyz, point::rgba>();
                    const auto read_points = read_mesh->czip_range<point::xyz, point::nxyz, point::rgba>();

                    for(const auto& [ref, read] : boost::combine(ref_points, read_points))
                    {
                        const auto& [pt1, n1, c1] = ref;
                        const auto& [pt2, n2, c2] = read;

                        CHECK((std::abs(pt1.x - pt2.x) < 0.00001));
                        CHECK((std::abs(pt1.y - pt2.y) < 0.00001));
                        CHECK((std::abs(pt1.z - pt2.z) < 0.00001));

                        CHECK_EQ(c1.r, c2.r);
                        CHECK_EQ(c1.g, c2.g);
                        CHECK_EQ(c1.b, c2.b);
                        CHECK_EQ(c1.a, c2.a);

                        CHECK((std::abs(n1.nx - n2.nx) < 0.00001));
                        CHECK((std::abs(n1.ny - n2.ny) < 0.00001));
                        CHECK((std::abs(n1.nz - n2.nz) < 0.00001));
                    }

                    const auto ref_cells  = ref_mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba>();
                    const auto read_cells = read_mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba>();

                    for(const auto& [ref, read] : boost::combine(ref_cells, read_cells))
                    {
                        const auto& [tri1, n1, c1] = ref;
                        const auto& [tri2, n2, c2] = read;

                        for(std::size_t i = 0 ; i < 3 ; ++i)
                        {
                            CHECK_EQ(tri1.pt[i], tri2.pt[i]);
                        }

                        CHECK((std::abs(n1.nx - n2.nx) < 0.00001));
                        CHECK((std::abs(n1.ny - n2.ny) < 0.00001));
                        CHECK((std::abs(n1.nz - n2.nz) < 0.00001));

                        CHECK_EQ(c1.r, c2.r);
                        CHECK_EQ(c1.g, c2.g);
                        CHECK_EQ(c1.b, c2.b);
                        CHECK_EQ(c1.a, c2.a);
                    }
                }
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("write_reconstructions")
    {
        sight::data::model_series::sptr model_series = sight::utest_data::generator::series_set::create_model_series(5);

        sight::core::os::temp_dir tmp_dir;

        run_model_series_srv(
            "sight::module::io::vtk::model_series_obj_writer",
            get_io_cfg_from_folder(tmp_dir),
            model_series
        );

        file_container_t files;
        for(fs::directory_iterator it(tmp_dir) ; it != fs::directory_iterator() ; ++it)
        {
            files.push_back(it->path().string());
        }

        // Writer generates a .mtl file for each .obj file
        CHECK_EQ(model_series->get_reconstruction_db().size() * 2, files.size());
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
