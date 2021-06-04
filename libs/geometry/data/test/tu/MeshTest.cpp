/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include <core/tools/Type.hpp>

#include <data/Matrix4.hpp>

#include <geometry/data/Matrix4.hpp>
#include <geometry/data/Mesh.hpp>

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <utestData/generator/Mesh.hpp>

#include <sstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::geometry::data::ut::MeshTest);

namespace sight::geometry::data
{

namespace ut
{

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
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::POINT_COLORS
        );

        geometry::data::Mesh::colorizeMeshPoints(mesh, R, G, B, A);

        const auto dumpLock = mesh->lock();

        auto itr          = mesh->begin<sight::data::iterator::ConstPointIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstPointIterator>();

        size_t count = 0;
        for( ; itr != itrEnd ; ++itr, ++count)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(R), static_cast<int>(itr->rgba->r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(G), static_cast<int>(itr->rgba->g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(B), static_cast<int>(itr->rgba->b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(A), static_cast<int>(itr->rgba->a));
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
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::POINT_COLORS
        );

        std::vector<size_t> vectorNumTriangle = {{0, 12, 1, 3, 21}};

        // fill color with 0
        geometry::data::Mesh::colorizeMeshPoints(mesh, 0, 0, 0, 0);

        geometry::data::Mesh::colorizeMeshPoints(mesh, vectorNumTriangle, R, G, B, A);

        const auto dumpLock = mesh->lock();

        const auto cellIterBegin = mesh->begin<sight::data::iterator::ConstCellIterator>();

        // get the 3 points of each triangles
        std::set<size_t> vertexIndices;
        for(size_t i = 0 ; i < vectorNumTriangle.size() ; ++i)
        {
            auto cell = cellIterBegin + vectorNumTriangle[i];
            vertexIndices.insert(cell->pointIdx[0]);
            vertexIndices.insert(cell->pointIdx[1]);
            vertexIndices.insert(cell->pointIdx[2]);
        }

        auto itr          = mesh->begin<sight::data::iterator::ConstPointIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstPointIterator>();

        size_t count = 0;
        for( ; itr != itrEnd ; ++itr)
        {
            auto iter = std::find(vertexIndices.begin(), vertexIndices.end(), count);
            if(iter != vertexIndices.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, itr->rgba->r);
                CPPUNIT_ASSERT_EQUAL(G, itr->rgba->g);
                CPPUNIT_ASSERT_EQUAL(B, itr->rgba->b);
                CPPUNIT_ASSERT_EQUAL(A, itr->rgba->a);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->r);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->g);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->a);
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
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        geometry::data::Mesh::colorizeMeshCells(mesh, R, G, B);

        const auto dumpLock = mesh->lock();

        auto itr          = mesh->begin<sight::data::iterator::ConstCellIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstCellIterator>();

        size_t count = 0;
        for( ; itr != itrEnd ; ++itr, ++count)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(R), static_cast<int>(itr->rgba->r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(G), static_cast<int>(itr->rgba->g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(B), static_cast<int>(itr->rgba->b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(255), static_cast<int>(itr->rgba->a));
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
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        geometry::data::Mesh::colorizeMeshCells(mesh, R, G, B, A);

        const auto dumpLock = mesh->lock();

        auto itr          = mesh->begin<sight::data::iterator::ConstCellIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstCellIterator>();

        size_t count = 0;
        for( ; itr != itrEnd ; ++itr, ++count)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(R), static_cast<int>(itr->rgba->r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(G), static_cast<int>(itr->rgba->g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(B), static_cast<int>(itr->rgba->b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(std::to_string(count), static_cast<int>(A), static_cast<int>(itr->rgba->a));
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
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        std::vector<size_t> vectorNumTriangle = {{2, 3, 18, 23, 6}};

        // fill color with 0
        geometry::data::Mesh::colorizeMeshCells(mesh, 0, 0, 0, 0);

        geometry::data::Mesh::colorizeMeshCells(mesh, vectorNumTriangle, R, G, B);

        const auto dumpLock = mesh->lock();

        auto itr          = mesh->begin<sight::data::iterator::ConstCellIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstCellIterator>();

        size_t count = 0;
        for( ; itr != itrEnd ; ++itr, ++count)
        {
            auto iter = std::find(vectorNumTriangle.begin(), vectorNumTriangle.end(), count);
            if(iter != vectorNumTriangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, itr->rgba->r);
                CPPUNIT_ASSERT_EQUAL(G, itr->rgba->g);
                CPPUNIT_ASSERT_EQUAL(B, itr->rgba->b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(255), itr->rgba->a);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->r);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->g);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->a);
            }
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
            mesh->getNumberOfPoints(),
            mesh->getNumberOfCells(),
            mesh->getCellDataSize(),
            sight::data::Mesh::Attributes::CELL_COLORS
        );

        std::vector<size_t> vectorNumTriangle = {{2, 3, 18, 23, 6, 5}};

        // fill color with 0
        geometry::data::Mesh::colorizeMeshCells(mesh, 0, 0, 0, 0);

        geometry::data::Mesh::colorizeMeshCells(mesh, vectorNumTriangle, R, G, B, A);

        const auto dumpLock = mesh->lock();

        auto itr          = mesh->begin<sight::data::iterator::ConstCellIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstCellIterator>();

        size_t count = 0;
        for( ; itr != itrEnd ; ++itr, ++count)
        {
            auto iter = std::find(vectorNumTriangle.begin(), vectorNumTriangle.end(), count);
            if(iter != vectorNumTriangle.end())
            {
                CPPUNIT_ASSERT_EQUAL(R, itr->rgba->r);
                CPPUNIT_ASSERT_EQUAL(G, itr->rgba->g);
                CPPUNIT_ASSERT_EQUAL(B, itr->rgba->b);
                CPPUNIT_ASSERT_EQUAL(A, itr->rgba->a);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->r);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->g);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->b);
                CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(0), itr->rgba->a);
            }
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::transformTest()
{
    sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
    const auto lock              = mesh->lock();
    utestData::generator::Mesh::generateTriangleMesh(mesh);
    geometry::data::Mesh::generatePointNormals(mesh);
    geometry::data::Mesh::generateCellNormals(mesh);

    sight::data::Mesh::sptr meshOrig = sight::data::Mesh::copy(mesh);
    const auto origLock              = meshOrig->lock();

    ::glm::dmat4x4 matrix(1.);
    matrix = ::glm::rotate(matrix, ::glm::radians(90.), ::glm::dvec3(0., 0., 1.));
    matrix = ::glm::translate(matrix, ::glm::dvec3(10., 20., 30.));
    matrix = ::glm::scale(matrix, ::glm::dvec3(2., 2., 2.));

    auto trans = sight::data::Matrix4::New();
    geometry::data::setTF3DFromMatrix(trans, matrix);

    geometry::data::Mesh::transform(meshOrig, mesh, trans);

    const auto dumpLock = mesh->lock();
    {
        auto origItr      = meshOrig->begin<sight::data::iterator::ConstPointIterator>();
        auto itr          = mesh->begin<sight::data::iterator::ConstPointIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstPointIterator>();

        for( ; itr != itrEnd ; ++itr, ++origItr)
        {
            // Test points transform
            const ::glm::vec4 pt(origItr->point->x, origItr->point->y, origItr->point->z, 1.);
            const ::glm::vec4 transformedPt = matrix * pt;

            CPPUNIT_ASSERT_EQUAL(transformedPt.x, itr->point->x);
            CPPUNIT_ASSERT_EQUAL(transformedPt.y, itr->point->y);
            CPPUNIT_ASSERT_EQUAL(transformedPt.z, itr->point->z);

            // Test points normals transform
            const ::glm::vec4 n(origItr->normal->nx, origItr->normal->ny, origItr->normal->nz, 0.);
            const ::glm::vec4 transformedNormal = ::glm::normalize(matrix * n);

            CPPUNIT_ASSERT_EQUAL(transformedNormal.x, itr->normal->nx);
            CPPUNIT_ASSERT_EQUAL(transformedNormal.y, itr->normal->ny);
            CPPUNIT_ASSERT_EQUAL(transformedNormal.z, itr->normal->nz);
        }
    }

    // Test cells normals transform
    {
        auto origItr      = meshOrig->begin<sight::data::iterator::ConstCellIterator>();
        auto itr          = mesh->begin<sight::data::iterator::ConstCellIterator>();
        const auto itrEnd = mesh->end<sight::data::iterator::ConstCellIterator>();

        for( ; itr != itrEnd ; ++itr, ++origItr)
        {
            const ::glm::vec4 n(origItr->normal->nx, origItr->normal->ny, origItr->normal->nz, 0.);
            const ::glm::vec4 transformedNormal = ::glm::normalize(matrix * n);

            CPPUNIT_ASSERT_EQUAL(transformedNormal.x, itr->normal->nx);
            CPPUNIT_ASSERT_EQUAL(transformedNormal.y, itr->normal->ny);
            CPPUNIT_ASSERT_EQUAL(transformedNormal.z, itr->normal->nz);
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
        const auto dumpLock          = mesh->lock();

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
        const auto dumpLock          = mesh->lock();

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
        const auto dumpLock          = mesh->lock();

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
        const auto dumpLock          = mesh->lock();

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
    const auto dumpLock          = mesh->lock();

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

    auto cellIter = mesh->begin<sight::data::iterator::ConstCellIterator>();

    // check first cell normal = {0, 0, -1}
    std::array<float, 3> n = {0.f, 0.f, -1.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->normal->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->normal->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->normal->nz);

    // check cell 2 normal = {0, 1, 0}
    ++cellIter;
    n = {0.f, 1.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->normal->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->normal->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->normal->nz);

    // check cell 3 normal = {1, 0, 0}
    ++cellIter;
    n = {1.f, 0.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->normal->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->normal->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->normal->nz);

    // check cell 4 normal = {0, 0, 1}
    ++cellIter;
    n = {0.f, 0.f, 1.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->normal->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->normal->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->normal->nz);

    // check cell 5 normal = {-1, 0, 0}
    ++cellIter;
    n = {-1.f, 0.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->normal->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->normal->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->normal->nz);

    // check cell 6 normal = {0, -1, 0}
    ++cellIter;
    n = {0.f, -1.f, 0.f};
    CPPUNIT_ASSERT_EQUAL(n[0], cellIter->normal->nx);
    CPPUNIT_ASSERT_EQUAL(n[1], cellIter->normal->ny);
    CPPUNIT_ASSERT_EQUAL(n[2], cellIter->normal->nz);
}

//------------------------------------------------------------------------------

void MeshTest::pointNormalTest()
{
    sight::data::Mesh::sptr mesh = sight::data::Mesh::New();
    const auto dumpLock          = mesh->lock();

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

    auto pointIter          = mesh->begin<sight::data::iterator::ConstPointIterator>();
    const auto pointIterEnd = mesh->begin<sight::data::iterator::ConstPointIterator>();

    // check first point normal = {-0.57735, 0.57735, -0.57735}
    std::array<float, 3> n = {-0.57735f, 0.57735f, -0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);

    // check point 2 normal = {0.57735, 0.57735, -0.57735}
    ++pointIter;
    n = {0.57735f, 0.57735f, -0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);

    // check point 3 normal = {0.57735, -0.57735, -0.57735}
    ++pointIter;
    n = {0.57735f, -0.57735f, -.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);

    // check point 4 normal = {-0.57735, -0.57735, -0.57735}
    ++pointIter;
    n = {-0.57735f, -0.57735f, -0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);

    // check point 5 normal = {-0.57735, 0.57735, 0.57735}
    ++pointIter;
    n = {-0.57735f, 0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);

    // check point 6 normal = {0.57735, 0.57735, 0.57735}
    ++pointIter;
    n = {0.57735f, 0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);

    // check point 7 normal = {0.57735, -0.57735, 0.57735}
    ++pointIter;
    n = {0.57735f, -0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);

    // check point 8 normal = {-0.57735, -0.57735, 0.57735}
    ++pointIter;
    n = {-0.57735f, -0.57735f, 0.57735f};
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[0], pointIter->normal->nx, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[1], pointIter->normal->ny, 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(n[2], pointIter->normal->nz, 0.00001);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::geometry::data
