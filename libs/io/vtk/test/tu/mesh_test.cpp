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

#include "mesh_test.hpp"

#include <core/os/temp_path.hpp>
#include <core/tools/random/generator.hpp>

#include <data/iterator.hpp>

#include <geometry/data/mesh.hpp>

#include <io/vtk/helper/mesh.hpp>
#include <io/vtk/MeshReader.hpp>
#include <io/vtk/MeshWriter.hpp>
#include <io/vtk/ObjMeshReader.hpp>
#include <io/vtk/ObjMeshWriter.hpp>
#include <io/vtk/PlyMeshReader.hpp>
#include <io/vtk/PlyMeshWriter.hpp>
#include <io/vtk/StlMeshReader.hpp>
#include <io/vtk/StlMeshWriter.hpp>
#include <io/vtk/VtpMeshReader.hpp>
#include <io/vtk/VtpMeshWriter.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/mesh.hpp>

#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTexturedSphereSource.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::mesh_test);

namespace sight::io::vtk::ut
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

void mesh_test::setUp()
{
}

//------------------------------------------------------------------------------

void mesh_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void mesh_test::test_mesh_to_vtk()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(mesh1);

    const vtkSmartPointer<vtkTexturedSphereSource> source = vtkSmartPointer<vtkTexturedSphereSource>::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer<vtkPolyData> poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh1->num_cells());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh1->num_points());

    io::vtk::helper::mesh::from_vtk_mesh(poly_source, mesh1);

    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);

    CPPUNIT_ASSERT(mesh1->num_cells());
    CPPUNIT_ASSERT(mesh1->num_points());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::point_normals>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::cell_normals>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::point_colors>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::cell_colors>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::point_tex_coords>());

    const vtkSmartPointer<vtkPolyData> vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(mesh1, vtk_mesh);
    CPPUNIT_ASSERT(vtk_mesh);

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfVerts(), vtk_mesh->GetNumberOfVerts());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfLines(), vtk_mesh->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPolys(), vtk_mesh->GetNumberOfPolys());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfStrips(), vtk_mesh->GetNumberOfStrips());

    const data::mesh::sptr mesh2 = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh2);

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void mesh_test::test_mesh_to_grid()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(mesh1);

    const vtkSmartPointer<vtkTexturedSphereSource> source = vtkSmartPointer<vtkTexturedSphereSource>::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer<vtkPolyData> poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh1->num_cells());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh1->num_points());

    io::vtk::helper::mesh::from_vtk_mesh(poly_source, mesh1);

    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);

    CPPUNIT_ASSERT(mesh1->num_cells());
    CPPUNIT_ASSERT(mesh1->num_points());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::point_normals>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::cell_normals>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::point_colors>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::cell_colors>());
    CPPUNIT_ASSERT(mesh1->has<data::mesh::attribute::point_tex_coords>());
    const vtkSmartPointer<vtkUnstructuredGrid> vtk_grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    io::vtk::helper::mesh::to_vtk_grid(mesh1, vtk_grid);
    CPPUNIT_ASSERT(vtk_grid);

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPoints(), vtk_grid->GetNumberOfPoints());

    const data::mesh::sptr mesh2 = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(mesh2);
    io::vtk::helper::mesh::from_vtk_grid(vtk_grid, mesh2);

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void mesh_test::test_synthetic_mesh()
{
    {
        const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
        utest_data::generator::mesh::generate_triangle_quad_mesh(mesh1);
        geometry::data::mesh::shake_point(mesh1);
        mesh1->shrink_to_fit();

        const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
        io::vtk::helper::mesh::to_vtk_mesh(mesh1, poly);
        CPPUNIT_ASSERT(poly);

        data::mesh::sptr mesh2 = std::make_shared<data::mesh>();
        io::vtk::helper::mesh::from_vtk_mesh(poly, mesh2);

        CPPUNIT_ASSERT(*mesh1 == *mesh2);
    }
    {
        const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
        utest_data::generator::mesh::generate_triangle_quad_mesh(mesh1);
        geometry::data::mesh::shake_point(mesh1);
        geometry::data::mesh::colorize_mesh_points(mesh1);
        geometry::data::mesh::colorize_mesh_cells(mesh1);
        geometry::data::mesh::generate_point_normals(mesh1);
        geometry::data::mesh::generate_cell_normals(mesh1);
        mesh1->shrink_to_fit();

        const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
        io::vtk::helper::mesh::to_vtk_mesh(mesh1, poly);
        CPPUNIT_ASSERT(poly);

        data::mesh::sptr mesh2 = std::make_shared<data::mesh>();
        io::vtk::helper::mesh::from_vtk_mesh(poly, mesh2);

        CPPUNIT_ASSERT(*mesh1 == *mesh2);
    }
}

//------------------------------------------------------------------------------

void mesh_test::test_export_import_synthetic_mesh()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generate_triangle_quad_mesh(mesh1);
    geometry::data::mesh::shake_point(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);
    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);

    mesh1->shrink_to_fit();

    core::os::temp_dir tmp_dir;
    const auto test_file = tmp_dir / "testExportImportSyntheticMesh.vtk";

    const io::vtk::mesh_writer::sptr writer = std::make_shared<io::vtk::mesh_writer>();
    writer->set_object(mesh1);
    writer->set_file(test_file);
    writer->write();
    CPPUNIT_ASSERT(std::filesystem::exists(test_file));

    const data::mesh::sptr mesh2            = std::make_shared<data::mesh>();
    const io::vtk::mesh_reader::sptr reader = std::make_shared<io::vtk::mesh_reader>();
    reader->set_object(mesh2);
    reader->set_file(test_file);
    reader->read();

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void mesh_test::test_point_cloud()
{
    const auto nb_points = static_cast<data::mesh::size_t>(100 + safe_rand() % 1000);

    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();

    mesh1->reserve(nb_points, nb_points, data::mesh::cell_type_t::point);

    const auto dump_lock = mesh1->dump_lock();

    for(data::mesh::size_t i = 0 ; i < nb_points ; ++i)
    {
        std::array<data::mesh::position_t, 3> point {};
        point[0] = (static_cast<float>(safe_rand() % 1000) - 500.F) / 3.F;
        point[1] = (static_cast<float>(safe_rand() % 1000) - 500.F) / 3.F;
        point[2] = (static_cast<float>(safe_rand() % 1000) - 500.F) / 3.F;
        mesh1->push_point(point);
        mesh1->push_cell(i);
    }

    const vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(mesh1, poly);
    CPPUNIT_ASSERT(poly);

    const data::mesh::sptr mesh2 = std::make_shared<data::mesh>();
    io::vtk::helper::mesh::from_vtk_mesh(poly, mesh2);

    CPPUNIT_ASSERT_EQUAL(nb_points, mesh2->num_points());
    CPPUNIT_ASSERT_EQUAL(nb_points, mesh2->num_cells());
    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void mesh_test::test_read_vtk_file()
{
    const std::filesystem::path test_file(utest_data::dir() / ("sight/mesh/vtk/sphere.vtk"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    io::vtk::mesh_reader::sptr vtk_reader = std::make_shared<io::vtk::mesh_reader>();

    vtk_reader->set_object(mesh);
    vtk_reader->set_file(test_file);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->num_cells() == 720);
    CPPUNIT_ASSERT(mesh->num_points() == 362);
}

//------------------------------------------------------------------------------

void mesh_test::test_read_vtp_file()
{
    const std::filesystem::path test_file(utest_data::dir() / ("sight/mesh/vtp/sphere.vtp"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    io::vtk::vtp_mesh_reader::sptr vtk_reader = std::make_shared<io::vtk::vtp_mesh_reader>();

    vtk_reader->set_object(mesh);
    vtk_reader->set_file(test_file);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->num_cells() == 720);
    CPPUNIT_ASSERT(mesh->num_points() == 362);
}

//------------------------------------------------------------------------------

void mesh_test::test_read_obj_file()
{
    const std::filesystem::path test_file(utest_data::dir() / ("sight/mesh/obj/sphere.obj"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    io::vtk::obj_mesh_reader::sptr vtk_reader = std::make_shared<io::vtk::obj_mesh_reader>();

    vtk_reader->set_object(mesh);
    vtk_reader->set_file(test_file);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->num_cells() == 720);
    CPPUNIT_ASSERT(mesh->num_points() == 362);
}

//------------------------------------------------------------------------------

void mesh_test::test_read_ply_file()
{
    const std::filesystem::path test_file(utest_data::dir() / ("sight/mesh/ply/sphere.ply"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    io::vtk::ply_mesh_reader::sptr vtk_reader = std::make_shared<io::vtk::ply_mesh_reader>();

    vtk_reader->set_object(mesh);
    vtk_reader->set_file(test_file);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->num_cells() == 720);
    CPPUNIT_ASSERT(mesh->num_points() == 362);
}

//------------------------------------------------------------------------------

void mesh_test::test_read_stl_file()
{
    const std::filesystem::path test_file(utest_data::dir() / ("sight/mesh/stl/sphere.stl"));
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + test_file.string() + "' does not exist",
        std::filesystem::exists(test_file)
    );

    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    io::vtk::stl_mesh_reader::sptr vtk_reader = std::make_shared<io::vtk::stl_mesh_reader>();

    vtk_reader->set_object(mesh);
    vtk_reader->set_file(test_file);

    CPPUNIT_ASSERT_NO_THROW(vtk_reader->read());

    CPPUNIT_ASSERT(mesh->num_cells() == 720);
    CPPUNIT_ASSERT(mesh->num_points() == 362);
}

//------------------------------------------------------------------------------

void mesh_test::test_write_vtk_file()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generate_triangle_mesh(mesh1);
    geometry::data::mesh::shake_point(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);
    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);

    mesh1->shrink_to_fit();

    core::os::temp_dir tmp_dir;
    const auto test_file = tmp_dir / "vtkTestMesh.vtk";

    const auto writer = std::make_shared<io::vtk::mesh_writer>();
    writer->set_object(mesh1);
    writer->set_file(test_file);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(test_file));
}

//------------------------------------------------------------------------------

void mesh_test::test_write_vtp_file()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generate_triangle_mesh(mesh1);
    geometry::data::mesh::shake_point(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);
    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);

    mesh1->shrink_to_fit();

    core::os::temp_dir tmp_dir;
    const auto test_file = tmp_dir / "vtpTestMesh.vtp";

    const auto writer = std::make_shared<io::vtk::vtp_mesh_writer>();
    writer->set_object(mesh1);
    writer->set_file(test_file);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(test_file));
}

//------------------------------------------------------------------------------

void mesh_test::test_write_obj_file()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generate_triangle_mesh(mesh1);
    geometry::data::mesh::shake_point(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);
    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);

    mesh1->shrink_to_fit();

    core::os::temp_dir tmp_dir;
    const auto test_file = tmp_dir / "objTestMesh.obj";

    const auto writer = std::make_shared<io::vtk::obj_mesh_writer>();
    writer->set_object(mesh1);
    writer->set_file(test_file);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(test_file));
}

//------------------------------------------------------------------------------

void mesh_test::test_write_ply_file()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generate_triangle_mesh(mesh1);
    geometry::data::mesh::shake_point(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);
    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);

    mesh1->shrink_to_fit();

    core::os::temp_dir tmp_dir;
    const auto test_file = tmp_dir / "plyTestMesh.ply";

    const auto writer = std::make_shared<io::vtk::ply_mesh_writer>();
    writer->set_object(mesh1);
    writer->set_file(test_file);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(test_file));
}

//------------------------------------------------------------------------------

void mesh_test::test_write_stl_file()
{
    const data::mesh::sptr mesh1 = std::make_shared<data::mesh>();
    utest_data::generator::mesh::generate_triangle_mesh(mesh1);
    geometry::data::mesh::shake_point(mesh1);
    geometry::data::mesh::colorize_mesh_points(mesh1);
    geometry::data::mesh::colorize_mesh_cells(mesh1);
    geometry::data::mesh::generate_point_normals(mesh1);
    geometry::data::mesh::generate_cell_normals(mesh1);

    mesh1->shrink_to_fit();

    core::os::temp_dir tmp_dir;
    const auto test_file = tmp_dir / "stlTestMesh.stl";

    const auto writer = std::make_shared<io::vtk::stl_mesh_writer>();
    writer->set_object(mesh1);
    writer->set_file(test_file);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(test_file));
}

//------------------------------------------------------------------------------

void mesh_test::to_vtk_mesh_with_lines_test()
{
    auto mesh      = std::make_shared<data::mesh>();
    auto mesh_lock = mesh->dump_lock();
    mesh->reserve(4, 3, data::mesh::cell_type_t::line);
    mesh->push_point(0, 1, 2);
    mesh->push_point(3, 4, 5);
    mesh->push_point(6, 7, 8);
    mesh->push_point(9, 10, 11);
    mesh->push_cell(0U, 1U);
    mesh->push_cell(1U, 2U);
    mesh->push_cell(2U, 3U);

    auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);

    for(std::uint8_t i = 0 ; i < 4 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 3 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "component " + std::to_string(j) + " of point " + std::to_string(i),
                i * 3. + j,
                vtk_mesh->GetPoints()->GetPoint(i)[j]
            );
        }
    }

    auto* point_id_list = vtkIdList::New();
    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        vtk_mesh->GetCellPoints(i, point_id_list);
        for(std::uint8_t j = 0 ; j < 2 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "point " + std::to_string(j) + " of cell " + std::to_string(i),
                vtkIdType(i) + j,
                point_id_list->GetId(j)
            );
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::to_vtk_mesh_with_tetras_test()
{
    auto mesh      = std::make_shared<data::mesh>();
    auto mesh_lock = mesh->dump_lock();
    mesh->reserve(5, 3, data::mesh::cell_type_t::tetra);
    mesh->push_point(0, 1, 2);
    mesh->push_point(3, 4, 5);
    mesh->push_point(6, 7, 8);
    mesh->push_point(9, 10, 11);
    mesh->push_point(12, 13, 14);
    mesh->push_cell(0U, 1U, 2U, 3U);
    mesh->push_cell(1U, 2U, 3U, 4U);
    mesh->push_cell(2U, 3U, 4U, 0U);

    auto vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);

    for(std::uint8_t i = 0 ; i < 5 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 3 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "component " + std::to_string(j) + " of point " + std::to_string(i),
                i * 3. + j,
                vtk_mesh->GetPoints()->GetPoint(i)[j]
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
            // CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(j) + " of cell " + std::to_string(i),
            // (vtkIdType(i) + j)%4, pointIdList->GetId(j));
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::to_vtk_mesh_with_cell_tex_coords_test()
{
    auto mesh      = std::make_shared<data::mesh>();
    auto mesh_lock = mesh->dump_lock();
    mesh->reserve(3, 3, data::mesh::cell_type_t::point, data::mesh::attribute::cell_tex_coords);
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
    io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);

    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 3 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "component " + std::to_string(j) + " of point " + std::to_string(i),
                i * 3. + j,
                vtk_mesh->GetPoints()->GetPoint(i)[j]
            );
        }
    }

    auto* point_id_list = vtkIdList::New();
    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        vtk_mesh->GetCellPoints(i, point_id_list);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), vtkIdType(i), point_id_list->GetId(0));
    }

    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        for(std::uint8_t j = 0 ; j < 2 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "cell " + std::to_string(i),
                double(i) + j,
                vtk_mesh->GetCellData()->GetTCoords()->GetTuple(i)[j]
            );
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::from_vtk_mesh_with_lines_test()
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

    auto mesh = std::make_shared<data::mesh>();
    io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

    auto mesh_lock = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    auto* point_id_list = vtkIdList::New();
    i = 0;
    for(auto c : mesh->crange<data::iterator::cell::line>())
    {
        vtk_mesh->GetCellPoints(i, point_id_list);
        for(std::uint8_t j = 0 ; j < 2 ; j++)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "point " + std::to_string(j) + " of cell " + std::to_string(i),
                data::iterator::cell_t(i) + j,
                c.pt[j]
            );
        }

        i++;
    }
}

//------------------------------------------------------------------------------

void mesh_test::from_vtk_mesh_with_quads_test()
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

    auto mesh = std::make_shared<data::mesh>();
    io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

    auto mesh_lock = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    i = 0;
    for(auto c : mesh->crange<data::iterator::cell::quad>())
    {
        std::uint8_t j = 0;
        for(auto pt : c.pt)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                "point " + std::to_string(j) + " of cell " + std::to_string(i),
                (i + j) % 5U,
                pt
            );
            j++;
        }

        i++;
    }
}

//------------------------------------------------------------------------------

void mesh_test::from_vtk_mesh_with_cell_tex_coords_test()
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

    auto mesh = std::make_shared<data::mesh>();
    io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

    auto mesh_lock = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    i = 0;
    for(auto [c, uv] : mesh->czip_range<data::iterator::cell::point, data::iterator::cell::uv>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), unsigned(i), c.pt);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), float(i), uv.u);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), float(i) + 1, uv.v);
        i++;
    }
}

//------------------------------------------------------------------------------

void mesh_test::from_vtk_mesh_with3_components_cell_colors_test()
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

    auto mesh = std::make_shared<data::mesh>();
    io::vtk::helper::mesh::from_vtk_mesh(vtk_mesh, mesh);

    auto mesh_lock = mesh->dump_lock();
    std::uint8_t i = 0;
    for(auto p : mesh->crange<data::iterator::point::xyz>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i, p.x);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 1, p.y);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("point " + std::to_string(i), 3.F * i + 2, p.z);
        i++;
    }

    i = 0;
    for(auto [c, rgba] : mesh->czip_range<data::iterator::cell::point, data::iterator::cell::rgba>())
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), unsigned(i), c.pt);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(3 * i), rgba.r);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(3 * i + 1), rgba.g);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(3 * i + 2), rgba.b);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("cell " + std::to_string(i), std::uint8_t(255), rgba.a);
        i++;
    }
}

} // namespace sight::io::vtk::ut
