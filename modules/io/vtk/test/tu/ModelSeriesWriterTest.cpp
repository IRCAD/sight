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

#include "ModelSeriesWriterTest.hpp"

#include <core/os/temp_path.hpp>

#include <data/activity_set.hpp>
#include <data/array.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <utest_data/generator/series_set.hpp>

#include <filesystem>
#include <string>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::vtk::ut::ModelSeriesWriterTest);

namespace sight::module::io::vtk::ut
{

namespace fs           = std::filesystem;
using file_container_t = std::vector<std::string>;

namespace point = sight::data::iterator::point;
namespace cell  = sight::data::iterator::cell;

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void run_model_series_srv(
    const std::string& _impl,
    const boost::property_tree::ptree& _cfg,
    const SPTR(data::object)& _obj
)
{
    service::base::sptr srv = service::add(_impl);

    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + _impl, srv);

    if(srv->is_a("sight::io::service::reader"))
    {
        srv->set_inout(_obj, "data");
    }
    else
    {
        srv->set_input(_obj, "data");
    }

    CPPUNIT_ASSERT_NO_THROW(srv->set_config(_cfg));
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::remove(srv);
}

//------------------------------------------------------------------------------

boost::property_tree::ptree get_io_cfg_from_folder(const fs::path& _file)
{
    service::config_t srv_cfg;
    srv_cfg.add("folder", _file.string());

    return srv_cfg;
}

//------------------------------------------------------------------------------

boost::property_tree::ptree get_io_cfg_from_files(const file_container_t& _files)
{
    service::config_t srv_cfg;
    for(const auto& file : _files)
    {
        srv_cfg.add("file", file);
    }

    return srv_cfg;
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::testWriteMeshes()
{
    data::model_series::sptr model_series = utest_data::generator::series_set::createModelSeries(5);

    const std::vector<std::string> all_extensions = {"vtk", "vtp", "obj", "ply", "stl"};

    core::os::temp_dir tmp_dir;

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
        std::sort(files.begin(), files.end());

        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Number of saved files",
            model_series->getReconstructionDB().size(),
            files.size()
        );

        auto series_set = std::make_shared<data::series_set>();

        run_model_series_srv(
            "sight::module::io::vtk::series_set_reader",
            get_io_cfg_from_files(files),
            series_set
        );

        CPPUNIT_ASSERT_EQUAL_MESSAGE("series_set Size", (std::size_t) 1, series_set->size());

        data::model_series::sptr read_series = std::dynamic_pointer_cast<data::model_series>(series_set->at(0));
        CPPUNIT_ASSERT_MESSAGE("A ModelSeries was expected", read_series);

        using rec_vec_t = data::model_series::reconstruction_vector_t;
        const rec_vec_t& read_recs = read_series->getReconstructionDB();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of reconstructions", files.size(), read_recs.size());

        const rec_vec_t& ref_recs = model_series->getReconstructionDB();
        auto it_ref               = ref_recs.begin();
        auto it_read              = read_recs.begin();

        for( ; it_ref != ref_recs.end() ; ++it_ref, ++it_read)
        {
            data::mesh::csptr ref_mesh  = (*it_ref)->getMesh();
            data::mesh::csptr read_mesh = (*it_read)->getMesh();

            const auto reflock        = ref_mesh->dump_lock();
            const auto read_mesh_lock = read_mesh->dump_lock();

            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "Number of Points.",
                ref_mesh->numPoints(),
                read_mesh->numPoints()
            );
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "Number of Cells.",
                ref_mesh->numCells(),
                read_mesh->numCells()
            );

            // Don't test internal structures for obj, ply and stl, since some of them are missing.
            if(ext != "obj" && ext != "ply" && ext != "stl")
            {
                const auto ref_points  = ref_mesh->czip_range<point::xyz, point::nxyz, point::rgba>();
                const auto read_points = read_mesh->czip_range<point::xyz, point::nxyz, point::rgba>();

                for(const auto& [ref, read] : boost::combine(ref_points, read_points))
                {
                    const auto& [pt1, n1, c1] = ref;
                    const auto& [pt2, n2, c2] = read;

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point.x ", pt1.x, pt2.x, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point.y", pt1.y, pt2.y, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point.z", pt1.z, pt2.z, 0.00001);

                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color R", c1.r, c2.r);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color G", c1.g, c2.g);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color B", c1.b, c2.b);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Point color A", c1.a, c2.a);

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point normal x", n1.nx, n2.nx, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point normal y", n1.ny, n2.ny, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Point normal z", n1.nz, n2.nz, 0.00001);
                }

                const auto ref_cells  = ref_mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba>();
                const auto read_cells = read_mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba>();

                for(const auto& [ref, read] : boost::combine(ref_cells, read_cells))
                {
                    const auto& [tri1, n1, c1] = ref;
                    const auto& [tri2, n2, c2] = read;

                    for(std::size_t i = 0 ; i < 3 ; ++i)
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell point index", tri1.pt[i], tri2.pt[i]);
                    }

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Cell normal x", n1.nx, n2.nx, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Cell normal y", n1.ny, n2.ny, 0.00001);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Cell normal z", n1.nz, n2.nz, 0.00001);

                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color R", c1.r, c2.r);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color G", c1.g, c2.g);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color B", c1.b, c2.b);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell color A", c1.a, c2.a);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ModelSeriesWriterTest::testWriteReconstructions()
{
    data::model_series::sptr model_series = utest_data::generator::series_set::createModelSeries(5);

    core::os::temp_dir tmp_dir;

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
    CPPUNIT_ASSERT_EQUAL(model_series->getReconstructionDB().size() * 2, files.size());
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk::ut
