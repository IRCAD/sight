/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <core/Profiling.hpp>

#include <data/Mesh.hpp>
#include <data/ObjectLock.hpp>

#include <utestData/generator/Mesh.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <tuple>
#include <utility>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::MeshTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::insertion()
{
    {
        data::Mesh::sptr mesh = data::Mesh::New();
        const auto lock       = mesh->lock();
        mesh->pushPoint(10, 20, 30);
        mesh->pushPoint(10, 10, 10);
        mesh->pushPoint(20, 21, 10);
        mesh->pushPoint(30, 30, 10);
        mesh->pushPoint(15, 20, 35);
        mesh->pushPoint(20, 20, 10);
        mesh->pushPoint(20, 63, 17);
        mesh->pushPoint(27, 83, 52);

        mesh->pushCell(1, 2, 3);
        mesh->pushCell(1, 3, 4);
        mesh->pushCell(1, 2, 5);

        data::Mesh::point_t p[3] = {3, 6, 5};
        mesh->pushCell(p, 3);

        data::Mesh::point_t p2[3] = {1, 3, 5};
        mesh->pushCell(p2, 3);

        data::Mesh::point_t p3[3] = {7, 2, 5};
        mesh->pushCell(p3, 3);

        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(8), mesh->numPoints());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(6), mesh->numCells());

        auto it = mesh->cbegin<data::iterator::point::xyz>();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(10.f, it->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(20.f, it->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(30.f, it->z, EPSILON);
        it += 2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(20.f, it->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(21.f, it->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(10.f, it->z, EPSILON);
        it += 5;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(27.f, it->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(83.f, it->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(52.f, it->z, EPSILON);
        --it;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(20.f, it->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(63.f, it->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(17.f, it->z, EPSILON);
        it -= 2;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(15.f, it->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(20.f, it->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(35.f, it->z, EPSILON);

        auto cellit = mesh->cbegin<data::iterator::cell::triangle>();
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(1), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(2), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(3), cellit->pt[2]);
        ++cellit;

        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(1), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(3), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(4), cellit->pt[2]);

        cellit += 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(3), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(6), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(5), cellit->pt[2]);

        cellit++;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(1), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(3), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(5), cellit->pt[2]);

        cellit += 1;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(7), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(2), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(5), cellit->pt[2]);

        --cellit;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(1), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(3), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(5), cellit->pt[2]);

        cellit--;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(3), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(6), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(5), cellit->pt[2]);
        cellit -= 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(1), cellit->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(3), cellit->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::point_t>(4), cellit->pt[2]);

        mesh->setPoint(4, 45, 59, 48);
        auto itPt = mesh->begin<data::iterator::point::xyz>();
        itPt += 4;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(45), itPt->x, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(59), itPt->y, EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(48), itPt->z, EPSILON);
    }

    {
        const data::Mesh::size_t NB_POINTS       = 60;
        const data::Mesh::size_t NB_CELLS        = 59;
        const data::Mesh::CellType CELL_TYPE     = data::Mesh::CellType::TRIANGLE;
        const data::Mesh::Attributes EXTRA_ARRAY =
            data::Mesh::Attributes::POINT_NORMALS
            | data::Mesh::Attributes::POINT_COLORS
            | data::Mesh::Attributes::POINT_TEX_COORDS
            | data::Mesh::Attributes::CELL_NORMALS
            | data::Mesh::Attributes::CELL_COLORS
            | data::Mesh::Attributes::CELL_TEX_COORDS;

        data::Mesh::sptr mesh = data::Mesh::New();

        mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
        const auto lock = mesh->lock();

        for(size_t i = 0 ; i < NB_POINTS ; ++i)
        {
            const std::uint8_t val                                = static_cast<uint8_t>(i);
            const std::array<data::Mesh::color_t, 4> color        = {val, val, val, val};
            const float floatVal                                  = static_cast<float>(i);
            const std::array<data::Mesh::normal_t, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array<data::Mesh::texcoord_t, 2> texCoords = {floatVal, floatVal};
            const size_t value                                    = 3 * i;
            const auto id                                         = mesh->pushPoint(
                static_cast<float>(value),
                static_cast<float>(value + 1),
                static_cast<float>(value + 2)
            );
            mesh->setPointColor(id, color);
            mesh->setPointNormal(id, normal);
            mesh->setPointTexCoord(id, texCoords);
        }

        for(data::Mesh::size_t i = 0 ; i < NB_CELLS ; ++i)
        {
            const auto id = mesh->pushCell(i, i + 1, i + 2);

            const data::Mesh::color_t val                         = static_cast<data::Mesh::color_t>(i);
            const std::array<data::Mesh::color_t, 4> color        = {val, val, val, val};
            const float floatVal                                  = static_cast<float>(i);
            const std::array<data::Mesh::normal_t, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array<data::Mesh::texcoord_t, 2> texCoords = {floatVal, floatVal};
            mesh->setCellColor(id, color);
            mesh->setCellNormal(id, normal);
            mesh->setCellTexCoord(id, texCoords);
        }

        CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh->numPoints());
        CPPUNIT_ASSERT_EQUAL(NB_CELLS, mesh->numCells());
        CPPUNIT_ASSERT_EQUAL(mesh->getAllocatedSizeInBytes(), mesh->getDataSizeInBytes());
        const bool resizeMemory = mesh->shrinkToFit();
        CPPUNIT_ASSERT_EQUAL(false, resizeMemory);
    }
    {
        const data::Mesh::size_t NB_POINTS = 1;
        const data::Mesh::size_t NB_CELLS  = 1;

        const data::Mesh::CellType CELL_TYPE     = data::Mesh::CellType::POINT;
        const data::Mesh::Attributes EXTRA_ARRAY =
            data::Mesh::Attributes::POINT_NORMALS
            | data::Mesh::Attributes::POINT_COLORS
            | data::Mesh::Attributes::POINT_TEX_COORDS;

        data::Mesh::sptr mesh = data::Mesh::New();

        mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
        const auto lock                          = mesh->lock();
        const size_t pointNormalsAllocatedSize   = NB_POINTS * 3 * sizeof(data::Mesh::normal_t);
        const size_t pointColorsAllocatedSize    = NB_POINTS * 4 * sizeof(data::Mesh::color_t);
        const size_t pointTexCoordsAllocatedSize = NB_POINTS * 2 * sizeof(data::Mesh::texcoord_t);
        const size_t pointPositionAllocatedSize  = NB_POINTS * 3 * sizeof(data::Mesh::position_t);

        // data::Mesh::CellType::POINT;
        const size_t cellDataAllocatedSize = NB_CELLS * 1 * sizeof(data::Mesh::cell_t);

        const size_t pointSize = pointPositionAllocatedSize + cellDataAllocatedSize
                                 + pointNormalsAllocatedSize + pointColorsAllocatedSize + pointTexCoordsAllocatedSize;

        CPPUNIT_ASSERT_EQUAL(
            pointSize,
            mesh->getAllocatedSizeInBytes()
        );
        for(size_t i = 0 ; i < 1001 ; ++i)
        {
            const std::uint8_t val                                = static_cast<uint8_t>(i);
            const std::array<data::Mesh::color_t, 4> color        = {val, val, val, val};
            const float floatVal                                  = static_cast<float>(i);
            const std::array<data::Mesh::normal_t, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array<data::Mesh::texcoord_t, 2> texCoords = {floatVal, floatVal};
            const size_t value                                    = 3 * i;
            const auto id                                         = mesh->pushPoint(
                static_cast<float>(value),
                static_cast<float>(value + 1),
                static_cast<float>(value + 2)
            );
            mesh->setPointColor(id, color);
            mesh->setPointNormal(id, normal);
            mesh->setPointTexCoord(id, texCoords);
        }

        using namespace data::iterator;
        {
            auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

            data::Mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                const float fValue = static_cast<float>(3 * count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue, p.x, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue + 1, p.y, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue + 2, p.z, EPSILON);
                const data::Mesh::color_t cVal = static_cast<data::Mesh::color_t>(count);
                CPPUNIT_ASSERT_EQUAL(cVal, c.r);
                CPPUNIT_ASSERT_EQUAL(cVal, c.g);
                CPPUNIT_ASSERT_EQUAL(cVal, c.b);
                CPPUNIT_ASSERT_EQUAL(cVal, c.a);
                const data::Mesh::normal_t fVal = static_cast<data::Mesh::normal_t>(count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, n.nx, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, n.ny, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, n.nz, EPSILON);

                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, uv.u, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, uv.v, EPSILON);

                ++count;
            }

            CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
        }

        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36040), mesh->getAllocatedSizeInBytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36036), mesh->getDataSizeInBytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh->numCells());
        const auto idx = mesh->pushPoint(0.0, 0.0, 0.0);
        mesh->setPointColor(idx, 255, 0, 0, 255);
        mesh->setPointNormal(idx, 2.0, 1.0, 3.0);
        mesh->setPointTexCoord(idx, 3.0, 1.0);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(72040), mesh->getAllocatedSizeInBytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36072), mesh->getDataSizeInBytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(1002), mesh->numPoints());
        CPPUNIT_ASSERT_EQUAL(EXTRA_ARRAY, mesh->getAttributes());
        CPPUNIT_ASSERT(mesh->has<data::Mesh::Attributes::POINT_COLORS>());
        CPPUNIT_ASSERT(mesh->has<data::Mesh::Attributes::POINT_NORMALS>());
        CPPUNIT_ASSERT(mesh->has<data::Mesh::Attributes::POINT_TEX_COORDS>());
    }

    {
        const data::Mesh::size_t NB_POINTS            = 1;
        const data::Mesh::size_t NB_CELLS             = 1;
        const data::Mesh::CellType CELL_TYPE1         = data::Mesh::CellType::TRIANGLE;
        const data::Mesh::Attributes ATTRIBUTES_ARRAY =
            data::Mesh::Attributes::CELL_NORMALS
            | data::Mesh::Attributes::CELL_COLORS
            | data::Mesh::Attributes::CELL_TEX_COORDS;
        data::Mesh::sptr mesh = data::Mesh::New();
        mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE1, ATTRIBUTES_ARRAY);
        const auto lock                          = mesh->lock();
        const size_t pointNormalsAllocatedSize   = NB_POINTS * 3 * sizeof(data::Mesh::normal_t);
        const size_t pointColorsAllocatedSize    = NB_POINTS * 4 * sizeof(data::Mesh::color_t);
        const size_t pointTexCoordsAllocatedSize = NB_POINTS * 2 * sizeof(data::Mesh::texcoord_t);
        const size_t pointPositionAllocatedSize  = NB_POINTS * 3 * sizeof(data::Mesh::position_t);

        // data::Mesh::CellType::TRIANGLE;
        const size_t cellDataAllocatedSize = NB_CELLS * 3 * sizeof(data::Mesh::cell_t);

        const size_t pointSize = pointPositionAllocatedSize + cellDataAllocatedSize
                                 + pointNormalsAllocatedSize + pointColorsAllocatedSize + pointTexCoordsAllocatedSize;

        CPPUNIT_ASSERT_EQUAL(
            pointSize,
            mesh->getAllocatedSizeInBytes()
        );

        for(data::Mesh::size_t i = 0 ; i < 1001 ; ++i)
        {
            const auto id = mesh->pushCell(i, i + 1, i + 2);

            const data::Mesh::color_t val                         = static_cast<data::Mesh::color_t>(i);
            const std::array<data::Mesh::color_t, 4> color        = {val, val, val, val};
            const float floatVal                                  = static_cast<float>(i);
            const std::array<data::Mesh::normal_t, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array<data::Mesh::texcoord_t, 2> texCoords = {floatVal, floatVal};
            mesh->setCellColor(id, color);
            mesh->setCellNormal(id, normal);
            mesh->setCellTexCoord(id, texCoords);
        }

        using namespace data::iterator;
        {
            const auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

            data::Mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(count), p.pt[0]);
                CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(count + 1), p.pt[1]);
                CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(count + 2), p.pt[2]);

                const data::Mesh::color_t cVal = static_cast<data::Mesh::color_t>(count);
                CPPUNIT_ASSERT_EQUAL(cVal, c.r);
                CPPUNIT_ASSERT_EQUAL(cVal, c.g);
                CPPUNIT_ASSERT_EQUAL(cVal, c.b);
                CPPUNIT_ASSERT_EQUAL(cVal, c.a);

                const data::Mesh::normal_t fVal = static_cast<data::Mesh::normal_t>(count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, n.nx, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, n.ny, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, n.nz, EPSILON);

                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, uv.u, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(fVal, uv.v, EPSILON);

                ++count;
            }

            CPPUNIT_ASSERT_EQUAL(mesh->numCells(), count);
        }

        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36048), mesh->getAllocatedSizeInBytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36036), mesh->getDataSizeInBytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh->numPoints());

        CPPUNIT_ASSERT_EQUAL(
            data::Mesh::Attributes::CELL_NORMALS
            | data::Mesh::Attributes::CELL_COLORS
            | data::Mesh::Attributes::CELL_TEX_COORDS,
            mesh->getAttributes()
        );
        CPPUNIT_ASSERT_EQUAL(ATTRIBUTES_ARRAY, mesh->getAttributes());
        CPPUNIT_ASSERT(mesh->has<data::Mesh::Attributes::CELL_COLORS>());
        CPPUNIT_ASSERT(mesh->has<data::Mesh::Attributes::CELL_NORMALS>());
        CPPUNIT_ASSERT(mesh->has<data::Mesh::Attributes::CELL_TEX_COORDS>());
    }
}

//-----------------------------------------------------------------------------

void MeshTest::copy()
{
    data::Mesh::sptr mesh            = data::Mesh::New();
    data::Mesh::sptr shallowCopyMesh = data::Mesh::New();

    const auto dumpLock = mesh->lock();

    mesh->pushPoint(10, 20, 30);
    mesh->pushPoint(10, 10, 10);
    mesh->pushPoint(20, 20, 10);
    mesh->pushPoint(30, 30, 10);
    mesh->pushPoint(15, 20, 35);
    mesh->pushPoint(20, 20, 10);
    mesh->pushPoint(20, 63, 17);
    mesh->pushPoint(27, 83, 52);

    mesh->pushCell(1, 2, 0, 2);
    mesh->pushCell(1, 3, 4, 5);
    mesh->pushCell(1, 2, 5, 4);

    data::Mesh::cell_t pt1[4] = {3, 6, 5, 1};
    mesh->pushCell(pt1, 4);

    data::Mesh::cell_t pt2[4] = {1, 3, 5, 7};
    mesh->pushCell(pt2, 4);

    data::Mesh::cell_t pt3[4] = {7, 2, 5, 4};
    mesh->pushCell(pt3, 4);

    mesh->resize(
        mesh->numPoints(),
        mesh->numCells(),
        data::Mesh::CellType::QUAD,
        data::Mesh::Attributes::POINT_COLORS | data::Mesh::Attributes::POINT_TEX_COORDS
    );

    using namespace data::iterator;

    // check deep copy
    {
        data::Mesh::sptr deepCopyMesh;
        deepCopyMesh = data::Object::copy(mesh);
        const auto copyDumpLock = deepCopyMesh->lock();

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), deepCopyMesh->numPoints());
        CPPUNIT_ASSERT_EQUAL(mesh->numCells(), deepCopyMesh->numCells());
        CPPUNIT_ASSERT_EQUAL(mesh->getDataSizeInBytes(), deepCopyMesh->getDataSizeInBytes());

        auto pointItr         = mesh->cbegin<point::xyz>();
        const auto pointEnd   = mesh->cend<point::xyz>();
        auto pointItrCopyMesh = deepCopyMesh->cbegin<point::xyz>();

        for( ; pointItr != pointEnd ; ++pointItr, ++pointItrCopyMesh)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr->x, pointItrCopyMesh->x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr->y, pointItrCopyMesh->y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(pointItr->z, pointItrCopyMesh->z, EPSILON);
        }

        const sight::data::Mesh::csptr mesh3 = mesh; // copy into a const pointer to explicit test const begin()
        auto cellItr                         = mesh3->begin<cell::quad>();
        const auto cellEnd                   = mesh3->end<cell::quad>();
        auto cellItrCopyMesh                 = deepCopyMesh->cbegin<cell::quad>();

        for( ; cellItr != cellEnd ; ++cellItr, ++cellItrCopyMesh)
        {
            for(size_t i = 0 ; i < 4 ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(cellItr->pt[i], cellItrCopyMesh->pt[i]);
            }
        }
    }

    //check shallow copy
    {
        shallowCopyMesh->shallowCopy(mesh);
        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), shallowCopyMesh->numPoints());
        CPPUNIT_ASSERT_EQUAL(mesh->numCells(), shallowCopyMesh->numCells());
        CPPUNIT_ASSERT_EQUAL(mesh->getDataSizeInBytes(), shallowCopyMesh->getDataSizeInBytes());

        {
            auto pointItr     = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>().begin();
            auto pointItrCopy = shallowCopyMesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>().begin();

            const auto& [p1, n1, c1, u1] = *pointItr;
            const auto& [p2, n2, c2, u2] = *pointItrCopy;
            CPPUNIT_ASSERT_EQUAL(&p1.x, &p2.x);
            CPPUNIT_ASSERT_EQUAL(&n1.nx, &n2.nx);
            CPPUNIT_ASSERT_EQUAL(&c1.r, &c2.r);
            CPPUNIT_ASSERT_EQUAL(&u1.u, &u2.u);
        }

        {
            const auto cellItr         = mesh->zip_range<cell::quad, cell::rgba, cell::uv>().begin();
            const auto cellItrCopyMesh = shallowCopyMesh->zip_range<cell::quad, cell::rgba, cell::uv>().begin();

            const auto& [i1, c1, t1] = *cellItr;
            const auto& [i2, c2, t2] = *cellItrCopyMesh;
            CPPUNIT_ASSERT_EQUAL(&i1.pt[0], &i2.pt[0]);
            CPPUNIT_ASSERT_EQUAL(&c1.r, &c2.r);
            CPPUNIT_ASSERT_EQUAL(&t1.u, &t2.u);
        }
    }
}

//------------------------------------------------------------------------------

void MeshTest::allocationTest()
{
    data::Mesh::sptr mesh = data::Mesh::New();

    const auto lock = mesh->lock();

    mesh->pushPoint(10, 20, 30);
    mesh->pushPoint(10, 10, 10);
    mesh->pushPoint(20, 20, 10);

    mesh->pushCell(0, 1, 2);
    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(3), mesh->numPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(1), mesh->numCells());

    CPPUNIT_ASSERT_EQUAL(
        size_t(3 * 3 * sizeof(data::Mesh::position_t) + 3 * sizeof(data::Mesh::cell_t)),
        mesh->getDataSizeInBytes()
    );

    data::Mesh::size_t pointSize = 3000;
    data::Mesh::size_t cellSize  = 2000;
    mesh->reserve(pointSize, cellSize, data::Mesh::CellType::TRIANGLE);

    const size_t pointPositionAllocatedSize = pointSize * 3 * sizeof(data::Mesh::position_t);
    const size_t cellDataAllocatedSize      = cellSize * 3 * sizeof(data::Mesh::cell_t);

    CPPUNIT_ASSERT_EQUAL(
        pointPositionAllocatedSize + cellDataAllocatedSize,
        mesh->getAllocatedSizeInBytes()
    );

    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(3), mesh->numPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(1), mesh->numCells());

    CPPUNIT_ASSERT_EQUAL(data::Mesh::Attributes::NONE, mesh->getAttributes());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::POINT_COLORS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_COLORS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::POINT_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::POINT_TEX_COORDS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_TEX_COORDS>());

    mesh->reserve(
        pointSize,
        cellSize,
        data::Mesh::CellType::TRIANGLE,
        data::Mesh::Attributes::POINT_NORMALS
        | data::Mesh::Attributes::POINT_COLORS | data::Mesh::Attributes::POINT_TEX_COORDS
    );

    const size_t pointNormalsAllocatedSize   = pointSize * 3 * sizeof(data::Mesh::normal_t);
    const size_t pointColorsAllocatedSize    = pointSize * 4 * sizeof(data::Mesh::color_t);
    const size_t pointTexCoordsAllocatedSize = pointSize * 2 * sizeof(data::Mesh::texcoord_t);
    CPPUNIT_ASSERT_EQUAL(
        pointPositionAllocatedSize + cellDataAllocatedSize
        + pointNormalsAllocatedSize + pointColorsAllocatedSize + pointTexCoordsAllocatedSize,
        mesh->getAllocatedSizeInBytes()
    );

    CPPUNIT_ASSERT_EQUAL(
        data::Mesh::Attributes::POINT_NORMALS
        | data::Mesh::Attributes::POINT_COLORS | data::Mesh::Attributes::POINT_TEX_COORDS,
        mesh->getAttributes()
    );
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::POINT_COLORS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_COLORS>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::POINT_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::POINT_TEX_COORDS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_TEX_COORDS>());

    mesh->reserve(
        pointSize,
        cellSize,
        data::Mesh::CellType::TRIANGLE,
        data::Mesh::Attributes::POINT_NORMALS
        | data::Mesh::Attributes::POINT_COLORS
        | data::Mesh::Attributes::POINT_TEX_COORDS
        | data::Mesh::Attributes::CELL_NORMALS
        | data::Mesh::Attributes::CELL_COLORS
        | data::Mesh::Attributes::CELL_TEX_COORDS
    );

    size_t oldsize = mesh->getAllocatedSizeInBytes();
    bool adjusted  = mesh->shrinkToFit();
    size_t newSize = mesh->getAllocatedSizeInBytes();

    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize > newSize);

    CPPUNIT_ASSERT_EQUAL(
        data::Mesh::Attributes::POINT_NORMALS
        | data::Mesh::Attributes::POINT_COLORS
        | data::Mesh::Attributes::POINT_TEX_COORDS
        | data::Mesh::Attributes::CELL_NORMALS
        | data::Mesh::Attributes::CELL_COLORS
        | data::Mesh::Attributes::CELL_TEX_COORDS,
        mesh->getAttributes()
    );
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::POINT_COLORS>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::CELL_COLORS>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::POINT_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::CELL_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::POINT_TEX_COORDS>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::Mesh::Attributes::CELL_TEX_COORDS>());

    mesh->clear();
    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh->numPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::Mesh::size_t>(0), mesh->numCells());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), mesh->getDataSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::POINT_COLORS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_COLORS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::POINT_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_NORMALS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::POINT_TEX_COORDS>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::Mesh::Attributes::CELL_TEX_COORDS>());
}

//------------------------------------------------------------------------------

void MeshTest::iteratorTest()
{
    auto msg = [](auto& count){return "iteration: " + std::to_string(count);};

    const data::Mesh::size_t NB_POINTS       = 60;
    const data::Mesh::size_t NB_CELLS        = 59;
    const data::Mesh::CellType CELL_TYPE     = data::Mesh::CellType::TRIANGLE;
    const data::Mesh::Attributes EXTRA_ARRAY =
        data::Mesh::Attributes::POINT_NORMALS
        | data::Mesh::Attributes::POINT_COLORS
        | data::Mesh::Attributes::POINT_TEX_COORDS
        | data::Mesh::Attributes::CELL_NORMALS
        | data::Mesh::Attributes::CELL_COLORS
        | data::Mesh::Attributes::CELL_TEX_COORDS;

    data::Mesh::sptr mesh = data::Mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for(data::Mesh::size_t i = 0 ; i < NB_POINTS ; ++i)
    {
        const std::uint8_t val                                = static_cast<uint8_t>(i);
        const std::array<data::Mesh::color_t, 4> color        = {val, val, val, val};
        const float floatVal                                  = static_cast<float>(i);
        const std::array<data::Mesh::normal_t, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array<data::Mesh::texcoord_t, 2> texCoords = {floatVal, floatVal};
        const size_t value                                    = 3 * i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value + 1), static_cast<float>(value + 2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for(data::Mesh::size_t i = 0 ; i < NB_CELLS ; ++i)
    {
        mesh->setCell(i, i, i + 1, i + 2);

        const data::Mesh::color_t val                         = static_cast<data::Mesh::color_t>(i);
        const std::array<data::Mesh::color_t, 4> color        = {val, val, val, val};
        const float floatVal                                  = static_cast<float>(i);
        const std::array<data::Mesh::normal_t, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array<data::Mesh::texcoord_t, 2> texCoords = {floatVal, floatVal};
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }

    using namespace data::iterator;
    {
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

        data::Mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            const float fValue = static_cast<float>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue, p.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue + 1, p.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue + 2, p.z, EPSILON);

            const data::Mesh::color_t cVal = static_cast<data::Mesh::color_t>(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.a);

            const data::Mesh::normal_t fVal = static_cast<data::Mesh::normal_t>(count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nz, EPSILON);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
    }

    {
        const sight::data::Mesh::csptr mesh3 = mesh; // copy into a const pointer to explicit test const begin()
        auto range                           = mesh3->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>();
        auto begin                           = std::make_reverse_iterator(range.end());
        auto end                             = std::make_reverse_iterator(range.begin());

        size_t count = NB_POINTS;
        std::for_each(
            begin,
            end,
            [&](const auto& it)
                {
                    const auto& [p, n, c, uv] = it;
                    --count;
                    const float fValue = static_cast<float>(3 * count);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fValue, p.x, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fValue + 1, p.y, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fValue + 2, p.z, EPSILON);

                    const data::Mesh::color_t cVal = static_cast<data::Mesh::color_t>(count);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.r);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.g);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.b);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.a);

                    const data::Mesh::normal_t fVal = static_cast<data::Mesh::normal_t>(count);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nx, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.ny, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nz, EPSILON);

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.u, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.v, EPSILON);
                });

        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), count);
    }

    {
        const auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

        data::Mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::Mesh::size_t>(count), p.pt[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::Mesh::size_t>(count + 1), p.pt[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::Mesh::size_t>(count + 2), p.pt[2]);

            const data::Mesh::color_t cVal = static_cast<data::Mesh::color_t>(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.a);

            const data::Mesh::normal_t fVal = static_cast<data::Mesh::normal_t>(count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nz, EPSILON);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numCells(), count);
    }

    {
        auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();
        auto begin = std::make_reverse_iterator(range.end());
        auto end   = std::make_reverse_iterator(range.begin());

        data::Mesh::size_t count = 0;
        std::for_each(
            begin,
            end,
            [&](const auto& it)
                {
                    const auto& [p, n, c, uv] = it;

                    CPPUNIT_ASSERT_EQUAL_MESSAGE(
                        msg(count),
                        static_cast<data::Mesh::size_t>(NB_CELLS - 1 - count),
                        p.pt[0]
                    );
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(
                        msg(count),
                        static_cast<data::Mesh::size_t>(NB_CELLS - 1 - count + 1),
                        p.pt[1]
                    );
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(
                        msg(count),
                        static_cast<data::Mesh::size_t>(NB_CELLS - 1 - count + 2),
                        p.pt[2]
                    );

                    const data::Mesh::color_t cVal = static_cast<data::Mesh::color_t>(NB_CELLS - 1 - count);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.r);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.g);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.b);
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.a);

                    const data::Mesh::normal_t fVal = static_cast<data::Mesh::normal_t>(NB_CELLS - 1 - count);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nx, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.ny, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nz, EPSILON);

                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.u, EPSILON);
                    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.v, EPSILON);
                    ++count;
                });

        CPPUNIT_ASSERT_EQUAL(mesh->numCells(), count);
    }

    data::Mesh::csptr mesh2 = data::Mesh::copy(mesh);
    const auto lock2        = mesh2->lock();
    {
        const auto range = mesh2->crange<point::xyz>();

        float count = 0;
        for(const auto& point : range)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, point.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, point.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, point.z, EPSILON);
        }
    }

    {
        auto range     = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();
        auto it        = std::make_reverse_iterator(range.end());
        const auto end = std::make_reverse_iterator(range.begin());

        data::Mesh::size_t count = 0;
        for( ; it != end ; ++it)
        {
            const auto& [p, n, c, uv] = *it;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::Mesh::size_t>(NB_CELLS - 1 - count), p.pt[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                msg(count),
                static_cast<data::Mesh::size_t>(NB_CELLS - 1 - count + 1),
                p.pt[1]
            );
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                msg(count),
                static_cast<data::Mesh::size_t>(NB_CELLS - 1 - count + 2),
                p.pt[2]
            );

            const data::Mesh::color_t cVal = static_cast<data::Mesh::color_t>(NB_CELLS - 1 - count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), cVal, c.a);

            const data::Mesh::normal_t fVal =
                static_cast<data::Mesh::normal_t>(NB_CELLS - 1 - count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, n.nz, EPSILON);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), fVal, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numCells(), count);
    }

    data::Mesh::sptr mesh3 = data::Mesh::New();
    const auto lock3       = mesh3->lock();
    mesh3->resize(NB_POINTS, NB_CELLS, data::Mesh::CellType::QUAD, EXTRA_ARRAY);

    {
        auto range = mesh3->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

        uint32_t count = 0;
        for(auto&& [p, n, c, uv] : range)
        {
            p.x = static_cast<float>(3 * count);
            p.y = static_cast<float>(3 * count + 1);
            p.z = static_cast<float>(3 * count + 2);

            c.r = static_cast<std::uint8_t>(4 * count);
            c.g = static_cast<std::uint8_t>(4 * count + 1);
            c.b = static_cast<std::uint8_t>(4 * count + 2);
            c.a = static_cast<std::uint8_t>(4 * count + 3);

            n.nx = static_cast<float>(3 * count + 1);
            n.ny = static_cast<float>(3 * count + 2);
            n.nz = static_cast<float>(3 * count + 3);

            uv.u = static_cast<float>(2 * count);
            uv.v = static_cast<float>(2 * count + 1);
            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
    }

    {
        auto range = mesh3->zip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

        data::Mesh::size_t count = 0;
        for(auto&& [p, n, c, uv] : range)
        {
            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                p.pt[i] = count + i;
            }

            c.r = static_cast<std::uint8_t>(4 * count);
            c.g = static_cast<std::uint8_t>(4 * count + 1);
            c.b = static_cast<std::uint8_t>(4 * count + 2);
            c.a = static_cast<std::uint8_t>(4 * count + 3);

            n.nx = static_cast<float>(3 * count + 1);
            n.ny = static_cast<float>(3 * count + 2);
            n.nz = static_cast<float>(3 * count + 3);

            uv.u = static_cast<float>(2 * count);
            uv.v = static_cast<float>(2 * count + 1);
            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numCells(), count);
    }

    {
        auto range = mesh3->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

        data::Mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            const float fValue = static_cast<float>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue, p.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue + 1, p.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(fValue + 2, p.z, EPSILON);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count, static_cast<data::Mesh::size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 1, static_cast<data::Mesh::size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 2, static_cast<data::Mesh::size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 3, static_cast<data::Mesh::size_t>(c.a));

            const data::Mesh::normal_t nVal = static_cast<data::Mesh::normal_t>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), nVal + 1, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), nVal + 2, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), nVal + 3, n.nz, EPSILON);

            const data::Mesh::normal_t uvVal = static_cast<data::Mesh::normal_t>(2 * count);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uvVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uvVal + 1, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
    }

    {
        auto range = mesh3->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

        data::Mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::Mesh::size_t>(count), p.pt[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::Mesh::size_t>(count + 1), p.pt[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::Mesh::size_t>(count + 2), p.pt[2]);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count, static_cast<data::Mesh::size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 1, static_cast<data::Mesh::size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 2, static_cast<data::Mesh::size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 3, static_cast<data::Mesh::size_t>(c.a));

            const data::Mesh::normal_t nVal = static_cast<data::Mesh::normal_t>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), nVal + 1, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), nVal + 2, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), nVal + 3, n.nz, EPSILON);

            const data::Mesh::normal_t uvVal = static_cast<data::Mesh::normal_t>(2 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uvVal, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uvVal + 1, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numCells(), count);
    }
}

//------------------------------------------------------------------------------

void MeshTest::iteratorCopyTest()
{
    auto msg = [](auto& count){return "iteration: " + std::to_string(count);};

    data::Mesh::sptr mesh = data::Mesh::New();
    mesh->resize(
        10,
        10,
        data::Mesh::CellType::POINT,
        data::Mesh::Attributes::POINT_NORMALS
        | data::Mesh::Attributes::POINT_COLORS
        | data::Mesh::Attributes::POINT_TEX_COORDS
        | data::Mesh::Attributes::CELL_NORMALS
        | data::Mesh::Attributes::CELL_COLORS
        | data::Mesh::Attributes::CELL_TEX_COORDS
    );

    const auto lock = mesh->lock();

    using namespace data::iterator;
    {
        //fill the mesh
        auto range = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point, cell::nxyz,
                                     cell::rgba, cell::uv>();

        data::Mesh::size_t count = 0;
        for(auto&& [p, n, c, uv, cell, cn, cc, cuv] : range)
        {
            // point
            p.x = static_cast<float>(count);
            p.y = static_cast<float>(count) + 0.6f;
            p.z = static_cast<float>(count) + 0.8f;

            n.nx = static_cast<float>(count) + 0.1f;
            n.ny = static_cast<float>(count) + 0.2f;
            n.nz = static_cast<float>(count) + 0.3f;

            c.r = static_cast<std::uint8_t>(4 * count);
            c.g = static_cast<std::uint8_t>(4 * count + 1);
            c.b = static_cast<std::uint8_t>(4 * count + 2);
            c.a = static_cast<std::uint8_t>(4 * count + 3);

            uv.u = static_cast<float>(count) + 0.7f;
            uv.v = static_cast<float>(count) + 0.9f;

            // cell
            cell.pt = count;

            cn.nx = static_cast<float>(count) + 0.11f;
            cn.ny = static_cast<float>(count) + 0.22f;
            cn.nz = static_cast<float>(count) + 0.33f;

            cc.r = static_cast<std::uint8_t>(4 * count + 1);
            cc.g = static_cast<std::uint8_t>(4 * count + 2);
            cc.b = static_cast<std::uint8_t>(4 * count + 3);
            cc.a = static_cast<std::uint8_t>(4 * count + 4);

            cuv.u = static_cast<float>(count) + 0.77f;
            cuv.v = static_cast<float>(count) + 0.99f;

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
    }

    {
        // check the mesh points
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point, cell::nxyz,
                                      cell::rgba, cell::uv>();

        data::Mesh::size_t count = 0;
        for(const auto& [p, n, c, uv, cell, cn, cc, cuv] : range)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count), p.x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.6f, p.y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.8f, p.z, 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.1f, n.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.2f, n.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.3f, n.nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count), c.r);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 1), c.g);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 2), c.b);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 3), c.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.7, uv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.9, uv.v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL(count, cell.pt);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.11f, cn.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.22f, cn.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.33f, cn.nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 1), cc.r);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 2), cc.g);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 3), cc.b);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 4), cc.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.77, cuv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.99, cuv.v, 0.00001);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
    }

    data::Mesh::sptr copiedMesh = data::Mesh::New();
    copiedMesh->resize(
        10,
        10,
        data::Mesh::CellType::POINT,
        data::Mesh::Attributes::POINT_NORMALS
        | data::Mesh::Attributes::POINT_COLORS
        | data::Mesh::Attributes::POINT_TEX_COORDS
        | data::Mesh::Attributes::CELL_NORMALS
        | data::Mesh::Attributes::CELL_COLORS
        | data::Mesh::Attributes::CELL_TEX_COORDS
    );
    const auto lock2 = copiedMesh->lock();
    {
        // copy the mesh points and cells with the iterator
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                      cell::point, cell::nxyz, cell::rgba, cell::uv>();
        auto rangeCopy = copiedMesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                               cell::point, cell::nxyz, cell::rgba, cell::uv>();

        std::copy(range.begin(), range.end(), rangeCopy.begin());
    }

    {
        // check the copied mesh
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point,
                                      cell::nxyz, cell::rgba, cell::uv>();
        auto rangeCopy = copiedMesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point,
                                                cell::nxyz, cell::rgba, cell::uv>();
        auto combinedRange = boost::combine(range, rangeCopy);

        data::Mesh::size_t count = 0;
        for(const auto& [pt1, pt2] : combinedRange)
        {
            const auto& [p1, n1, c1, uv1, cell1, cn1, cc1, cuv1] = pt1;
            const auto& [p2, n2, c2, uv2, cell2, cn2, cc2, cuv2] = pt2;

            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), p1.x, p2.x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), p1.y, p2.y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), p1.z, p2.z, 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n1.nx, n2.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n1.ny, n2.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n1.nz, n2.nz, 0.00001);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.r, c2.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.g, c2.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.b, c2.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.a, c2.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv1.u, uv2.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv1.v, uv2.v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL(cell1.pt, cell2.pt);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(cn1.nx, cn2.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cn1.ny, cn2.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cn1.nz, cn2.nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(cc1.r, cc2.r);
            CPPUNIT_ASSERT_EQUAL(cc1.g, cc2.g);
            CPPUNIT_ASSERT_EQUAL(cc1.b, cc2.b);
            CPPUNIT_ASSERT_EQUAL(cc1.a, cc2.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(cuv1.u, cuv2.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cuv1.v, cuv2.v, 0.00001);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
    }

    {
        // check the mesh points are deep copied and not shallow copied

        auto range = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                     cell::point, cell::nxyz, cell::rgba, cell::uv>();

        auto point = boost::make_tuple(
            data::iterator::point::xyz({1.0f, 1.1f, 1.2f}),
            data::iterator::point::nxyz({1.f, 0.f, 0.f}),
            data::iterator::point::rgba({25, 15, 18, 32}),
            data::iterator::point::uv({0.5f, 1.f}),
            data::iterator::cell::point({0}),
            data::iterator::cell::nxyz({0.f, 1.f, 0.f}),
            data::iterator::cell::rgba({20, 13, 10, 37}),
            data::iterator::cell::uv({0.2f, 0.8f})
        );
        std::fill(range.begin(), range.end(), point);

        // check the mesh points are filled
        data::Mesh::size_t count = 0;
        const auto& [p2, n2, c2, uv2, cell2, cn2, cc2, cuv2] = point;

        for(const auto& [p1, n1, c1, uv1, cell1, cn1, cc1, cuv1] : range)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), p1.x, p2.x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), p1.y, p2.y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), p1.z, p2.z, 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n1.nx, n2.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n1.ny, n2.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n1.nz, n2.nz, 0.00001);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.r, c2.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.g, c2.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.b, c2.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.a, c2.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv1.u, uv2.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv1.v, uv2.v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL(cell1.pt, cell2.pt);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(cn1.nx, cn2.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cn1.ny, cn2.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cn1.nz, cn2.nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(cc1.r, cc2.r);
            CPPUNIT_ASSERT_EQUAL(cc1.g, cc2.g);
            CPPUNIT_ASSERT_EQUAL(cc1.b, cc2.b);
            CPPUNIT_ASSERT_EQUAL(cc1.a, cc2.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(cuv1.u, cuv2.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(cuv1.v, cuv2.v, 0.00001);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);

        auto rangeCopy = copiedMesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                                cell::point, cell::nxyz, cell::rgba, cell::uv>();

        count = 0;
        for(const auto& [p, n, c, uv, cell, cn, cc, cuv] : rangeCopy)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count), p.x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.6f, p.y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.8f, p.z, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.1f, n.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.2f, n.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.3f, n.nz, 0.00001);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count), c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 1), c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 2), c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 3), c.a);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.7, uv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.9, uv.v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), count, cell.pt);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.11f, cn.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.22f, cn.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.33f, cn.nz, 0.00001);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 1), cc.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 2), cc.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 3), cc.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 4), cc.a);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.77, cuv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.99, cuv.v, 0.00001);
            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->numPoints(), count);
    }
}

//------------------------------------------------------------------------------

void MeshTest::benchmarkIterator()
{
    data::Mesh::sptr mesh = data::Mesh::New();
    const auto lock       = mesh->lock();

    static const size_t N = 1;

    mesh->resize(
        655360,
        10000,
        data::Mesh::CellType::TRIANGLE,
        data::Mesh::Attributes::POINT_NORMALS
    );

    auto pos  = mesh->begin<iterator::point::xyz>();
    auto norm = mesh->begin<iterator::point::nxyz>();

    auto pos2  = mesh->end<iterator::point::xyz>();
    auto norm2 = mesh->end<iterator::point::nxyz>();

    auto fn2 = [](const auto& tuple)
               {
                   auto& [p, n] = tuple;
                   p.x          = p.x > 2.f ? 5.f : 1.f;
                   p.y          = 2.f;
                   p.z          = 3.f;
                   n.nx         = -1.f;
                   n.ny         = -1.f;
                   n.nz         = 1.f;
               };

    auto fn3 = [](auto& p)
               {
                   p.x = p.x > 2.f ? 5.f : 1.f;
                   p.y = 2.f;
                   p.z = 3.f;
               };

    auto fn4 = [](auto& n)
               {
                   n.nx = n.ny > 2.f ? 5.f : 1.f;
                   n.ny = -1.f;
                   n.nz = 1.f;
               };

    {
        FW_PROFILE("std::for_each - array_iterator (nouveau en MR)");
        for(size_t i = 0 ; i < N ; ++i)
        {
            std::for_each(pos, pos2, fn3);
            std::for_each(norm, norm2, fn4);
        }
    }

    auto range = mesh->zip_range<iterator::point::xyz, iterator::point::nxyz>();
    {
        FW_PROFILE("std::for_each - zip array_iterator");
        for(size_t i = 0 ; i < N ; ++i)
        {
            std::for_each(range.begin(), range.end(), fn2);
        }
    }

    std::vector<iterator::point::xyz> posV(655360);
    std::vector<iterator::point::nxyz> normV(655360);
    {
        auto z = boost::combine(posV, normV);
        {
            FW_PROFILE("std::for - std boost combine");
            for(size_t i = 0 ; i < N ; ++i)
            {
                for(auto&& [p, n] : z)
                {
                    p.x  = p.x > 2.f ? 5.f : 1.f;
                    p.y  = 2.f;
                    p.z  = 3.f;
                    n.nx = -1.f;
                    n.ny = -1.f;
                    n.nz = 1.f;
                }
            }
        }
    }
}

} //namespace ut

} //namespace sight::data
