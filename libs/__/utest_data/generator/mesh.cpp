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

struct rand_float
{
    //------------------------------------------------------------------------------

    float operator()()
    {
        return (static_cast<float>(safe_rand() % 101) - 50.F) / 500.F;
    }
};

//------------------------------------------------------------------------------

void mesh::generate_mesh(const data::mesh::sptr& _mesh)
{
    const data::mesh::attribute attributes = data::mesh::attribute::point_colors
                                             | data::mesh::attribute::point_normals
                                             | data::mesh::attribute::cell_colors
                                             | data::mesh::attribute::cell_normals;

    utest_data::generator::mesh::generate_triangle_mesh(_mesh, attributes);
    utest_data::generator::mesh::shake_points(_mesh);
    _mesh->shrink_to_fit();
}

//------------------------------------------------------------------------------

void mesh::generate_triangle_quad_mesh(
    const data::mesh::sptr& _mesh,
    data::mesh::attribute _attributes
)
{
    data::mesh::size_t nb_points_by_edge = 10;
    float edge_dim                       = 100.;
    mesh::points_map_t points;

    const data::mesh::size_t nb_points         = nb_points_by_edge * nb_points_by_edge * 2 * 4 * 2;
    const data::mesh::size_t nb_triangle_cells = nb_points_by_edge * nb_points_by_edge * 2 * 2;

    _mesh->clear();
    _mesh->reserve(nb_points, nb_triangle_cells, data::mesh::cell_type_t::triangle, _attributes);
    mesh::add_triangle_mesh(_mesh, points, nb_points_by_edge, edge_dim);
}

//------------------------------------------------------------------------------

void mesh::generate_triangle_mesh(
    const data::mesh::sptr& _mesh,
    data::mesh::attribute _attributes
)
{
    data::mesh::size_t nb_points_by_edge = 10;
    float edge_dim                       = 100.;
    mesh::points_map_t points;
    const data::mesh::size_t nb_points = nb_points_by_edge * nb_points_by_edge * 2 * 4;
    const data::mesh::size_t nb_cells  = nb_points_by_edge * nb_points_by_edge * 2 * 2;

    _mesh->clear();
    _mesh->reserve(nb_points, nb_cells, data::mesh::cell_type_t::triangle, _attributes);
    mesh::add_triangle_mesh(_mesh, points, nb_points_by_edge, edge_dim);
    _mesh->shrink_to_fit();
}

//------------------------------------------------------------------------------

void mesh::generate_quad_mesh(
    const data::mesh::sptr& _mesh,
    data::mesh::attribute _attributes
)
{
    data::mesh::size_t nb_points_by_edge = 10;
    float edge_dim                       = 100.;
    mesh::points_map_t points;
    const data::mesh::size_t nb_points = nb_points_by_edge * nb_points_by_edge * 2 * 4;
    const data::mesh::size_t nb_cells  = nb_points_by_edge * nb_points_by_edge * 2;

    _mesh->clear();
    _mesh->reserve(nb_points, nb_cells, data::mesh::cell_type_t::quad, _attributes);
    mesh::add_quad_mesh(_mesh, points, nb_points_by_edge, edge_dim);
    _mesh->shrink_to_fit();
}

//------------------------------------------------------------------------------

void mesh::add_quad_mesh(
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

            idx1 = mesh::add_point(pt1.data(), _mesh, _points);
            idx2 = mesh::add_point(pt2.data(), _mesh, _points);
            idx3 = mesh::add_point(pt3.data(), _mesh, _points);
            idx4 = mesh::add_point(pt4.data(), _mesh, _points);

            const auto cell_id = _mesh->push_cell(idx1, idx3, idx4, idx2); // NOLINT(readability-suspicious-call-argument)

            if(_mesh->has<data::mesh::attribute::cell_colors>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->set_cell_color(cell_id, r, g, b, a);
            }

            if(_mesh->has<data::mesh::attribute::cell_normals>())
            {
                const float nx = 0;
                const float ny = 1;
                const float nz = 0;

                _mesh->set_cell_normal(cell_id, nx, ny, nz);
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

            idx1 = mesh::add_point(pt1.data(), _mesh, _points);
            idx2 = mesh::add_point(pt2.data(), _mesh, _points);
            idx3 = mesh::add_point(pt3.data(), _mesh, _points);
            idx4 = mesh::add_point(pt4.data(), _mesh, _points);

            const auto cell_id = _mesh->push_cell(idx1, idx3, idx4, idx2); // NOLINT(readability-suspicious-call-argument)
            if(_mesh->has<data::mesh::attribute::cell_colors>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->set_cell_color(cell_id, r, g, b, a);
            }

            if(_mesh->has<data::mesh::attribute::cell_normals>())
            {
                const float nx = 1;
                const float ny = 0;
                const float nz = 0;

                _mesh->set_cell_normal(cell_id, nx, ny, nz);
            }
        }
    }
}

//------------------------------------------------------------------------------

void mesh::add_triangle_mesh(
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

            idx1 = mesh::add_point(pt1.data(), _mesh, _points);
            idx2 = mesh::add_point(pt2.data(), _mesh, _points);
            idx3 = mesh::add_point(pt3.data(), _mesh, _points);
            idx4 = mesh::add_point(pt4.data(), _mesh, _points);

            const auto cell_id1 = _mesh->push_cell(idx1, idx4, idx2); // NOLINT(readability-suspicious-call-argument)
            const auto cell_id2 = _mesh->push_cell(idx1, idx3, idx4); // NOLINT(readability-suspicious-call-argument)

            if(_mesh->has<data::mesh::attribute::cell_colors>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->set_cell_color(cell_id1, r, g, b, a);
                _mesh->set_cell_color(cell_id2, a, g, r, b);
            }

            if(_mesh->has<data::mesh::attribute::cell_normals>())
            {
                const float nx = 0;
                const float ny = 1;
                const float nz = 0;

                _mesh->set_cell_normal(cell_id1, nx, ny, nz);
                _mesh->set_cell_normal(cell_id2, nx, ny, nz);
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

            idx1 = mesh::add_point(pt1.data(), _mesh, _points);
            idx2 = mesh::add_point(pt2.data(), _mesh, _points);
            idx3 = mesh::add_point(pt3.data(), _mesh, _points);
            idx4 = mesh::add_point(pt4.data(), _mesh, _points);

            const auto cell_id1 = _mesh->push_cell(idx2, idx4, idx3); // NOLINT(readability-suspicious-call-argument)
            const auto cell_id2 = _mesh->push_cell(idx1, idx2, idx3);

            if(_mesh->has<data::mesh::attribute::cell_colors>())
            {
                const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
                const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
                _mesh->set_cell_color(cell_id1, r, g, b, a);
                _mesh->set_cell_color(cell_id2, a, g, r, b);
            }

            if(_mesh->has<data::mesh::attribute::cell_normals>())
            {
                const float nx = 1;
                const float ny = 0;
                const float nz = 0;

                _mesh->set_cell_normal(cell_id1, nx, ny, nz);
                _mesh->set_cell_normal(cell_id2, nx, ny, nz);
            }
        }
    }
}

//------------------------------------------------------------------------------

data::mesh::size_t mesh::add_point(
    const data::mesh::position_t* _pt,
    const data::mesh::sptr& _mesh,
    points_map_t& _points
)
{
    rand_float rand_float;
    std::array<float, 3> my_point = {_pt[0], _pt[1], _pt[2]};

    auto it = _points.find(my_point);
    if(it != _points.end())
    {
        return it->second;
    }

    const data::mesh::point_t idx = _mesh->push_point(_pt[0], _pt[1], _pt[2]);
    if(_mesh->has<data::mesh::attribute::point_colors>())
    {
        const auto r = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto g = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto b = static_cast<std::uint8_t>(safe_rand() % 255);
        const auto a = static_cast<std::uint8_t>(safe_rand() % 255);
        _mesh->set_point_color(idx, r, g, b, a);
    }

    if(_mesh->has<data::mesh::attribute::point_normals>())
    {
        const float nx = rand_float();
        const float ny = rand_float();
        const float nz = rand_float();

        const float length = std::sqrt(nx * nx + ny * ny + nz * nz);
        _mesh->set_point_normal(idx, nx / length, ny / length, nz / length);
    }

    _points[my_point] = idx;
    return idx;
}

//------------------------------------------------------------------------------

void mesh::shake_points(const data::mesh::sptr& _mesh)
{
    rand_float rand_float;
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
