/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include <data/matrix4.hpp>

#include <geometry/data/matrix4.hpp>
#include <geometry/data/mesh.hpp>

#include <utest_data/generator/mesh.hpp>

#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::mesh_test);

namespace sight::geometry::data::ut
{

namespace point = sight::data::iterator::point;
namespace cell  = sight::data::iterator::cell;

const double epsilon = 10e-6;

//------------------------------------------------------------------------------

void mesh_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void mesh_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void mesh_test::colorizePointsTest()
{
    {
        const std::uint8_t r = 214;
        const std::uint8_t g = 25;
        const std::uint8_t b = 33;
        const std::uint8_t a = 63;

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->get_cell_type(),
            sight::data::mesh::Attributes::POINT_COLORS
        );

        geometry::data::mesh::colorizeMeshPoints(mesh, r, g, b, a);

        const auto dump_lock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<point::rgba>())
        {
            ++count;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(r), static_cast<int>(color.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(g), static_cast<int>(color.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(b), static_cast<int>(color.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(a), static_cast<int>(color.a));
        }
    }

    // Check to colorize few points with RGBA
    {
        const std::uint8_t r = 214;
        const std::uint8_t g = 155;
        const std::uint8_t b = 34;
        const std::uint8_t a = 124;

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->get_cell_type(),
            sight::data::mesh::Attributes::POINT_COLORS
        );

        std::vector<std::size_t> vector_num_triangle = {{0, 12, 1, 3, 21}};

        // fill color with 0
        geometry::data::mesh::colorizeMeshPoints(mesh, 0, 0, 0, 0);

        geometry::data::mesh::colorizeMeshPoints(mesh, vector_num_triangle, r, g, b, a);

        const auto dump_lock = mesh->dump_lock();

        const auto cell_iter_begin = mesh->cbegin<cell::triangle>();

        // get the 3 points of each triangles
        std::set<std::size_t> vertex_indices;
        for(std::size_t i : vector_num_triangle)
        {
            auto cell = cell_iter_begin + std::int64_t(i);
            vertex_indices.insert(cell->pt[0]);
            vertex_indices.insert(cell->pt[1]);
            vertex_indices.insert(cell->pt[2]);
        }

        std::size_t count = 0;
        for(const auto& color : mesh->range<point::rgba>())
        {
            auto iter = vertex_indices.find(count);
            if(iter != vertex_indices.end())
            {
                CPPUNIT_ASSERT_EQUAL(r, color.r);
                CPPUNIT_ASSERT_EQUAL(g, color.g);
                CPPUNIT_ASSERT_EQUAL(b, color.b);
                CPPUNIT_ASSERT_EQUAL(a, color.a);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.r);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.g);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.a);
            }

            ++count;
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::colorizeCellsTest()
{
    // Check to colorize all the mesh with RGB
    {
        const std::uint8_t r = 24;
        const std::uint8_t g = 55;
        const std::uint8_t b = 3;

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->get_cell_type(),
            sight::data::mesh::Attributes::CELL_COLORS
        );

        geometry::data::mesh::colorizeMeshCells(mesh, r, g, b);

        const auto dump_lock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            ++count;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(r), static_cast<int>(color.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(g), static_cast<int>(color.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(b), static_cast<int>(color.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(255), static_cast<int>(color.a));
        }
    }

    // Check to colorize all the mesh with RGBA
    {
        const std::uint8_t r = 214;
        const std::uint8_t g = 25;
        const std::uint8_t b = 33;
        const std::uint8_t a = 63;

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->get_cell_type(),
            sight::data::mesh::Attributes::CELL_COLORS
        );

        geometry::data::mesh::colorizeMeshCells(mesh, r, g, b, a);

        const auto dump_lock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            ++count;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(r), static_cast<int>(color.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(g), static_cast<int>(color.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(b), static_cast<int>(color.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(a), static_cast<int>(color.a));
        }
    }

    // Check to colorize few cells with RGB
    {
        const std::uint8_t r = 24;
        const std::uint8_t g = 55;
        const std::uint8_t b = 3;

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->get_cell_type(),
            sight::data::mesh::Attributes::CELL_COLORS
        );

        std::vector<std::size_t> vector_num_triangle = {{2, 3, 18, 23, 6}};

        // fill color with 0
        geometry::data::mesh::colorizeMeshCells(mesh, 0, 0, 0, 0);

        geometry::data::mesh::colorizeMeshCells(mesh, vector_num_triangle, r, g, b);

        const auto dump_lock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            auto iter = std::find(vector_num_triangle.begin(), vector_num_triangle.end(), count);
            if(iter != vector_num_triangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(r, color.r);
                CPPUNIT_ASSERT_EQUAL(g, color.g);
                CPPUNIT_ASSERT_EQUAL(b, color.b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(255), color.a);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.r);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.g);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.a);
            }

            ++count;
        }
    }

    // Check to colorize few cells with RGBA
    {
        const std::uint8_t r = 2;
        const std::uint8_t g = 125;
        const std::uint8_t b = 75;
        const std::uint8_t a = 55;

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->get_cell_type(),
            sight::data::mesh::Attributes::CELL_COLORS
        );

        std::vector<std::size_t> vector_num_triangle = {{2, 3, 18, 23, 6, 5}};

        // fill color with 0
        geometry::data::mesh::colorizeMeshCells(mesh, 0, 0, 0, 0);

        geometry::data::mesh::colorizeMeshCells(mesh, vector_num_triangle, r, g, b, a);

        const auto dump_lock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            auto iter = std::find(vector_num_triangle.begin(), vector_num_triangle.end(), count);
            if(iter != vector_num_triangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(r, color.r);
                CPPUNIT_ASSERT_EQUAL(g, color.g);
                CPPUNIT_ASSERT_EQUAL(b, color.b);
                CPPUNIT_ASSERT_EQUAL(a, color.a);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.r);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.g);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), color.a);
            }

            ++count;
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::transformTest()
{
    auto vec3_length = [](auto _v) -> float
                       {
                           return static_cast<float>(
                               std::sqrt(std::pow(_v[0], 2) + std::pow(_v[1], 2) + std::pow(_v[2], 2))
                           );
                       };
    const std::array<std::array<float, 3>, 3> expected_points {
        std::array {3.F, -5.F, 5.F},
        std::array {9.F, -11.F, 11.F},
        std::array {15.F, -17.F, 17.F}
    };
    const std::array<std::array<float, 3>, 3> expected_normals {
        std::array {2.F, -6.F, 4.F},
        std::array {8.F, -12.F, 10.F},
        std::array {14.F, -18.F, 16.F}
    };
    const std::array<float, 3> expected_normal_lengths {
        vec3_length(expected_normals[0]),
        vec3_length(expected_normals[1]),
        vec3_length(expected_normals[2])
    };
    const glm::mat4 translation    = glm::translate(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1));
    const glm::mat4 rotation       = glm::rotate(glm::identity<glm::mat4>(), glm::pi<float>() / 2, glm::vec3(1, 0, 0));
    const glm::mat4 scale          = glm::scale(glm::identity<glm::mat4>(), glm::vec3(2, 2, 2));
    const glm::mat4 transformation = translation * rotation * scale;
    sight::data::matrix4 trans;
    geometry::data::from_glm_mat(trans, transformation);

    // Points only
    {
        sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
        const auto in_lock         = in->dump_lock();
        in->pushPoint(1, 2, 3);
        in->pushPoint(4, 5, 6);
        in->pushPoint(7, 8, 9);
        sight::data::mesh::sptr out = sight::data::mesh::copy(in);
        const auto out_lock         = out->dump_lock();
        geometry::data::mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& point : out->crange<point::xyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][0], point.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][1], point.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][2], point.z, epsilon);
            i++;
        }
    }

    // Points + Point normals
    {
        sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
        in->reserve(3, 1, sight::data::mesh::cell_type_t::POINT, sight::data::mesh::Attributes::POINT_NORMALS);
        const auto in_lock = in->dump_lock();
        auto id            = in->pushPoint(1, 2, 3);
        in->setPointNormal(id, 1, 2, 3);
        id = in->pushPoint(4, 5, 6);
        in->setPointNormal(id, 4, 5, 6);
        id = in->pushPoint(7, 8, 9);
        in->setPointNormal(id, 7, 8, 9);
        sight::data::mesh::sptr out = sight::data::mesh::copy(in);
        const auto out_lock         = out->dump_lock();
        geometry::data::mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& [p, n] : out->czip_range<point::xyz, point::nxyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][0], p.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][1], p.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][2], p.z, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][0] / expected_normal_lengths[i], n.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][1] / expected_normal_lengths[i], n.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][2] / expected_normal_lengths[i], n.nz, epsilon);

            i++;
        }
    }

    // Points + Cell normals
    {
        sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
        in->reserve(3, 3, sight::data::mesh::cell_type_t::POINT, sight::data::mesh::Attributes::CELL_NORMALS);
        const auto in_lock = in->dump_lock();
        auto id_p          = in->pushPoint(1, 2, 3);
        auto id_c          = in->pushCell(id_p);
        in->setCellNormal(id_c, 1, 2, 3);
        id_p = in->pushPoint(4, 5, 6);
        id_c = in->pushCell(id_p);
        in->setCellNormal(id_c, 4, 5, 6);
        id_p = in->pushPoint(7, 8, 9);
        id_c = in->pushCell(id_p);
        in->setCellNormal(id_c, 7, 8, 9);
        sight::data::mesh::sptr out = sight::data::mesh::copy(in);
        const auto out_lock         = out->dump_lock();
        geometry::data::mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& [p, n] : out->czip_range<point::xyz, cell::nxyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][0], p.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][1], p.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][2], p.z, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][0] / expected_normal_lengths[i], n.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][1] / expected_normal_lengths[i], n.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][2] / expected_normal_lengths[i], n.nz, epsilon);

            i++;
        }
    }

    // Points + All normals
    {
        sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
        in->reserve(
            3,
            3,
            sight::data::mesh::cell_type_t::POINT,
            sight::data::mesh::Attributes::POINT_NORMALS | sight::data::mesh::Attributes::CELL_NORMALS
        );
        const auto in_lock = in->dump_lock();
        auto id_p          = in->pushPoint(1, 2, 3);
        in->setPointNormal(id_p, 1, 2, 3);
        auto id_c = in->pushCell(id_p);
        in->setCellNormal(id_c, 1, 2, 3);
        id_p = in->pushPoint(4, 5, 6);
        in->setPointNormal(id_p, 4, 5, 6);
        id_c = in->pushCell(id_p);
        in->setCellNormal(id_c, 4, 5, 6);
        id_p = in->pushPoint(7, 8, 9);
        in->setPointNormal(id_p, 7, 8, 9);
        id_c = in->pushCell(id_p);
        in->setCellNormal(id_c, 7, 8, 9);
        sight::data::mesh::sptr out = sight::data::mesh::copy(in);
        const auto out_lock         = out->dump_lock();
        geometry::data::mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& [p, pn, cn] : out->czip_range<point::xyz, point::nxyz, cell::nxyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][0], p.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][1], p.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_points[i][2], p.z, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][0] / expected_normal_lengths[i], pn.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][1] / expected_normal_lengths[i], pn.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][2] / expected_normal_lengths[i], pn.nz, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][0] / expected_normal_lengths[i], cn.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][1] / expected_normal_lengths[i], cn.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_normals[i][2] / expected_normal_lengths[i], cn.nz, epsilon);

            i++;
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::isClosedTest()
{
    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleMesh(mesh);

        const bool is_closed = geometry::data::mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, is_closed);
    }

    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateQuadMesh(mesh);

        const bool is_closed = geometry::data::mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, is_closed);
    }

    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        utest_data::generator::mesh::generateTriangleQuadMesh(mesh);

        const bool is_closed = geometry::data::mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, is_closed);
    }

    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto dump_lock         = mesh->dump_lock();

        mesh->pushPoint(0.F, 0.F, 0.F);
        mesh->pushPoint(1.F, 0.F, 0.F);
        mesh->pushPoint(1.F, 1.F, 0.F);
        mesh->pushPoint(0.F, 1.F, 0.F);
        mesh->pushPoint(0.F, 0.F, 1.F);
        mesh->pushPoint(1.F, 0.F, 1.F);
        mesh->pushPoint(1.F, 1.F, 1.F);
        mesh->pushPoint(0.F, 1.F, 1.F);

        mesh->pushCell(0, 1, 2, 3);
        mesh->pushCell(0, 4, 5, 1);
        mesh->pushCell(1, 5, 6, 2);
        mesh->pushCell(4, 5, 6, 7);
        mesh->pushCell(0, 4, 7, 3);
        mesh->pushCell(3, 7, 6, 2);

        const bool is_closed = geometry::data::mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(true, is_closed);
    }

    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto dump_lock         = mesh->dump_lock();

        mesh->pushPoint(0.F, 0.F, 0.F);
        mesh->pushPoint(1.F, 0.F, 0.F);
        mesh->pushPoint(1.F, 1.F, 0.F);
        mesh->pushPoint(0.F, 1.F, 0.F);
        mesh->pushPoint(0.F, 0.F, 1.F);
        mesh->pushPoint(1.F, 0.F, 1.F);
        mesh->pushPoint(1.F, 1.F, 1.F);
        mesh->pushPoint(0.F, 1.F, 1.F);

        mesh->pushCell(0, 1, 3);
        mesh->pushCell(3, 1, 2);
        mesh->pushCell(0, 4, 1);
        mesh->pushCell(1, 4, 5);
        mesh->pushCell(1, 5, 2);
        mesh->pushCell(5, 6, 2);
        mesh->pushCell(4, 5, 7);
        mesh->pushCell(5, 6, 7);
        mesh->pushCell(0, 4, 3);
        mesh->pushCell(4, 7, 3);
        mesh->pushCell(3, 7, 2);
        mesh->pushCell(7, 6, 2);

        const bool is_closed = geometry::data::mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(true, is_closed);
    }

    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto dump_lock         = mesh->dump_lock();

        mesh->pushPoint(0.F, 0.F, 0.F);
        mesh->pushPoint(1.F, 0.F, 0.F);
        mesh->pushPoint(1.F, 1.F, 0.F);
        mesh->pushPoint(0.F, 1.F, 0.F);
        mesh->pushPoint(0.F, 0.F, 1.F);
        mesh->pushPoint(1.F, 0.F, 1.F);
        mesh->pushPoint(1.F, 1.F, 1.F);
        mesh->pushPoint(0.F, 1.F, 1.F);

        mesh->pushCell(0, 1, 3);
        mesh->pushCell(3, 1, 2);
        mesh->pushCell(0, 4, 1);
        mesh->pushCell(1, 4, 5);
        mesh->pushCell(1, 5, 2);
        mesh->pushCell(5, 6, 2);
        mesh->pushCell(4, 5, 7);
        mesh->pushCell(5, 6, 7);
        mesh->pushCell(0, 4, 3);
        // removed this cell mesh->pushCell(4, 7, 3);
        mesh->pushCell(3, 7, 2);
        mesh->pushCell(7, 6, 2);

        const bool is_closed = geometry::data::mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, is_closed);
    }
}

//------------------------------------------------------------------------------

void mesh_test::cellNormalTest()
{
    sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
    const auto dump_lock         = mesh->dump_lock();

    mesh->pushPoint(0.F, 0.F, 0.F);
    mesh->pushPoint(1.F, 0.F, 0.F);
    mesh->pushPoint(1.F, 1.F, 0.F);
    mesh->pushPoint(0.F, 1.F, 0.F);
    mesh->pushPoint(0.F, 0.F, 1.F);
    mesh->pushPoint(1.F, 0.F, 1.F);
    mesh->pushPoint(1.F, 1.F, 1.F);
    mesh->pushPoint(0.F, 1.F, 1.F);

    mesh->pushCell(0, 3, 2, 1);
    mesh->pushCell(0, 4, 5, 1);
    mesh->pushCell(1, 2, 6, 5);
    mesh->pushCell(4, 5, 6, 7);
    mesh->pushCell(0, 4, 7, 3);
    mesh->pushCell(3, 2, 6, 7);

    CPPUNIT_ASSERT_NO_THROW(geometry::data::mesh::generateCellNormals(mesh));

    auto cell_iter = mesh->begin<cell::nxyz>();

    // check first cell normal = {0, 0, -1}
    std::array<float, 3> n = {0.F, 0.F, -1.F};
    CPPUNIT_ASSERT_EQUAL(n[0], cell_iter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cell_iter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cell_iter->nz);

    // check cell 2 normal = {0, 1, 0}
    ++cell_iter;
    n = {0.F, 1.F, 0.F};
    CPPUNIT_ASSERT_EQUAL(n[0], cell_iter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cell_iter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cell_iter->nz);

    // check cell 3 normal = {1, 0, 0}
    ++cell_iter;
    n = {1.F, 0.F, 0.F};
    CPPUNIT_ASSERT_EQUAL(n[0], cell_iter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cell_iter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cell_iter->nz);

    // check cell 4 normal = {0, 0, 1}
    ++cell_iter;
    n = {0.F, 0.F, 1.F};
    CPPUNIT_ASSERT_EQUAL(n[0], cell_iter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cell_iter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cell_iter->nz);

    // check cell 5 normal = {-1, 0, 0}
    ++cell_iter;
    n = {-1.F, 0.F, 0.F};
    CPPUNIT_ASSERT_EQUAL(n[0], cell_iter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cell_iter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cell_iter->nz);

    // check cell 6 normal = {0, -1, 0}
    ++cell_iter;
    n = {0.F, -1.F, 0.F};
    CPPUNIT_ASSERT_EQUAL(n[0], cell_iter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cell_iter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cell_iter->nz);
}

//------------------------------------------------------------------------------

void mesh_test::pointNormalTest()
{
    sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
    const auto dump_lock         = mesh->dump_lock();

    mesh->pushPoint(0.F, 0.F, 0.F);
    mesh->pushPoint(1.F, 0.F, 0.F);
    mesh->pushPoint(1.F, 1.F, 0.F);
    mesh->pushPoint(0.F, 1.F, 0.F);
    mesh->pushPoint(0.F, 0.F, 1.F);
    mesh->pushPoint(1.F, 0.F, 1.F);
    mesh->pushPoint(1.F, 1.F, 1.F);
    mesh->pushPoint(0.F, 1.F, 1.F);

    mesh->pushCell(0, 3, 2, 1);
    mesh->pushCell(0, 4, 5, 1);
    mesh->pushCell(1, 2, 6, 5);
    mesh->pushCell(4, 5, 6, 7);
    mesh->pushCell(0, 4, 7, 3);
    mesh->pushCell(3, 2, 6, 7);

    CPPUNIT_ASSERT_NO_THROW(geometry::data::mesh::generatePointNormals(mesh));

    auto point_iter = mesh->begin<point::nxyz>();

    // check first point normal = {-0.57735, 0.57735, -0.57735}
    std::array<float, 3> n = {-0.57735F, 0.57735F, -0.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);

    // check point 2 normal = {0.57735, 0.57735, -0.57735}
    ++point_iter;
    n = {0.57735F, 0.57735F, -0.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);

    // check point 3 normal = {0.57735, -0.57735, -0.57735}
    ++point_iter;
    n = {0.57735F, -0.57735F, -.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);

    // check point 4 normal = {-0.57735, -0.57735, -0.57735}
    ++point_iter;
    n = {-0.57735F, -0.57735F, -0.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);

    // check point 5 normal = {-0.57735, 0.57735, 0.57735}
    ++point_iter;
    n = {-0.57735F, 0.57735F, 0.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);

    // check point 6 normal = {0.57735, 0.57735, 0.57735}
    ++point_iter;
    n = {0.57735F, 0.57735F, 0.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);

    // check point 7 normal = {0.57735, -0.57735, 0.57735}
    ++point_iter;
    n = {0.57735F, -0.57735F, 0.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);

    // check point 8 normal = {-0.57735, -0.57735, 0.57735}
    ++point_iter;
    n = {-0.57735F, -0.57735F, 0.57735F};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], point_iter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], point_iter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], point_iter->nz, 0.00001);
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data::ut
