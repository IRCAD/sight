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
#include <core/tools/random/generator.hpp>

#include <geometry/data/mesh.hpp>

#include <io/vtk/helper/mesh.hpp>
#include <io/vtk/mesh_reader.hpp>
#include <io/vtk/mesh_writer.hpp>
#include <io/vtk/obj_mesh_reader.hpp>
#include <io/vtk/obj_mesh_writer.hpp>
#include <io/vtk/ply_mesh_reader.hpp>
#include <io/vtk/ply_mesh_writer.hpp>
#include <io/vtk/stl_mesh_reader.hpp>
#include <io/vtk/stl_mesh_writer.hpp>
#include <io/vtk/vtp_mesh_reader.hpp>
#include <io/vtk/vtp_mesh_writer.hpp>

#include <utest_data/data.hpp>
#include <utest_data/generator/mesh.hpp>

#include <doctest/doctest.h>

#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkTexturedSphereSource.h>

TEST_SUITE("sight::io::vtk::mesh")
{
    using sight::core::tools::random::safe_rand;

//------------------------------------------------------------------------------

    TEST_CASE("mesh_to_vtk")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        CHECK(mesh1);

        const vtkSmartPointer<vtkTexturedSphereSource> source = vtkSmartPointer<vtkTexturedSphereSource>::New();
        source->SetThetaResolution(50);
        source->SetPhiResolution(50);
        source->SetRadius(2);
        source->Update();
        const vtkSmartPointer<vtkPolyData> poly_source = source->GetOutput();

        CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh1->num_cells());
        CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh1->num_points());

        sight::io::vtk::helper::mesh::from_vtk_mesh(poly_source, mesh1);

        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);

        CHECK(mesh1->num_cells());
        CHECK(mesh1->num_points());
        CHECK(mesh1->has<sight::data::mesh::attribute::point_normals>());
        CHECK(mesh1->has<sight::data::mesh::attribute::cell_normals>());
        CHECK(mesh1->has<sight::data::mesh::attribute::point_colors>());
        CHECK(mesh1->has<sight::data::mesh::attribute::cell_colors>());
        CHECK(mesh1->has<sight::data::mesh::attribute::point_tex_coords>());

        const vtkSmartPointer<vtkPolyData> vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        sight::io::vtk::helper::mesh::to_vtk_mesh(mesh1, vtk_mesh);
        CHECK(vtk_mesh);

        CHECK_EQ(poly_source->GetNumberOfVerts(), vtk_mesh->GetNumberOfVerts());
        CHECK_EQ(poly_source->GetNumberOfLines(), vtk_mesh->GetNumberOfLines());
        CHECK_EQ(poly_source->GetNumberOfPolys(), vtk_mesh->GetNumberOfPolys());
        CHECK_EQ(poly_source->GetNumberOfStrips(), vtk_mesh->GetNumberOfStrips());

        const sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
        CHECK(mesh2);
        sight::io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh2);

        CHECK(*mesh1 == *mesh2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("mesh_to_grid")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        CHECK(mesh1);

        const vtkSmartPointer<vtkTexturedSphereSource> source = vtkSmartPointer<vtkTexturedSphereSource>::New();
        source->SetThetaResolution(50);
        source->SetPhiResolution(50);
        source->SetRadius(2);
        source->Update();
        const vtkSmartPointer<vtkPolyData> poly_source = source->GetOutput();

        CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh1->num_cells());
        CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh1->num_points());

        sight::io::vtk::helper::mesh::from_vtk_mesh(poly_source, mesh1);

        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);

        CHECK(mesh1->num_cells());
        CHECK(mesh1->num_points());
        CHECK(mesh1->has<sight::data::mesh::attribute::point_normals>());
        CHECK(mesh1->has<sight::data::mesh::attribute::cell_normals>());
        CHECK(mesh1->has<sight::data::mesh::attribute::point_colors>());
        CHECK(mesh1->has<sight::data::mesh::attribute::cell_colors>());
        CHECK(mesh1->has<sight::data::mesh::attribute::point_tex_coords>());
        const vtkSmartPointer<vtkUnstructuredGrid> vtk_grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        sight::io::vtk::helper::mesh::to_vtk_grid(mesh1, vtk_grid);
        CHECK(vtk_grid);

        CHECK_EQ(poly_source->GetNumberOfPoints(), vtk_grid->GetNumberOfPoints());

        const sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
        CHECK(mesh2);
        sight::io::vtk::helper::mesh::from_vtk_grid(vtk_grid, mesh2);

        CHECK(*mesh1 == *mesh2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("synthetic_mesh")
    {
        {
            const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_quad_mesh(mesh1);
            sight::geometry::data::mesh::shake_point(mesh1);
            mesh1->shrink_to_fit();

            const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
            sight::io::vtk::helper::mesh::to_vtk_mesh(mesh1, poly);
            CHECK(poly);

            sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
            sight::io::vtk::helper::mesh::from_vtk_mesh(poly, mesh2);

            CHECK(*mesh1 == *mesh2);
        }
        {
            const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_quad_mesh(mesh1);
            sight::geometry::data::mesh::shake_point(mesh1);
            sight::geometry::data::mesh::colorize_mesh_points(mesh1);
            sight::geometry::data::mesh::colorize_mesh_cells(mesh1);
            sight::geometry::data::mesh::generate_point_normals(mesh1);
            sight::geometry::data::mesh::generate_cell_normals(mesh1);
            mesh1->shrink_to_fit();

            const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
            sight::io::vtk::helper::mesh::to_vtk_mesh(mesh1, poly);
            CHECK(poly);

            sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
            sight::io::vtk::helper::mesh::from_vtk_mesh(poly, mesh2);

            CHECK(*mesh1 == *mesh2);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("export_import_synthetic_mesh")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_triangle_quad_mesh(mesh1);
        sight::geometry::data::mesh::shake_point(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);
        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);

        mesh1->shrink_to_fit();

        sight::core::os::temp_dir tmp_dir;
        const auto test_file = tmp_dir / "testExportImportSyntheticMesh.vtk";

        const sight::io::vtk::mesh_writer::sptr writer = std::make_shared<sight::io::vtk::mesh_writer>();
        writer->set_object(mesh1);
        writer->set_file(test_file);
        auto write_observer = std::make_shared<sight::core::notification::observer>("Test write");
        writer->write(write_observer);
        CHECK(std::filesystem::exists(test_file));

        const sight::data::mesh::sptr mesh2            = std::make_shared<sight::data::mesh>();
        const sight::io::vtk::mesh_reader::sptr reader = std::make_shared<sight::io::vtk::mesh_reader>();
        reader->set_object(mesh2);
        reader->set_file(test_file);
        auto read_observer = std::make_shared<sight::core::notification::observer>("Test read");
        reader->read(read_observer);

        CHECK(*mesh1 == *mesh2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("point_cloud")
    {
        const auto nb_points = static_cast<sight::data::mesh::size_t>(100 + safe_rand() % 1000);

        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();

        mesh1->reserve(nb_points, nb_points, sight::data::mesh::cell_type_t::point);

        const auto dump_lock = mesh1->dump_lock();

        for(sight::data::mesh::size_t i = 0 ; i < nb_points ; ++i)
        {
            std::array<sight::data::mesh::position_t, 3> point {};
            point[0] = (static_cast<float>(safe_rand() % 1000) - 500.F) / 3.F;
            point[1] = (static_cast<float>(safe_rand() % 1000) - 500.F) / 3.F;
            point[2] = (static_cast<float>(safe_rand() % 1000) - 500.F) / 3.F;
            mesh1->push_point(point);
            mesh1->push_cell(i);
        }

        const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
        sight::io::vtk::helper::mesh::to_vtk_mesh(mesh1, poly);
        CHECK(poly);

        const sight::data::mesh::sptr mesh2 = std::make_shared<sight::data::mesh>();
        sight::io::vtk::helper::mesh::from_vtk_mesh(poly, mesh2);

        CHECK_EQ(nb_points, mesh2->num_points());
        CHECK_EQ(nb_points, mesh2->num_cells());
        CHECK(*mesh1 == *mesh2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("read_vtk_file")
    {
        const std::filesystem::path test_file(sight::utest_data::dir() / "sight/mesh/vtk/sphere.vtk");
        CHECK_MESSAGE(
            std::filesystem::exists(test_file),
            "The file '",
            test_file.string(),
            "' does not exist"
        );

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        sight::io::vtk::mesh_reader::sptr vtk_reader = std::make_shared<sight::io::vtk::mesh_reader>();

        vtk_reader->set_object(mesh);
        vtk_reader->set_file(test_file);

        auto read_observer = std::make_shared<sight::core::notification::observer>("Test read");
        CHECK_NOTHROW(vtk_reader->read(read_observer));

        CHECK(mesh->num_cells() == 720);
        CHECK(mesh->num_points() == 362);
    }

//------------------------------------------------------------------------------

// FIXME: This test is skipped at runtime on linux until vtk is patched upstream.
// see https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=1064762
#ifdef WIN32
    TEST_CASE("read_vtp_file")
#else
    TEST_CASE("read_vtp_file" * doctest::skip())
#endif
    {
        const std::filesystem::path test_file(sight::utest_data::dir() / "sight/mesh/vtp/sphere.vtp");
        CHECK_MESSAGE(
            std::filesystem::exists(test_file),
            "The file '",
            test_file.string(),
            "' does not exist"
        );

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        sight::io::vtk::vtp_mesh_reader::sptr vtk_reader = std::make_shared<sight::io::vtk::vtp_mesh_reader>();

        vtk_reader->set_object(mesh);
        vtk_reader->set_file(test_file);

        auto read_observer = std::make_shared<sight::core::notification::observer>("Test read");
        CHECK_NOTHROW(vtk_reader->read(read_observer));

        CHECK(mesh->num_cells() == 720);
        CHECK(mesh->num_points() == 362);
    }

//------------------------------------------------------------------------------

    TEST_CASE("read_obj_file")
    {
        const std::filesystem::path test_file(sight::utest_data::dir() / "sight/mesh/obj/sphere.obj");
        CHECK_MESSAGE(
            std::filesystem::exists(test_file),
            "The file '",
            test_file.string(),
            "' does not exist"
        );

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        sight::io::vtk::obj_mesh_reader::sptr vtk_reader = std::make_shared<sight::io::vtk::obj_mesh_reader>();

        vtk_reader->set_object(mesh);
        vtk_reader->set_file(test_file);

        auto read_observer = std::make_shared<sight::core::notification::observer>("Test read");
        CHECK_NOTHROW(vtk_reader->read(read_observer));

        CHECK(mesh->num_cells() == 720);
        CHECK(mesh->num_points() == 362);
    }

//------------------------------------------------------------------------------

    TEST_CASE("read_ply_file")
    {
        const std::filesystem::path test_file(sight::utest_data::dir() / "sight/mesh/ply/sphere.ply");
        CHECK_MESSAGE(
            std::filesystem::exists(test_file),
            "The file '",
            test_file.string(),
            "' does not exist"
        );

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        sight::io::vtk::ply_mesh_reader::sptr vtk_reader = std::make_shared<sight::io::vtk::ply_mesh_reader>();

        vtk_reader->set_object(mesh);
        vtk_reader->set_file(test_file);

        auto read_observer = std::make_shared<sight::core::notification::observer>("Test read");
        CHECK_NOTHROW(vtk_reader->read(read_observer));

        CHECK(mesh->num_cells() == 720);
        CHECK(mesh->num_points() == 362);
    }

//------------------------------------------------------------------------------

    TEST_CASE("read_stl_file")
    {
        const std::filesystem::path test_file(sight::utest_data::dir() / "sight/mesh/stl/sphere.stl");
        CHECK_MESSAGE(
            std::filesystem::exists(test_file),
            "The file '",
            test_file.string(),
            "' does not exist"
        );

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        sight::io::vtk::stl_mesh_reader::sptr vtk_reader = std::make_shared<sight::io::vtk::stl_mesh_reader>();

        vtk_reader->set_object(mesh);
        vtk_reader->set_file(test_file);

        auto read_observer = std::make_shared<sight::core::notification::observer>("Test read");
        CHECK_NOTHROW(vtk_reader->read(read_observer));

        CHECK(mesh->num_cells() == 720);
        CHECK(mesh->num_points() == 362);
    }

//------------------------------------------------------------------------------

    TEST_CASE("write_vtk_file")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_triangle_mesh(mesh1);
        sight::geometry::data::mesh::shake_point(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);
        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);

        mesh1->shrink_to_fit();

        sight::core::os::temp_dir tmp_dir;
        const auto test_file = tmp_dir / "vtkTestMesh.vtk";

        const auto writer = std::make_shared<sight::io::vtk::mesh_writer>();
        writer->set_object(mesh1);
        writer->set_file(test_file);
        auto write_observer = std::make_shared<sight::core::notification::observer>("Test write");
        CHECK_NOTHROW(writer->write(write_observer));
        CHECK(std::filesystem::exists(test_file));
    }

//------------------------------------------------------------------------------

    TEST_CASE("write_vtp_file")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_triangle_mesh(mesh1);
        sight::geometry::data::mesh::shake_point(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);
        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);

        mesh1->shrink_to_fit();

        sight::core::os::temp_dir tmp_dir;
        const auto test_file = tmp_dir / "vtpTestMesh.vtp";

        const auto writer = std::make_shared<sight::io::vtk::vtp_mesh_writer>();
        writer->set_object(mesh1);
        writer->set_file(test_file);
        auto write_observer = std::make_shared<sight::core::notification::observer>("Test write");
        CHECK_NOTHROW(writer->write(write_observer));
        CHECK(std::filesystem::exists(test_file));
    }

//------------------------------------------------------------------------------

    TEST_CASE("write_obj_file")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_triangle_mesh(mesh1);
        sight::geometry::data::mesh::shake_point(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);
        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);

        mesh1->shrink_to_fit();

        sight::core::os::temp_dir tmp_dir;
        const auto test_file = tmp_dir / "objTestMesh.obj";

        const auto writer = std::make_shared<sight::io::vtk::obj_mesh_writer>();
        writer->set_object(mesh1);
        writer->set_file(test_file);
        auto write_observer = std::make_shared<sight::core::notification::observer>("Test write");
        CHECK_NOTHROW(writer->write(write_observer));
        CHECK(std::filesystem::exists(test_file));
    }

//------------------------------------------------------------------------------

    TEST_CASE("write_ply_file")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_triangle_mesh(mesh1);
        sight::geometry::data::mesh::shake_point(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);
        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);

        mesh1->shrink_to_fit();

        sight::core::os::temp_dir tmp_dir;
        const auto test_file = tmp_dir / "plyTestMesh.ply";

        const auto writer = std::make_shared<sight::io::vtk::ply_mesh_writer>();
        writer->set_object(mesh1);
        writer->set_file(test_file);
        auto write_observer = std::make_shared<sight::core::notification::observer>("Test write");
        CHECK_NOTHROW(writer->write(write_observer));
        CHECK(std::filesystem::exists(test_file));
    }

//------------------------------------------------------------------------------

    TEST_CASE("write_stl_file")
    {
        const sight::data::mesh::sptr mesh1 = std::make_shared<sight::data::mesh>();
        sight::utest_data::generator::mesh::generate_triangle_mesh(mesh1);
        sight::geometry::data::mesh::shake_point(mesh1);
        sight::geometry::data::mesh::colorize_mesh_points(mesh1);
        sight::geometry::data::mesh::colorize_mesh_cells(mesh1);
        sight::geometry::data::mesh::generate_point_normals(mesh1);
        sight::geometry::data::mesh::generate_cell_normals(mesh1);

        mesh1->shrink_to_fit();

        sight::core::os::temp_dir tmp_dir;
        const auto test_file = tmp_dir / "stlTestMesh.stl";

        const auto writer = std::make_shared<sight::io::vtk::stl_mesh_writer>();
        writer->set_object(mesh1);
        writer->set_file(test_file);
        auto write_observer = std::make_shared<sight::core::notification::observer>("Test write");
        CHECK_NOTHROW(writer->write(write_observer));
        CHECK(std::filesystem::exists(test_file));
    }

//------------------------------------------------------------------------------

    TEST_CASE("to_vtk_mesh_with_lines")
    {
        auto mesh      = std::make_shared<sight::data::mesh>();
        auto mesh_lock = mesh->dump_lock();
        mesh->reserve(4, 3, sight::data::mesh::cell_type_t::line);
        mesh->push_point(0, 1, 2);
        mesh->push_point(3, 4, 5);
        mesh->push_point(6, 7, 8);
        mesh->push_point(9, 10, 11);
        mesh->push_cell(0U, 1U);
        mesh->push_cell(1U, 2U);
        mesh->push_cell(2U, 3U);

        auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        sight::io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);

        for(std::uint8_t i = 0 ; i < 4 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 3 ; j++)
            {
                CHECK_MESSAGE(
                    i * 3. + j == vtk_mesh->GetPoints()->GetPoint(i)[j],
                    "component ",
                    std::to_string(j),
                    " of point ",
                    std::to_string(i)
                );
            }
        }

        auto* point_id_list = vtkIdList::New();
        for(std::uint8_t i = 0 ; i < 3 ; i++)
        {
            vtk_mesh->GetCellPoints(i, point_id_list);
            for(std::uint8_t j = 0 ; j < 2 ; j++)
            {
                CHECK_MESSAGE(
                    vtkIdType(i) + j == point_id_list->GetId(j),
                    "point ",
                    std::to_string(j),
                    " of cell ",
                    std::to_string(i)
                );
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("to_vtk_mesh_with_tetras")
    {
        auto mesh      = std::make_shared<sight::data::mesh>();
        auto mesh_lock = mesh->dump_lock();
        mesh->reserve(5, 3, sight::data::mesh::cell_type_t::tetra);
        mesh->push_point(0, 1, 2);
        mesh->push_point(3, 4, 5);
        mesh->push_point(6, 7, 8);
        mesh->push_point(9, 10, 11);
        mesh->push_point(12, 13, 14);
        mesh->push_cell(0U, 1U, 2U, 3U);
        mesh->push_cell(1U, 2U, 3U, 4U);
        mesh->push_cell(2U, 3U, 4U, 0U);

        auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        sight::io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);

        for(std::uint8_t i = 0 ; i < 5 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 3 ; j++)
            {
                CHECK_MESSAGE(
                    i * 3. + j == vtk_mesh->GetPoints()->GetPoint(i)[j],
                    "component ",
                    std::to_string(j),
                    " of point ",
                    std::to_string(i)
                );
            }
        }

        auto* point_id_list = vtkIdList::New();
        for(std::uint8_t i = 0 ; i < 3 ; i++)
        {
            vtk_mesh->GetCellPoints(i, point_id_list);
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                // TODO: fix crash. With cell_type_t = TETRA, the pointIdList is always empty.
                // CHECK_MESSAGE((vtkIdType(i) + j)%4 == pointIdList->GetId(j), "point ", std::to_string(j), " of cell
                // ", std::to_string(i));
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("to_vtk_mesh_with_cell_tex_coords")
    {
        auto mesh      = std::make_shared<sight::data::mesh>();
        auto mesh_lock = mesh->dump_lock();
        mesh->reserve(3, 3, sight::data::mesh::cell_type_t::point, sight::data::mesh::attribute::cell_tex_coords);
        mesh->push_point(0, 1, 2);
        mesh->push_point(3, 4, 5);
        mesh->push_point(6, 7, 8);
        mesh->push_cell(0U);
        mesh->push_cell(1U);
        mesh->push_cell(2U);
        mesh->set_cell_tex_coord(0, 0, 1);
        mesh->set_cell_tex_coord(1, 1, 2);
        mesh->set_cell_tex_coord(2, 2, 3);

        auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        sight::io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);

        for(std::uint8_t i = 0 ; i < 3 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 3 ; j++)
            {
                CHECK_MESSAGE(
                    i * 3. + j == vtk_mesh->GetPoints()->GetPoint(i)[j],
                    "component ",
                    std::to_string(j),
                    " of point ",
                    std::to_string(i)
                );
            }
        }

        auto* point_id_list = vtkIdList::New();
        for(std::uint8_t i = 0 ; i < 3 ; i++)
        {
            vtk_mesh->GetCellPoints(i, point_id_list);
            CHECK_MESSAGE(vtkIdType(i) == point_id_list->GetId(0), "cell ", std::to_string(i));
        }

        for(std::uint8_t i = 0 ; i < 3 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 2 ; j++)
            {
                CHECK_MESSAGE(
                    double(i) + j == vtk_mesh->GetCellData()->GetTCoords()->GetTuple(i)[j],
                    "cell ",
                    std::to_string(i)
                );
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("from_vtk_mesh_with_lines")
    {
        auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        auto points   = vtkSmartPointer<vtkPoints>::New();
        points->InsertNextPoint(0, 1, 2);
        points->InsertNextPoint(3, 4, 5);
        points->InsertNextPoint(6, 7, 8);
        points->InsertNextPoint(9, 10, 11);
        vtk_mesh->SetPoints(points);
        auto cells = vtkSmartPointer<vtkCellArray>::New();
        cells->InsertNextCell({0, 1});
        cells->InsertNextCell({1, 2});
        cells->InsertNextCell({2, 3});
        vtk_mesh->SetLines(cells);

        auto mesh = std::make_shared<sight::data::mesh>();
        sight::io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

        auto mesh_lock = mesh->dump_lock();
        std::uint8_t i = 0;
        for(auto p : mesh->crange<sight::data::iterator::point::xyz>())
        {
            CHECK_MESSAGE(3.F * i == p.x, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 1 == p.y, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 2 == p.z, "point ", std::to_string(i));
            i++;
        }

        auto* point_id_list = vtkIdList::New();
        i = 0;
        for(auto c : mesh->crange<sight::data::iterator::cell::line>())
        {
            vtk_mesh->GetCellPoints(i, point_id_list);
            for(std::uint8_t j = 0 ; j < 2 ; j++)
            {
                CHECK_MESSAGE(
                    sight::data::iterator::cell_t(i) + j == c.pt[j],
                    "point ",
                    std::to_string(j),
                    " of cell ",
                    std::to_string(i)
                );
            }

            i++;
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("from_vtk_mesh_with_quads")
    {
        auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        auto points   = vtkSmartPointer<vtkPoints>::New();
        points->InsertNextPoint(0, 1, 2);
        points->InsertNextPoint(3, 4, 5);
        points->InsertNextPoint(6, 7, 8);
        points->InsertNextPoint(9, 10, 11);
        points->InsertNextPoint(12, 13, 14);
        vtk_mesh->SetPoints(points);
        auto cells = vtkSmartPointer<vtkCellArray>::New();
        cells->InsertNextCell({0, 1, 2, 3});
        cells->InsertNextCell({1, 2, 3, 4});
        cells->InsertNextCell({2, 3, 4, 0});
        vtk_mesh->SetPolys(cells);

        auto mesh = std::make_shared<sight::data::mesh>();
        sight::io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

        auto mesh_lock = mesh->dump_lock();
        std::uint8_t i = 0;
        for(auto p : mesh->crange<sight::data::iterator::point::xyz>())
        {
            CHECK_MESSAGE(3.F * i == p.x, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 1 == p.y, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 2 == p.z, "point ", std::to_string(i));
            i++;
        }

        i = 0;
        for(auto c : mesh->crange<sight::data::iterator::cell::quad>())
        {
            std::uint8_t j = 0;
            for(auto pt : c.pt)
            {
                CHECK_MESSAGE(
                    (i + j) % 5U == pt,
                    "point ",
                    std::to_string(j),
                    " of cell ",
                    std::to_string(i)
                );
                j++;
            }

            i++;
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("from_vtk_mesh_with_cell_tex_coords")
    {
        auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        auto points   = vtkSmartPointer<vtkPoints>::New();
        points->InsertNextPoint(0, 1, 2);
        points->InsertNextPoint(3, 4, 5);
        points->InsertNextPoint(6, 7, 8);
        vtk_mesh->SetPoints(points);
        auto cells                     = vtkSmartPointer<vtkCellArray>::New();
        std::vector<vtkIdType> id_list = {0};
        cells->InsertNextCell(static_cast<vtkIdType>(id_list.size()), id_list.data());
        id_list = {1};
        cells->InsertNextCell(static_cast<vtkIdType>(id_list.size()), id_list.data());
        id_list = {2};
        cells->InsertNextCell(static_cast<vtkIdType>(id_list.size()), id_list.data());
        vtk_mesh->SetVerts(cells);
        auto* tex_coords_array = new float [6] {0, 1, 1, 2, 2, 3};
        auto tex_coords        = vtkSmartPointer<vtkFloatArray>::New();
        tex_coords->SetNumberOfComponents(2);
        tex_coords->SetArray(tex_coords_array, 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
        vtk_mesh->GetCellData()->SetTCoords(tex_coords);

        auto mesh = std::make_shared<sight::data::mesh>();
        sight::io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

        auto mesh_lock = mesh->dump_lock();
        std::uint8_t i = 0;
        for(auto p : mesh->crange<sight::data::iterator::point::xyz>())
        {
            CHECK_MESSAGE(3.F * i == p.x, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 1 == p.y, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 2 == p.z, "point ", std::to_string(i));
            i++;
        }

        i = 0;
        for(const auto& [c, uv] : mesh->czip_range<sight::data::iterator::cell::point,
                                                   sight::data::iterator::cell::uv>())
        {
            CHECK_MESSAGE(unsigned(i) == c.pt, "cell ", std::to_string(i));
            CHECK_MESSAGE(float(i) == uv.u, "cell ", std::to_string(i));
            CHECK_MESSAGE(float(i) + 1 == uv.v, "cell ", std::to_string(i));
            i++;
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("from_vtk_mesh_with3_components_cell_colors")
    {
        auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
        auto points   = vtkSmartPointer<vtkPoints>::New();
        points->InsertNextPoint(0, 1, 2);
        points->InsertNextPoint(3, 4, 5);
        points->InsertNextPoint(6, 7, 8);
        vtk_mesh->SetPoints(points);
        auto cells                     = vtkSmartPointer<vtkCellArray>::New();
        std::vector<vtkIdType> id_list = {0};
        cells->InsertNextCell(static_cast<vtkIdType>(id_list.size()), id_list.data());
        id_list = {1};
        cells->InsertNextCell(static_cast<vtkIdType>(id_list.size()), id_list.data());
        id_list = {2};
        cells->InsertNextCell(static_cast<vtkIdType>(id_list.size()), id_list.data());
        vtk_mesh->SetVerts(cells);
        auto* colors_array = new std::uint8_t[9] {0, 1, 2, 3, 4, 5, 6, 7, 8};
        auto colors        = vtkSmartPointer<vtkUnsignedCharArray>::New();
        colors->SetNumberOfComponents(3);
        colors->SetName("Colors");
        colors->SetArray(colors_array, 3, 0, vtkFloatArray::VTK_DATA_ARRAY_DELETE);
        vtk_mesh->GetCellData()->SetScalars(colors);

        auto mesh = std::make_shared<sight::data::mesh>();
        sight::io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

        auto mesh_lock = mesh->dump_lock();
        std::uint8_t i = 0;
        for(auto p : mesh->crange<sight::data::iterator::point::xyz>())
        {
            CHECK_MESSAGE(3.F * i == p.x, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 1 == p.y, "point ", std::to_string(i));
            CHECK_MESSAGE(3.F * i + 2 == p.z, "point ", std::to_string(i));
            i++;
        }

        i = 0;
        for(const auto& [c, rgba] : mesh->czip_range<sight::data::iterator::cell::point,
                                                     sight::data::iterator::cell::rgba>())
        {
            CHECK_MESSAGE(unsigned(i) == c.pt, "cell ", std::to_string(i));
            CHECK_MESSAGE(std::uint8_t(3 * i) == rgba.r, "cell ", std::to_string(i));
            CHECK_MESSAGE(std::uint8_t(3 * i + 1) == rgba.g, "cell ", std::to_string(i));
            CHECK_MESSAGE(std::uint8_t(3 * i + 2) == rgba.b, "cell ", std::to_string(i));
            CHECK_MESSAGE(std::uint8_t(255) == rgba.a, "cell ", std::to_string(i));
            i++;
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
