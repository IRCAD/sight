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

#include "utest_data/generator/mesh.hpp"

#include <core/tools/random/generator.hpp>

#include <cstdlib>
#include <ctime>

namespace sight::utest_data::generator
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

void mesh::generateMesh(const data::mesh::sptr& _mesh)
{
    const data::mesh::Attributes attributes = data::mesh::Attributes::POINT_COLORS
                                              | data::mesh::Attributes::POINT_NORMALS
                                              | data::mesh::Attributes::CELL_COLORS
                                              | data::mesh::Attributes::CELL_NORMALS;

    utest_data::generator::mesh::generateTriangleMesh(_mesh, attributes);
    utest_data::generator::mesh::shakePoints(_mesh);
    _mesh->shrinkToFit();
}

//------------------------------------------------------------------------------

void mesh::generateTriangleQuadMesh(
    const data::mesh::sptr& _mesh,
    data::mesh::Attributes _attributes
)
{
    data::mesh::size_t nb_points_by_edge = 10;
    float edge_dim                       = 100.;
    mesh::points_map_t points;

    const data::mesh::size_t nb_points         = nb_points_by_edge * nb_points_by_edge * 2 * 4 * 2;
    const data::mesh::size_t nb_triangle_cells = nb_points_by_edge * nb_points_by_edge * 2 * 2;

    _mesh->clear();
    _mesh->reserve(nb_points, nb_triangle_cells, data::mesh::cell_type_t::TRIANGLE, _attributes);
    mesh::addTriangleMesh(_mesh, points, nb_points_by_edge, edge_dim);
}

//------------------------------------------------------------------------------

void mesh::generateTriangleMesh(
    const data::mesh::sptr& _mesh,
    data::mesh::Attributes _attributes
)
{
    data::mesh::size_t nb_points_by_edge = 10;
    float edge_dim                       = 100.;
    mesh::points_map_t points;
    const data::mesh::size_t nb_points = nb_points_by_edge * nb_points_by_edge * 2 * 4;
    const data::mesh::size_t nb_cells  = nb_points_by_edge * nb_points_by_edge * 2 * 2;

    _mesh->clear();
    _mesh->reserve(nb_points, nb_cells, data::mesh::cell_type_t::TRIANGLE, _attributes);
    mesh::addTriangleMesh(_mesh, points, nb_points_by_edge, edge_dim);
    _mesh->shrinkToFit();
}

//------------------------------------------------------------------------------

void mesh::generateQuadMesh(
    const data::mesh::sptr& _mesh,
    data::mesh::Attributes _attributes
)
{
    data::mesh::size_t nb_points_by_edge = 10;
    float edge_dim                       = 100.;
    mesh::points_map_t points;
    const data::mesh::size_t nb_points = nb_points_by_edge * nb_points_by_edge * 2 * 4;
    const data::mesh::size_t nb_cells  = nb_points_by_edge * nb_points_by_edge * 2;

    _mesh->clear();
    _mesh->reserve(nb_points, nb_cells, data::mesh::cell_type_t::QUAD, _attributes);
    mesh::addQuadMesh(_mesh, points, nb_points_by_edge, edge_dim);
    _mesh->shrinkToFit();
}

//------------------------------------------------------------------------------

void mesh::addQuadMesh(
    const data::mesh::sptr& _mesh,
    points_map_t& _points,
    std::size_t _nb_points_by_edge,
    float _edge_dim
)
{
    const auto dump_lock = _mesh->dump_lock();
    std::array<data::mesh::position_t, 3> pt1 {};
    std::array<data::mesh::position_t, 3> pt2 {};
    std::array<data::mesh::position_t, 3> pt3 {};
    std::array<data::mesh::position_t, 3> pt4 {};
    data::mesh::size_t idx1 = 0;
    data::mesh::size_t idx2 = 0;
    data::mesh::size_t idx3 = 0;
    data::mesh::size_t idx4 = 0;
    const float step        = _edge_dim / static_cast<float>(_nb_points_by_edge);

    //Face Y = edgeDim
    for(std::size_t x = 0 ; x < _nb_points_by_edge ; x++)
    {
        for(std::size_t z = 0 ; z < _nb_points_by_edge ; z++)
        {
            pt1[0] = static_cast<float>(x) * step;
            pt1[1] = _edge_dim;
            pt1[2] = static_cast<float>(z) * step;

            pt2[0] = static_cast<float>(x + 1) * step;
            pt2[1] = _edge_dim;
            pt2[2] = static_cast<float>(z) * step;

            pt3[0] = static_cast<float>(x) * step;
            pt3[1] = _edge_dim;
            pt3[2] = static_cast<float>(z + 1) * step;

            pt4[0] = static_cast<float>(x + 1) * step;
            pt4[1] = _edge_dim;
            pt4[2] = static_cast<float>(z + 1) * step;

            idx1 = mesh::addPoint(pt1.data(), _mesh, _points);
            idx2 = mesh::addPoint(pt2.data(), _mesh, _points);
            idx3 = mesh::addPoint(pt3.data(), _mesh, _points);
            idx4 = mesh::addPoint(pt4.data(), _mesh, _points);

            const auto cell_id = _mesh->pushCell(idx1, idx3, idx4, idx2); // NOLINT(readability-suspicious-call-argument)

            if(_mesh->has<data::mesh::Attributes::CELL_COLORS>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->setCellColor(cell_id, r, g, b, a);
            }

            if(_mesh->has<data::mesh::Attributes::CELL_NORMALS>())
            {
                const float nx = 0;
                const float ny = 1;
                const float nz = 0;

                _mesh->setCellNormal(cell_id, nx, ny, nz);
            }
        }
    }

    //Face X = edgeDim
    for(std::size_t y = 0 ; y < _nb_points_by_edge ; y++)
    {
        for(std::size_t z = 0 ; z < _nb_points_by_edge ; z++)
        {
            pt1[0] = _edge_dim;
            pt1[1] = static_cast<float>(y) * step;
            pt1[2] = static_cast<float>(z) * step;

            pt2[0] = _edge_dim;
            pt2[1] = static_cast<float>(y) * step;
            pt2[2] = static_cast<float>(z + 1) * step;

            pt3[0] = _edge_dim;
            pt3[1] = static_cast<float>(y + 1) * step;
            pt3[2] = static_cast<float>(z) * step;

            pt4[0] = _edge_dim;
            pt4[1] = static_cast<float>(y + 1) * step;
            pt4[2] = static_cast<float>(z + 1) * step;

            idx1 = mesh::addPoint(pt1.data(), _mesh, _points);
            idx2 = mesh::addPoint(pt2.data(), _mesh, _points);
            idx3 = mesh::addPoint(pt3.data(), _mesh, _points);
            idx4 = mesh::addPoint(pt4.data(), _mesh, _points);

            const auto cell_id = _mesh->pushCell(idx1, idx3, idx4, idx2); // NOLINT(readability-suspicious-call-argument)
            if(_mesh->has<data::mesh::Attributes::CELL_COLORS>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->setCellColor(cell_id, r, g, b, a);
            }

            if(_mesh->has<data::mesh::Attributes::CELL_NORMALS>())
            {
                const float nx = 1;
                const float ny = 0;
                const float nz = 0;

                _mesh->setCellNormal(cell_id, nx, ny, nz);
            }
        }
    }
}

//------------------------------------------------------------------------------

void mesh::addTriangleMesh(
    const data::mesh::sptr& _mesh,
    points_map_t& _points,
    std::size_t _nb_points_by_edge,
    float _edge_dim
)
{
    const auto dump_lock = _mesh->dump_lock();

    std::array<data::mesh::position_t, 3> pt1 {};
    std::array<data::mesh::position_t, 3> pt2 {};
    std::array<data::mesh::position_t, 3> pt3 {};
    std::array<data::mesh::position_t, 3> pt4 {};
    data::mesh::size_t idx1 = 0;
    data::mesh::size_t idx2 = 0;
    data::mesh::size_t idx3 = 0;
    data::mesh::size_t idx4 = 0;
    const float step        = _edge_dim / static_cast<float>(_nb_points_by_edge);

    //Face Z = 0
    for(std::size_t x = 0 ; x < _nb_points_by_edge ; x++)
    {
        for(std::size_t y = 0 ; y < _nb_points_by_edge ; y++)
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

            idx1 = mesh::addPoint(pt1.data(), _mesh, _points);
            idx2 = mesh::addPoint(pt2.data(), _mesh, _points);
            idx3 = mesh::addPoint(pt3.data(), _mesh, _points);
            idx4 = mesh::addPoint(pt4.data(), _mesh, _points);

            const auto cell_id1 = _mesh->pushCell(idx1, idx4, idx2); // NOLINT(readability-suspicious-call-argument)
            const auto cell_id2 = _mesh->pushCell(idx1, idx3, idx4); // NOLINT(readability-suspicious-call-argument)

            if(_mesh->has<data::mesh::Attributes::CELL_COLORS>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->setCellColor(cell_id1, r, g, b, a);
                _mesh->setCellColor(cell_id2, a, g, r, b);
            }

            if(_mesh->has<data::mesh::Attributes::CELL_NORMALS>())
            {
                const float nx = 0;
                const float ny = 1;
                const float nz = 0;

                _mesh->setCellNormal(cell_id1, nx, ny, nz);
                _mesh->setCellNormal(cell_id2, nx, ny, nz);
            }
        }
    }

    //Face X = 0
    for(std::size_t y = 0 ; y < _nb_points_by_edge ; y++)
    {
        for(std::size_t z = 0 ; z < _nb_points_by_edge ; z++)
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

            idx1 = mesh::addPoint(pt1.data(), _mesh, _points);
            idx2 = mesh::addPoint(pt2.data(), _mesh, _points);
            idx3 = mesh::addPoint(pt3.data(), _mesh, _points);
            idx4 = mesh::addPoint(pt4.data(), _mesh, _points);

            const auto cell_id1 = _mesh->pushCell(idx2, idx4, idx3); // NOLINT(readability-suspicious-call-argument)
            const auto cell_id2 = _mesh->pushCell(idx1, idx2, idx3);

            if(_mesh->has<data::mesh::Attributes::CELL_COLORS>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->setCellColor(cell_id1, r, g, b, a);
                _mesh->setCellColor(cell_id2, a, g, r, b);
            }

            if(_mesh->has<data::mesh::Attributes::CELL_NORMALS>())
            {
                const float nx = 1;
                const float ny = 0;
                const float nz = 0;

                _mesh->setCellNormal(cell_id1, nx, ny, nz);
                _mesh->setCellNormal(cell_id2, nx, ny, nz);
            }
        }
    }
}

//------------------------------------------------------------------------------

data::mesh::size_t mesh::addPoint(
    const data::mesh::position_t* _pt,
    const data::mesh::sptr& _mesh,
    points_map_t& _points
)
{
    RandFloat rand_float;
    std::array<float, 3> my_point = {_pt[0], _pt[1], _pt[2]};

    auto it = _points.find(my_point);
    if(it != _points.end())
    {
        return it->second;
    }

    const data::mesh::point_t idx = _mesh->pushPoint(_pt[0], _pt[1], _pt[2]);
    if(_mesh->has<data::mesh::Attributes::POINT_COLORS>())
    {
        const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
        _mesh->setPointColor(idx, r, g, b, a);
    }

    if(_mesh->has<data::mesh::Attributes::POINT_NORMALS>())
    {
        const float nx = rand_float();
        const float ny = rand_float();
        const float nz = rand_float();

        const float length = std::sqrt(nx * nx + ny * ny + nz * nz);
        _mesh->setPointNormal(idx, nx / length, ny / length, nz / length);
    }

    _points[my_point] = idx;
    return idx;
}

//------------------------------------------------------------------------------

void mesh::shakePoints(const data::mesh::sptr& _mesh)
{
    RandFloat rand_float;
    const auto dump_lock = _mesh->dump_lock();

    auto itr           = _mesh->begin<data::iterator::point::xyz>();
    const auto itr_end = _mesh->end<data::iterator::point::xyz>();

    for( ; itr != itr_end ; ++itr)
    {
        itr->x += rand_float() * 5;
        itr->y += rand_float() * 5;
        itr->z += rand_float() * 5;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::utest_data::generator
