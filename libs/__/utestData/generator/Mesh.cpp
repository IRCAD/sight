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

// cspell:ignore NOLINT

#include "utestData/generator/Mesh.hpp"

#include <core/tools/random/generator.hpp>

#include <cstdlib>
#include <ctime>

namespace sight::utestData::generator
{

using core::tools::random::safe_rand;

struct RandFloat
{
    //------------------------------------------------------------------------------

    float operator()()
    {
        return (static_cast<float>(safe_rand() % 101) - 50.F) / 500.F;
    }
};

//------------------------------------------------------------------------------

void Mesh::generateMesh(const data::Mesh::sptr& mesh)
{
    const data::Mesh::Attributes attributes = data::Mesh::Attributes::POINT_COLORS
                                              | data::Mesh::Attributes::POINT_NORMALS
                                              | data::Mesh::Attributes::CELL_COLORS
                                              | data::Mesh::Attributes::CELL_NORMALS;

    utestData::generator::Mesh::generateTriangleMesh(mesh, attributes);
    utestData::generator::Mesh::shakePoints(mesh);
    mesh->shrinkToFit();
}

//------------------------------------------------------------------------------

void Mesh::generateTriangleQuadMesh(
    const data::Mesh::sptr& mesh,
    data::Mesh::Attributes attributes
)
{
    data::Mesh::size_t nbPointsByEdge = 10;
    float edgeDim                     = 100.;
    Mesh::PointsMapType points;

    const data::Mesh::size_t nbPoints        = nbPointsByEdge * nbPointsByEdge * 2 * 4 * 2;
    const data::Mesh::size_t nbTriangleCells = nbPointsByEdge * nbPointsByEdge * 2 * 2;

    mesh->clear();
    mesh->reserve(nbPoints, nbTriangleCells, data::Mesh::CellType::TRIANGLE, attributes);
    Mesh::addTriangleMesh(mesh, points, nbPointsByEdge, edgeDim);
}

//------------------------------------------------------------------------------

void Mesh::generateTriangleMesh(
    const data::Mesh::sptr& mesh,
    data::Mesh::Attributes attributes
)
{
    data::Mesh::size_t nbPointsByEdge = 10;
    float edgeDim                     = 100.;
    Mesh::PointsMapType points;
    const data::Mesh::size_t nbPoints = nbPointsByEdge * nbPointsByEdge * 2 * 4;
    const data::Mesh::size_t nbCells  = nbPointsByEdge * nbPointsByEdge * 2 * 2;

    mesh->clear();
    mesh->reserve(nbPoints, nbCells, data::Mesh::CellType::TRIANGLE, attributes);
    Mesh::addTriangleMesh(mesh, points, nbPointsByEdge, edgeDim);
    mesh->shrinkToFit();
}

//------------------------------------------------------------------------------

void Mesh::generateQuadMesh(
    const data::Mesh::sptr& mesh,
    data::Mesh::Attributes attributes
)
{
    data::Mesh::size_t nbPointsByEdge = 10;
    float edgeDim                     = 100.;
    Mesh::PointsMapType points;
    const data::Mesh::size_t nbPoints = nbPointsByEdge * nbPointsByEdge * 2 * 4;
    const data::Mesh::size_t nbCells  = nbPointsByEdge * nbPointsByEdge * 2;

    mesh->clear();
    mesh->reserve(nbPoints, nbCells, data::Mesh::CellType::QUAD, attributes);
    Mesh::addQuadMesh(mesh, points, nbPointsByEdge, edgeDim);
    mesh->shrinkToFit();
}

//------------------------------------------------------------------------------

void Mesh::addQuadMesh(const data::Mesh::sptr& mesh, PointsMapType& points, std::size_t nbPointsByEdge, float edgeDim)
{
    const auto dumpLock = mesh->dump_lock();
    std::array<data::Mesh::position_t, 3> pt1 {};
    std::array<data::Mesh::position_t, 3> pt2 {};
    std::array<data::Mesh::position_t, 3> pt3 {};
    std::array<data::Mesh::position_t, 3> pt4 {};
    data::Mesh::size_t idx1 = 0;
    data::Mesh::size_t idx2 = 0;
    data::Mesh::size_t idx3 = 0;
    data::Mesh::size_t idx4 = 0;
    const float step        = edgeDim / static_cast<float>(nbPointsByEdge);

    //Face Y = edgeDim
    for(std::size_t x = 0 ; x < nbPointsByEdge ; x++)
    {
        for(std::size_t z = 0 ; z < nbPointsByEdge ; z++)
        {
            pt1[0] = static_cast<float>(x) * step;
            pt1[1] = edgeDim;
            pt1[2] = static_cast<float>(z) * step;

            pt2[0] = static_cast<float>(x + 1) * step;
            pt2[1] = edgeDim;
            pt2[2] = static_cast<float>(z) * step;

            pt3[0] = static_cast<float>(x) * step;
            pt3[1] = edgeDim;
            pt3[2] = static_cast<float>(z + 1) * step;

            pt4[0] = static_cast<float>(x + 1) * step;
            pt4[1] = edgeDim;
            pt4[2] = static_cast<float>(z + 1) * step;

            idx1 = Mesh::addPoint(pt1.data(), mesh, points);
            idx2 = Mesh::addPoint(pt2.data(), mesh, points);
            idx3 = Mesh::addPoint(pt3.data(), mesh, points);
            idx4 = Mesh::addPoint(pt4.data(), mesh, points);

            const auto cellId = mesh->pushCell(idx1, idx3, idx4, idx2); // NOLINT(readability-suspicious-call-argument)

            if(mesh->has<data::Mesh::Attributes::CELL_COLORS>())
            {
                const auto R = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto G = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto B = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto A = static_cast<std::uint8_t>(safe_rand() % 255);
                mesh->setCellColor(cellId, R, G, B, A);
            }

            if(mesh->has<data::Mesh::Attributes::CELL_NORMALS>())
            {
                const float NX = 0;
                const float NY = 1;
                const float NZ = 0;

                mesh->setCellNormal(cellId, NX, NY, NZ);
            }
        }
    }

    //Face X = edgeDim
    for(std::size_t y = 0 ; y < nbPointsByEdge ; y++)
    {
        for(std::size_t z = 0 ; z < nbPointsByEdge ; z++)
        {
            pt1[0] = edgeDim;
            pt1[1] = static_cast<float>(y) * step;
            pt1[2] = static_cast<float>(z) * step;

            pt2[0] = edgeDim;
            pt2[1] = static_cast<float>(y) * step;
            pt2[2] = static_cast<float>(z + 1) * step;

            pt3[0] = edgeDim;
            pt3[1] = static_cast<float>(y + 1) * step;
            pt3[2] = static_cast<float>(z) * step;

            pt4[0] = edgeDim;
            pt4[1] = static_cast<float>(y + 1) * step;
            pt4[2] = static_cast<float>(z + 1) * step;

            idx1 = Mesh::addPoint(pt1.data(), mesh, points);
            idx2 = Mesh::addPoint(pt2.data(), mesh, points);
            idx3 = Mesh::addPoint(pt3.data(), mesh, points);
            idx4 = Mesh::addPoint(pt4.data(), mesh, points);

            const auto cellId = mesh->pushCell(idx1, idx3, idx4, idx2); // NOLINT(readability-suspicious-call-argument)
            if(mesh->has<data::Mesh::Attributes::CELL_COLORS>())
            {
                const auto R = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto G = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto B = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto A = static_cast<std::uint8_t>(safe_rand() % 255);
                mesh->setCellColor(cellId, R, G, B, A);
            }

            if(mesh->has<data::Mesh::Attributes::CELL_NORMALS>())
            {
                const float NX = 1;
                const float NY = 0;
                const float NZ = 0;

                mesh->setCellNormal(cellId, NX, NY, NZ);
            }
        }
    }
}

//------------------------------------------------------------------------------

void Mesh::addTriangleMesh(
    const data::Mesh::sptr& mesh,
    PointsMapType& points,
    std::size_t nbPointsByEdge,
    float edgeDim
)
{
    const auto dumpLock = mesh->dump_lock();

    std::array<data::Mesh::position_t, 3> pt1 {};
    std::array<data::Mesh::position_t, 3> pt2 {};
    std::array<data::Mesh::position_t, 3> pt3 {};
    std::array<data::Mesh::position_t, 3> pt4 {};
    data::Mesh::size_t idx1 = 0;
    data::Mesh::size_t idx2 = 0;
    data::Mesh::size_t idx3 = 0;
    data::Mesh::size_t idx4 = 0;
    const float step        = edgeDim / static_cast<float>(nbPointsByEdge);

    //Face Z = 0
    for(std::size_t x = 0 ; x < nbPointsByEdge ; x++)
    {
        for(std::size_t y = 0 ; y < nbPointsByEdge ; y++)
        {
            pt1[0] = static_cast<float>(x) * step;
            pt1[1] = static_cast<float>(y) * step;
            pt1[2] = 0;

            pt2[0] = static_cast<float>(x + 1) * step;
            pt2[1] = static_cast<float>(y) * step;
            pt2[2] = 0;

            pt3[0] = static_cast<float>(x) * step;
            pt3[1] = static_cast<float>(y + 1) * step;
            pt3[2] = 0;

            pt4[0] = static_cast<float>(x + 1) * step;
            pt4[1] = static_cast<float>(y + 1) * step;
            pt4[2] = 0;

            idx1 = Mesh::addPoint(pt1.data(), mesh, points);
            idx2 = Mesh::addPoint(pt2.data(), mesh, points);
            idx3 = Mesh::addPoint(pt3.data(), mesh, points);
            idx4 = Mesh::addPoint(pt4.data(), mesh, points);

            const auto cellId1 = mesh->pushCell(idx1, idx4, idx2); // NOLINT(readability-suspicious-call-argument)
            const auto cellId2 = mesh->pushCell(idx1, idx3, idx4); // NOLINT(readability-suspicious-call-argument)

            if(mesh->has<data::Mesh::Attributes::CELL_COLORS>())
            {
                const auto R = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto G = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto B = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto A = static_cast<std::uint8_t>(safe_rand() % 255);
                mesh->setCellColor(cellId1, R, G, B, A);
                mesh->setCellColor(cellId2, A, G, R, B);
            }

            if(mesh->has<data::Mesh::Attributes::CELL_NORMALS>())
            {
                const float NX = 0;
                const float NY = 1;
                const float NZ = 0;

                mesh->setCellNormal(cellId1, NX, NY, NZ);
                mesh->setCellNormal(cellId2, NX, NY, NZ);
            }
        }
    }

    //Face X = 0
    for(std::size_t y = 0 ; y < nbPointsByEdge ; y++)
    {
        for(std::size_t z = 0 ; z < nbPointsByEdge ; z++)
        {
            pt1[0] = 0;
            pt1[1] = static_cast<float>(y) * step;
            pt1[2] = static_cast<float>(z) * step;

            pt2[0] = 0;
            pt2[1] = static_cast<float>(y) * step;
            pt2[2] = static_cast<float>(z + 1) * step;

            pt3[0] = 0;
            pt3[1] = static_cast<float>(y + 1) * step;
            pt3[2] = static_cast<float>(z) * step;

            pt4[0] = 0;
            pt4[1] = static_cast<float>(y + 1) * step;
            pt4[2] = static_cast<float>(z + 1) * step;

            idx1 = Mesh::addPoint(pt1.data(), mesh, points);
            idx2 = Mesh::addPoint(pt2.data(), mesh, points);
            idx3 = Mesh::addPoint(pt3.data(), mesh, points);
            idx4 = Mesh::addPoint(pt4.data(), mesh, points);

            const auto cellId1 = mesh->pushCell(idx2, idx4, idx3); // NOLINT(readability-suspicious-call-argument)
            const auto cellId2 = mesh->pushCell(idx1, idx2, idx3);

            if(mesh->has<data::Mesh::Attributes::CELL_COLORS>())
            {
                const auto R = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto G = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto B = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto A = static_cast<std::uint8_t>(safe_rand() % 255);
                mesh->setCellColor(cellId1, R, G, B, A);
                mesh->setCellColor(cellId2, A, G, R, B);
            }

            if(mesh->has<data::Mesh::Attributes::CELL_NORMALS>())
            {
                const float NX = 1;
                const float NY = 0;
                const float NZ = 0;

                mesh->setCellNormal(cellId1, NX, NY, NZ);
                mesh->setCellNormal(cellId2, NX, NY, NZ);
            }
        }
    }
}

//------------------------------------------------------------------------------

data::Mesh::size_t Mesh::addPoint(
    const data::Mesh::position_t* pt,
    const data::Mesh::sptr& mesh,
    PointsMapType& points
)
{
    RandFloat randFloat;
    std::array<float, 3> myPoint = {pt[0], pt[1], pt[2]};

    auto it = points.find(myPoint);
    if(it != points.end())
    {
        return it->second;
    }

    const data::Mesh::point_t idx = mesh->pushPoint(pt[0], pt[1], pt[2]);
    if(mesh->has<data::Mesh::Attributes::POINT_COLORS>())
    {
        const auto R = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto G = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto B = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto A = static_cast<std::uint8_t>(safe_rand() % 255);
        mesh->setPointColor(idx, R, G, B, A);
    }

    if(mesh->has<data::Mesh::Attributes::POINT_NORMALS>())
    {
        const float NX = randFloat();
        const float NY = randFloat();
        const float NZ = randFloat();

        const float length = std::sqrt(NX * NX + NY * NY + NZ * NZ);
        mesh->setPointNormal(idx, NX / length, NY / length, NZ / length);
    }

    points[myPoint] = idx;
    return idx;
}

//------------------------------------------------------------------------------

void Mesh::shakePoints(const data::Mesh::sptr& mesh)
{
    RandFloat randFloat;
    const auto dumpLock = mesh->dump_lock();

    auto itr          = mesh->begin<data::iterator::point::xyz>();
    const auto itrEnd = mesh->end<data::iterator::point::xyz>();

    for( ; itr != itrEnd ; ++itr)
    {
        itr->x += randFloat() * 5;
        itr->y += randFloat() * 5;
        itr->z += randFloat() * 5;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::utestData::generator
