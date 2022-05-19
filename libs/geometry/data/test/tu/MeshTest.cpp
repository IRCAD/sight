/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "MeshTest.hpp"

#include <data/Matrix4.hpp>

#include <geometry/data/Matrix4.hpp>
#include <geometry/data/Mesh.hpp>

#include <utestData/generator/Mesh.hpp>

#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::MeshTest);

namespace sight::geometry::data
{

namespace ut
{

using namespace sight::data::iterator;

const double epsilon = 10e-6;

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::colorizePointsTest()
{
    {
        const std::uint8_t R = 214;
        const std::uint8_t G = 25;
        const std::uint8_t B = 33;
        const std::uint8_t A = 63;

        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::POINT_COLORS
        );

        geometry::data::Mesh::colorizeMeshPoints(mesh, R, G, B, A);

        const auto dumpLock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<point::rgba>())
        {
            ++count;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(R), static_cast<int>(color.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(G), static_cast<int>(color.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(B), static_cast<int>(color.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(A), static_cast<int>(color.a));
        }
    }

    // Check to colorize few points with RGBA
    {
        const std::uint8_t R = 214;
        const std::uint8_t G = 155;
        const std::uint8_t B = 34;
        const std::uint8_t A = 124;

        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::POINT_COLORS
        );

        std::vector<std::size_t> vectorNumTriangle = {{0, 12, 1, 3, 21}};

        // fill color with 0
        geometry::data::Mesh::colorizeMeshPoints(mesh, 0, 0, 0, 0);

        geometry::data::Mesh::colorizeMeshPoints(mesh, vectorNumTriangle, R, G, B, A);

        const auto dumpLock = mesh->dump_lock();

        const auto cellIterBegin = mesh->cbegin<cell::triangle>();

        // get the 3 points of each triangles
        std::set<std::size_t> vertexIndices;
        for(std::size_t i = 0 ; i < vectorNumTriangle.size() ; ++i)
        {
            auto cell = cellIterBegin + long(vectorNumTriangle[i]);
            vertexIndices.insert(cell->pt[0]);
            vertexIndices.insert(cell->pt[1]);
            vertexIndices.insert(cell->pt[2]);
        }

        std::size_t count = 0;
        for(const auto& color : mesh->range<point::rgba>())
        {
            auto iter = std::find(vertexIndices.begin(), vertexIndices.end(), count);
            if(iter != vertexIndices.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, color.r);
                CPPUNIT_ASSERT_EQUAL(G, color.g);
                CPPUNIT_ASSERT_EQUAL(B, color.b);
                CPPUNIT_ASSERT_EQUAL(A, color.a);
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

void MeshTest::colorizeCellsTest()
{
    // Check to colorize all the mesh with RGB
    {
        const std::uint8_t R = 24;
        const std::uint8_t G = 55;
        const std::uint8_t B = 3;

        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        geometry::data::Mesh::colorizeMeshCells(mesh, R, G, B);

        const auto dumpLock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            ++count;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(R), static_cast<int>(color.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(G), static_cast<int>(color.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(B), static_cast<int>(color.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(255), static_cast<int>(color.a));
        }
    }

    // Check to colorize all the mesh with RGBA
    {
        const std::uint8_t R = 214;
        const std::uint8_t G = 25;
        const std::uint8_t B = 33;
        const std::uint8_t A = 63;

        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        geometry::data::Mesh::colorizeMeshCells(mesh, R, G, B, A);

        const auto dumpLock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            ++count;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(R), static_cast<int>(color.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(G), static_cast<int>(color.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(B), static_cast<int>(color.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(A), static_cast<int>(color.a));
        }
    }

    // Check to colorize few cells with RGB
    {
        const std::uint8_t R = 24;
        const std::uint8_t G = 55;
        const std::uint8_t B = 3;

        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        std::vector<std::size_t> vectorNumTriangle = {{2, 3, 18, 23, 6}};

        // fill color with 0
        geometry::data::Mesh::colorizeMeshCells(mesh, 0, 0, 0, 0);

        geometry::data::Mesh::colorizeMeshCells(mesh, vectorNumTriangle, R, G, B);

        const auto dumpLock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            auto iter = std::find(vectorNumTriangle.begin(), vectorNumTriangle.end(), count);
            if(iter != vectorNumTriangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, color.r);
                CPPUNIT_ASSERT_EQUAL(G, color.g);
                CPPUNIT_ASSERT_EQUAL(B, color.b);
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
        const std::uint8_t R = 2;
        const std::uint8_t G = 125;
        const std::uint8_t B = 75;
        const std::uint8_t A = 55;

        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleMesh(mesh);

        mesh->resize(
            mesh->numPoints(),
            mesh->numCells(),
            mesh->getCellType(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        std::vector<std::size_t> vectorNumTriangle = {{2, 3, 18, 23, 6, 5}};

        // fill color with 0
        geometry::data::Mesh::colorizeMeshCells(mesh, 0, 0, 0, 0);

        geometry::data::Mesh::colorizeMeshCells(mesh, vectorNumTriangle, R, G, B, A);

        const auto dumpLock = mesh->dump_lock();

        std::size_t count = 0;
        for(const auto& color : mesh->crange<cell::rgba>())
        {
            auto iter = std::find(vectorNumTriangle.begin(), vectorNumTriangle.end(), count);
            if(iter != vectorNumTriangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, color.r);
                CPPUNIT_ASSERT_EQUAL(G, color.g);
                CPPUNIT_ASSERT_EQUAL(B, color.b);
                CPPUNIT_ASSERT_EQUAL(A, color.a);
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

void MeshTest::transformTest()
{
    auto vec3Length = [](auto v) -> float
                      {
                          return static_cast<float>(
                              std::sqrt(std::pow(v[0], 2) + std::pow(v[1], 2) + std::pow(v[2], 2))
                          );
                      };
    const std::array<std::array<float, 3>, 3> expectedPoints {
        std::array {3.f, -5.f, 5.f},
        std::array {9.f, -11.f, 11.f},
        std::array {15.f, -17.f, 17.f}
    };
    const std::array<std::array<float, 3>, 3> expectedNormals {
        std::array {2.f, -6.f, 4.f},
        std::array {8.f, -12.f, 10.f},
        std::array {14.f, -18.f, 16.f}
    };
    const std::array<float, 3> expectedNormalLengths {
        vec3Length(expectedNormals[0]),
        vec3Length(expectedNormals[1]),
        vec3Length(expectedNormals[2])
    };
    const glm::mat4 translation    = glm::translate(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1));
    const glm::mat4 rotation       = glm::rotate(glm::identity<glm::mat4>(), glm::pi<float>() / 2, glm::vec3(1, 0, 0));
    const glm::mat4 scale          = glm::scale(glm::identity<glm::mat4>(), glm::vec3(2, 2, 2));
    const glm::mat4 transformation = translation * rotation * scale;
    sight::data::Matrix4 trans;
    geometry::data::setTF3DFromMatrix(trans, transformation);

    // Points only
    {
        sight::data::Mesh::sptr in = sight::data::Mesh::New();
        const auto inLock          = in->dump_lock();
        in->pushPoint(1, 2, 3);
        in->pushPoint(4, 5, 6);
        in->pushPoint(7, 8, 9);
        sight::data::Mesh::sptr out = sight::data::Mesh::copy(in);
        const auto outLock          = out->dump_lock();
        geometry::data::Mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& point : out->crange<point::xyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][0], point.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][1], point.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][2], point.z, epsilon);
            i++;
        }
    }

    // Points + Point normals
    {
        sight::data::Mesh::sptr in = sight::data::Mesh::New();
        in->reserve(3, 1, sight::data::Mesh::CellType::POINT, sight::data::Mesh::Attributes::POINT_NORMALS);
        const auto inLock = in->dump_lock();
        auto id           = in->pushPoint(1, 2, 3);
        in->setPointNormal(id, 1, 2, 3);
        id = in->pushPoint(4, 5, 6);
        in->setPointNormal(id, 4, 5, 6);
        id = in->pushPoint(7, 8, 9);
        in->setPointNormal(id, 7, 8, 9);
        sight::data::Mesh::sptr out = sight::data::Mesh::copy(in);
        const auto outLock          = out->dump_lock();
        geometry::data::Mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& [p, n] : out->czip_range<point::xyz, point::nxyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][0], p.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][1], p.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][2], p.z, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][0] / expectedNormalLengths[i], n.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][1] / expectedNormalLengths[i], n.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][2] / expectedNormalLengths[i], n.nz, epsilon);

            i++;
        }
    }

    // Points + Cell normals
    {
        sight::data::Mesh::sptr in = sight::data::Mesh::New();
        in->reserve(3, 3, sight::data::Mesh::CellType::POINT, sight::data::Mesh::Attributes::CELL_NORMALS);
        const auto inLock = in->dump_lock();
        auto idP          = in->pushPoint(1, 2, 3);
        auto idC          = in->pushCell(idP);
        in->setCellNormal(idC, 1, 2, 3);
        idP = in->pushPoint(4, 5, 6);
        idC = in->pushCell(idP);
        in->setCellNormal(idC, 4, 5, 6);
        idP = in->pushPoint(7, 8, 9);
        idC = in->pushCell(idP);
        in->setCellNormal(idC, 7, 8, 9);
        sight::data::Mesh::sptr out = sight::data::Mesh::copy(in);
        const auto outLock          = out->dump_lock();
        geometry::data::Mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& [p, n] : out->czip_range<point::xyz, cell::nxyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][0], p.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][1], p.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][2], p.z, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][0] / expectedNormalLengths[i], n.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][1] / expectedNormalLengths[i], n.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][2] / expectedNormalLengths[i], n.nz, epsilon);

            i++;
        }
    }

    // Points + All normals
    {
        sight::data::Mesh::sptr in = sight::data::Mesh::New();
        in->reserve(
            3,
            3,
            sight::data::Mesh::CellType::POINT,
            sight::data::Mesh::Attributes::POINT_NORMALS | sight::data::Mesh::Attributes::CELL_NORMALS
        );
        const auto inLock = in->dump_lock();
        auto idP          = in->pushPoint(1, 2, 3);
        in->setPointNormal(idP, 1, 2, 3);
        auto idC = in->pushCell(idP);
        in->setCellNormal(idC, 1, 2, 3);
        idP = in->pushPoint(4, 5, 6);
        in->setPointNormal(idP, 4, 5, 6);
        idC = in->pushCell(idP);
        in->setCellNormal(idC, 4, 5, 6);
        idP = in->pushPoint(7, 8, 9);
        in->setPointNormal(idP, 7, 8, 9);
        idC = in->pushCell(idP);
        in->setCellNormal(idC, 7, 8, 9);
        sight::data::Mesh::sptr out = sight::data::Mesh::copy(in);
        const auto outLock          = out->dump_lock();
        geometry::data::Mesh::transform(in, out, trans);
        std::size_t i = 0;
        for(const auto& [p, pn, cn] : out->czip_range<point::xyz, point::nxyz, cell::nxyz>())
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][0], p.x, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][1], p.y, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedPoints[i][2], p.z, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][0] / expectedNormalLengths[i], pn.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][1] / expectedNormalLengths[i], pn.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][2] / expectedNormalLengths[i], pn.nz, epsilon);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][0] / expectedNormalLengths[i], cn.nx, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][1] / expectedNormalLengths[i], cn.ny, epsilon);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectedNormals[i][2] / expectedNormalLengths[i], cn.nz, epsilon);

            i++;
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::isClosedTest()
{
    {
        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleMesh(mesh);

        const bool isClosed = geometry::data::Mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, isClosed);
    }

    {
        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateQuadMesh(mesh);

        const bool isClosed = geometry::data::Mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, isClosed);
    }

    {
        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        utestData::generator::Mesh::generateTriangleQuadMesh(mesh);

        const bool isClosed = geometry::data::Mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, isClosed);
    }

    {
        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        const auto dumpLock          = mesh->dump_lock();

        mesh->pushPoint(0.f, 0.f, 0.f);
        mesh->pushPoint(1.f, 0.f, 0.f);
        mesh->pushPoint(1.f, 1.f, 0.f);
        mesh->pushPoint(0.f, 1.f, 0.f);
        mesh->pushPoint(0.f, 0.f, 1.f);
        mesh->pushPoint(1.f, 0.f, 1.f);
        mesh->pushPoint(1.f, 1.f, 1.f);
        mesh->pushPoint(0.f, 1.f, 1.f);

        mesh->pushCell(0, 1, 2, 3);
        mesh->pushCell(0, 4, 5, 1);
        mesh->pushCell(1, 5, 6, 2);
        mesh->pushCell(4, 5, 6, 7);
        mesh->pushCell(0, 4, 7, 3);
        mesh->pushCell(3, 7, 6, 2);

        const bool isClosed = geometry::data::Mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(true, isClosed);
    }

    {
        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        const auto dumpLock          = mesh->dump_lock();

        mesh->pushPoint(0.f, 0.f, 0.f);
        mesh->pushPoint(1.f, 0.f, 0.f);
        mesh->pushPoint(1.f, 1.f, 0.f);
        mesh->pushPoint(0.f, 1.f, 0.f);
        mesh->pushPoint(0.f, 0.f, 1.f);
        mesh->pushPoint(1.f, 0.f, 1.f);
        mesh->pushPoint(1.f, 1.f, 1.f);
        mesh->pushPoint(0.f, 1.f, 1.f);

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

        const bool isClosed = geometry::data::Mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(true, isClosed);
    }

    {
        sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
        const auto dumpLock          = mesh->dump_lock();

        mesh->pushPoint(0.f, 0.f, 0.f);
        mesh->pushPoint(1.f, 0.f, 0.f);
        mesh->pushPoint(1.f, 1.f, 0.f);
        mesh->pushPoint(0.f, 1.f, 0.f);
        mesh->pushPoint(0.f, 0.f, 1.f);
        mesh->pushPoint(1.f, 0.f, 1.f);
        mesh->pushPoint(1.f, 1.f, 1.f);
        mesh->pushPoint(0.f, 1.f, 1.f);

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

        const bool isClosed = geometry::data::Mesh::isClosed(mesh);

        CPPUNIT_ASSERT_EQUAL(false, isClosed);
    }
}

//------------------------------------------------------------------------------

void MeshTest::cellNormalTest()
{
    sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
    const auto dumpLock          = mesh->dump_lock();

    mesh->pushPoint(0.f, 0.f, 0.f);
    mesh->pushPoint(1.f, 0.f, 0.f);
    mesh->pushPoint(1.f, 1.f, 0.f);
    mesh->pushPoint(0.f, 1.f, 0.f);
    mesh->pushPoint(0.f, 0.f, 1.f);
    mesh->pushPoint(1.f, 0.f, 1.f);
    mesh->pushPoint(1.f, 1.f, 1.f);
    mesh->pushPoint(0.f, 1.f, 1.f);

    mesh->pushCell(0, 3, 2, 1);
    mesh->pushCell(0, 4, 5, 1);
    mesh->pushCell(1, 2, 6, 5);
    mesh->pushCell(4, 5, 6, 7);
    mesh->pushCell(0, 4, 7, 3);
    mesh->pushCell(3, 2, 6, 7);

    CPPUNIT_ASSERT_NO_THROW(geometry::data::Mesh::generateCellNormals(mesh));

    auto cellIter = mesh->begin<cell::nxyz>();

    // check first cell normal = {0, 0, -1}
    std::array<float, 3> n = {0.f, 0.f, -1.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->nz);

    // check cell 2 normal = {0, 1, 0}
    ++cellIter;
    n = {0.f, 1.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->nz);

    // check cell 3 normal = {1, 0, 0}
    ++cellIter;
    n = {1.f, 0.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->nz);

    // check cell 4 normal = {0, 0, 1}
    ++cellIter;
    n = {0.f, 0.f, 1.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->nz);

    // check cell 5 normal = {-1, 0, 0}
    ++cellIter;
    n = {-1.f, 0.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->nz);

    // check cell 6 normal = {0, -1, 0}
    ++cellIter;
    n = {0.f, -1.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->nz);
}

//------------------------------------------------------------------------------

void MeshTest::pointNormalTest()
{
    sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
    const auto dumpLock          = mesh->dump_lock();

    mesh->pushPoint(0.f, 0.f, 0.f);
    mesh->pushPoint(1.f, 0.f, 0.f);
    mesh->pushPoint(1.f, 1.f, 0.f);
    mesh->pushPoint(0.f, 1.f, 0.f);
    mesh->pushPoint(0.f, 0.f, 1.f);
    mesh->pushPoint(1.f, 0.f, 1.f);
    mesh->pushPoint(1.f, 1.f, 1.f);
    mesh->pushPoint(0.f, 1.f, 1.f);

    mesh->pushCell(0, 3, 2, 1);
    mesh->pushCell(0, 4, 5, 1);
    mesh->pushCell(1, 2, 6, 5);
    mesh->pushCell(4, 5, 6, 7);
    mesh->pushCell(0, 4, 7, 3);
    mesh->pushCell(3, 2, 6, 7);

    CPPUNIT_ASSERT_NO_THROW(geometry::data::Mesh::generatePointNormals(mesh));

    auto pointIter = mesh->begin<point::nxyz>();

    // check first point normal = {-0.57735, 0.57735, -0.57735}
    std::array<float, 3> n = {-0.57735f, 0.57735f, -0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);

    // check point 2 normal = {0.57735, 0.57735, -0.57735}
    ++pointIter;
    n = {0.57735f, 0.57735f, -0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);

    // check point 3 normal = {0.57735, -0.57735, -0.57735}
    ++pointIter;
    n = {0.57735f, -0.57735f, -.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);

    // check point 4 normal = {-0.57735, -0.57735, -0.57735}
    ++pointIter;
    n = {-0.57735f, -0.57735f, -0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);

    // check point 5 normal = {-0.57735, 0.57735, 0.57735}
    ++pointIter;
    n = {-0.57735f, 0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);

    // check point 6 normal = {0.57735, 0.57735, 0.57735}
    ++pointIter;
    n = {0.57735f, 0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);

    // check point 7 normal = {0.57735, -0.57735, 0.57735}
    ++pointIter;
    n = {0.57735f, -0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);

    // check point 8 normal = {-0.57735, -0.57735, 0.57735}
    ++pointIter;
    n = {-0.57735f, -0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->nz, 0.00001);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::geometry::data
