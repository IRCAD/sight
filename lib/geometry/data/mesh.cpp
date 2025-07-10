/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "geometry/data/mesh.hpp"

#include "data/thread/region_threader.hpp"

#include "geometry/data/matrix4.hpp"

#include <core/com/signal.hxx>
#include <core/tools/random/generator.hpp>

#include <boost/multi_array/multi_array_ref.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstdlib>
#include <ctime>
#include <functional>

namespace sight::geometry::data
{

using core::tools::random::safe_rand;
namespace point = sight::data::iterator::point;
namespace cell  = sight::data::iterator::cell;

struct rand_float
{
    //------------------------------------------------------------------------------

    float operator()()
    {
        return ((static_cast<float>(safe_rand() % 101) - 50.F)) / 500.F;
    }
};

//------------------------------------------------------------------------------

glm::vec3 compute_triangle_normal(const glm::vec3& _p1, const glm::vec3& _p2, const glm::vec3& _p3)
{
    glm::vec3 n(_p2 - _p1);
    glm::vec3 v(_p3 - _p1);
    n = glm::cross(n, v);
    return glm::normalize(n);
}

//------------------------------------------------------------------------------

void generate_region_cell_normals(
    const sight::data::mesh::sptr& _mesh,
    const std::size_t _region_min,
    const std::size_t _region_max
)
{
    switch(_mesh->cell_type())
    {
        case sight::data::mesh::cell_type_t::point:
        case sight::data::mesh::cell_type_t::line:
        {
            auto cell_range = _mesh->range<cell::nxyz>();
            std::fill(cell_range.begin(), cell_range.end(), cell::nxyz({0.F, 0.F, 0.F}));
            break;
        }

        case sight::data::mesh::cell_type_t::triangle:
        {
            const auto point_begin = _mesh->cbegin<point::xyz>();

            auto cell_range     = _mesh->zip_range<cell::triangle, cell::nxyz>();
            auto cell_begin     = cell_range.begin() + std::int64_t(_region_min);
            const auto cell_end = cell_range.begin() + std::int64_t(_region_max);

            int i = 0;
            std::for_each(
                cell_begin,
                cell_end,
                [&](auto&& _it)
                {
                    i++;
                    auto&& [cell, normal] = _it;
                    auto p_itr            = point_begin + cell.pt[0];
                    const glm::vec3 p1(p_itr->x, p_itr->y, p_itr->z);
                    p_itr = point_begin + cell.pt[1];
                    const glm::vec3 p2(p_itr->x, p_itr->y, p_itr->z);
                    p_itr = point_begin + cell.pt[2];
                    const glm::vec3 p3(p_itr->x, p_itr->y, p_itr->z);
                    auto n = compute_triangle_normal(p1, p2, p3);

                    normal.nx = n.x;
                    normal.ny = n.y;
                    normal.nz = n.z;
                });
            break;
        }

        case sight::data::mesh::cell_type_t::quad:
        case sight::data::mesh::cell_type_t::tetra:
        {
            const auto point_begin = _mesh->cbegin<point::xyz>();

            auto cell_range     = _mesh->zip_range<cell::quad, cell::nxyz>();
            auto cell_begin     = cell_range.begin() + std::int64_t(_region_min);
            const auto cell_end = cell_range.begin() + std::int64_t(_region_max);

            std::for_each(
                cell_begin,
                cell_end,
                [&](auto&& _it)
                {
                    glm::vec3 n(0.F);
                    auto&& [cell, normal] = _it;

                    for(std::size_t i = 0 ; i < 4 ; ++i)
                    {
                        auto p_itr = point_begin + cell.pt[i];
                        const glm::vec3 p1(p_itr->x, p_itr->y, p_itr->z);
                        p_itr = point_begin + cell.pt[(i + 1) % 4];
                        const glm::vec3 p2(p_itr->x, p_itr->y, p_itr->z);
                        p_itr = point_begin + cell.pt[(i + 2) % 4];
                        const glm::vec3 p3(p_itr->x, p_itr->y, p_itr->z);

                        n += compute_triangle_normal(p1, p2, p3);
                    }

                    n        /= 4.F;
                    n         = glm::normalize(n);
                    normal.nx = n.x;
                    normal.ny = n.y;
                    normal.nz = n.z;
                });
            break;
        }

        default:
            SIGHT_ASSERT("_SIZE is an invalid cell type", false);
    }
}

//------------------------------------------------------------------------------

template<typename T>
void vector_sum(std::vector<std::vector<T> >& _vectors, std::size_t _region_min, std::size_t _region_max)
{
    if(_vectors.empty())
    {
        return;
    }

    auto v_iter = _vectors.begin();

    std::vector<T>& res = _vectors[0];

    for(++v_iter ; v_iter != _vectors.end() ; ++v_iter)
    {
        for(std::size_t i = _region_min ; i < _region_max ; ++i)
        {
            res[i] += (*v_iter)[i];
        }
    }
}

//------------------------------------------------------------------------------

void mesh::generate_cell_normals(sight::data::mesh::sptr _mesh)
{
    const sight::data::mesh::size_t number_of_cells = _mesh->num_cells();
    if(number_of_cells > 0)
    {
        if(!_mesh->has<sight::data::mesh::attribute::cell_normals>())
        {
            _mesh->resize(
                _mesh->num_points(),
                _mesh->num_cells(),
                _mesh->cell_type(),
                sight::data::mesh::attribute::cell_normals
            );
        }

        const auto dump_lock = _mesh->dump_lock();

        sight::data::thread::region_threader rt((number_of_cells >= 200000) ? 4 : 1);
        rt(
            [_mesh](std::size_t _p_h1, std::ptrdiff_t _p_h2, auto&& ...)
            {
                generate_region_cell_normals(_mesh, _p_h1, std::size_t(_p_h2));
            },
            number_of_cells
        );
    }
}

//------------------------------------------------------------------------------

using float_vectors_t = std::vector<std::vector<float> >;

//------------------------------------------------------------------------------

void generate_region_cell_normals_by_points(
    float_vectors_t& _normals_data,
    std::size_t _data_id,
    const sight::data::mesh::sptr& _mesh,
    const std::size_t _region_min,
    const std::size_t _region_max
)
{
    float_vectors_t::value_type& normals_results = _normals_data[_data_id];

    const sight::data::mesh::size_t nb_of_points = _mesh->num_points();
    normals_results.resize(std::size_t(3) * nb_of_points, 0.F);

    auto accum_normal = [&](const auto& _cell, const auto& _normal)
                        {
                            _normals_data[_data_id][std::size_t(3) * _cell]     += _normal.nx;
                            _normals_data[_data_id][std::size_t(3) * _cell + 1] += _normal.ny;
                            _normals_data[_data_id][std::size_t(3) * _cell + 2] += _normal.nz;
                        };

    switch(_mesh->cell_type())
    {
        case sight::data::mesh::cell_type_t::point:
            break;

        case sight::data::mesh::cell_type_t::line:
        {
            const auto range = _mesh->czip_range<cell::line, cell::nxyz>();
            auto begin       = range.begin() + std::int64_t(_region_min);
            const auto end   = range.begin() + std::int64_t(_region_max);
            std::for_each(
                begin,
                end,
                [&](const auto& _it)
                {
                    const auto& [line, n] = _it;
                    accum_normal(line.pt[0], n);
                    accum_normal(line.pt[1], n);
                });
            break;
        }

        case sight::data::mesh::cell_type_t::triangle:
        {
            const auto range = _mesh->czip_range<cell::triangle, cell::nxyz>();
            auto begin       = range.begin() + std::int64_t(_region_min);
            const auto end   = range.begin() + std::int64_t(_region_max);
            std::for_each(
                begin,
                end,
                [&](const auto& _it)
                {
                    const auto& [cell, n] = _it;
                    accum_normal(cell.pt[0], n);
                    accum_normal(cell.pt[1], n);
                    accum_normal(cell.pt[2], n);
                });
            break;
        }

        case sight::data::mesh::cell_type_t::quad:
        case sight::data::mesh::cell_type_t::tetra:
        {
            const auto range = _mesh->czip_range<cell::quad, cell::nxyz>();
            auto begin       = range.begin() + std::int64_t(_region_min);
            const auto end   = range.begin() + std::int64_t(_region_max);
            std::for_each(
                begin,
                end,
                [&](const auto& _it)
                {
                    const auto& [cell, n] = _it;
                    accum_normal(cell.pt[0], n);
                    accum_normal(cell.pt[1], n);
                    accum_normal(cell.pt[2], n);
                    accum_normal(cell.pt[3], n);
                });
            break;
        }

        default:
            SIGHT_ASSERT("_SIZE is an invalid cell type", false);
    }
}

//------------------------------------------------------------------------------

void normalize_region_cell_normals_by_points(
    float_vectors_t::value_type& _normals_data,
    sight::data::mesh::sptr _mesh,
    const std::size_t _region_min,
    const std::size_t _region_max
)
{
    float* normal_sum = reinterpret_cast<float*>(&(*_normals_data.begin()));

    auto point_itr = _mesh->begin<point::nxyz>() + std::int64_t(_region_min);

    for(size_t i = _region_min ; i < _region_max ; i++, ++point_itr)
    {
        glm::vec3 normal = glm::make_vec3<float>(&normal_sum[i * 3]);

        normal        = glm::normalize(normal);
        point_itr->nx = normal.x;
        point_itr->ny = normal.y;
        point_itr->nz = normal.z;
    }
}

//------------------------------------------------------------------------------

void mesh::generate_point_normals(sight::data::mesh::sptr _mesh)
{
    const sight::data::mesh::size_t nb_of_points = _mesh->num_points();
    if(nb_of_points > 0)
    {
        const sight::data::mesh::size_t number_of_cells = _mesh->num_cells();

        // To generate point normals, we need to use the cell normals
        if(!_mesh->has<sight::data::mesh::attribute::cell_normals>())
        {
            generate_cell_normals(_mesh);
        }

        if(!_mesh->has<sight::data::mesh::attribute::point_normals>())
        {
            _mesh->resize(
                _mesh->num_points(),
                _mesh->num_cells(),
                _mesh->cell_type(),
                sight::data::mesh::attribute::point_normals
            );
        }

        const auto dump_lock = _mesh->dump_lock();

        sight::data::thread::region_threader rt((nb_of_points >= 100000) ? 4 : 1);

        float_vectors_t normals_data(rt.number_of_thread());

        rt(
            [&normals_data, _mesh](std::size_t _p_h1, std::ptrdiff_t _p_h2, std::size_t _p_h3, auto&& ...)
            {
                generate_region_cell_normals_by_points(
                    normals_data,
                    _p_h3,
                    _mesh,
                    _p_h1,
                    std::size_t(_p_h2)
                );
            },
            number_of_cells
        );

        rt(
            [&normals_data](std::size_t _p_h1, std::ptrdiff_t _p_h2, auto&& ...)
            {
                vector_sum<float_vectors_t::value_type::value_type>(
                    normals_data,
                    _p_h1,
                    std::size_t(_p_h2)
                );
            },
            nb_of_points * std::ptrdiff_t(3)
        );

        rt(
            [&capture0 = normals_data[0], _mesh](std::size_t _p_h1, std::ptrdiff_t _p_h2, auto&& ...)
            {
                normalize_region_cell_normals_by_points(
                    capture0,
                    _mesh,
                    _p_h1,
                    std::size_t(_p_h2)
                );
            },
            nb_of_points
        );
    }
}

//------------------------------------------------------------------------------

template<typename T>
void region_shake_normals(T _normals, const std::size_t _region_min, const std::size_t _region_max)
{
    rand_float rand_float;
    for(std::size_t i = _region_min ; i < _region_max ; ++i)
    {
        glm::vec3 v(rand_float(), rand_float(), rand_float());
        _normals[std::int64_t(i)] += v;
        _normals[std::int64_t(i)]  = glm::normalize(_normals[std::int64_t(i)]);
    }
}

//------------------------------------------------------------------------------

void mesh::shake_point_normals(sight::data::mesh::sptr _mesh)
{
    const auto dump_lock = _mesh->dump_lock();

    rand_float rand_float;

    for(auto& n : _mesh->range<point::nxyz>())
    {
        glm::vec3 v(rand_float(), rand_float(), rand_float());
        glm::vec3 normal(n.nx, n.ny, n.nz);
        normal += v;
        normal  = glm::normalize(normal);
        n.nx    = normal.x;
        n.ny    = normal.y;
        n.nz    = normal.z;
    }
}

//------------------------------------------------------------------------------

void mesh::shake_cell_normals(sight::data::mesh::sptr _mesh)
{
    const auto dump_lock = _mesh->dump_lock();

    rand_float rand_float;

    for(auto& n : _mesh->range<cell::nxyz>())
    {
        glm::vec3 v(rand_float(), rand_float(), rand_float());
        glm::vec3 normal(n.nx, n.ny, n.nz);
        normal += v;
        normal  = glm::normalize(normal);
        n.nx    = normal.x;
        n.ny    = normal.y;
        n.nz    = normal.z;
    }
}

//------------------------------------------------------------------------------

void mesh::colorize_mesh_points(sight::data::mesh::sptr _mesh)
{
    if(!_mesh->has<sight::data::mesh::attribute::point_colors>())
    {
        _mesh->resize(
            _mesh->num_points(),
            _mesh->num_cells(),
            _mesh->cell_type(),
            sight::data::mesh::attribute::point_colors
        );
    }

    const auto dump_lock = _mesh->dump_lock();

    for(auto& c : _mesh->range<point::rgba>())
    {
        c.r = static_cast<std::uint8_t>(safe_rand() % 256);
        c.g = static_cast<std::uint8_t>(safe_rand() % 256);
        c.b = static_cast<std::uint8_t>(safe_rand() % 256);
        c.a = static_cast<std::uint8_t>(safe_rand() % 256);
    }
}

//------------------------------------------------------------------------------

void mesh::colorize_mesh_cells(sight::data::mesh::sptr _mesh)
{
    if(!_mesh->has<sight::data::mesh::attribute::cell_colors>())
    {
        _mesh->resize(
            _mesh->num_points(),
            _mesh->num_cells(),
            _mesh->cell_type(),
            sight::data::mesh::attribute::cell_colors
        );
    }

    const auto dump_lock = _mesh->dump_lock();

    for(auto& c : _mesh->range<cell::rgba>())
    {
        c.r = static_cast<std::uint8_t>(safe_rand() % 256);
        c.g = static_cast<std::uint8_t>(safe_rand() % 256);
        c.b = static_cast<std::uint8_t>(safe_rand() % 256);
        c.a = static_cast<std::uint8_t>(safe_rand() % 256);
    }
}

//------------------------------------------------------------------------------

void mesh::shake_point(sight::data::mesh::sptr _mesh)
{
    rand_float rand_float;

    const auto dump_lock = _mesh->dump_lock();

    for(auto& p : _mesh->range<point::xyz>())
    {
        p.x += rand_float() * 5;
        p.y += rand_float() * 5;
        p.z += rand_float() * 5;
    }
}

//------------------------------------------------------------------------------

void mesh::transform(
    sight::data::mesh::csptr _in_mesh,
    sight::data::mesh::sptr _out_mesh,
    const sight::data::matrix4& _t
)
{
    const auto in_dump_lock  = _in_mesh->dump_lock();
    const auto out_dump_lock = _out_mesh->dump_lock();

    const glm::dmat4x4 matrix = sight::geometry::data::to_glm_mat(_t);

    [[maybe_unused]] const std::size_t num_pts = _in_mesh->num_points();
    SIGHT_ASSERT("In and out meshes should have the same number of points", num_pts == _out_mesh->num_points());

    SIGHT_ASSERT(
        "in and out meshes must have the same point normals attribute",
        (_in_mesh->has<sight::data::mesh::attribute::point_normals>()
         && _out_mesh->has<sight::data::mesh::attribute::point_normals>())
        || (!_in_mesh->has<sight::data::mesh::attribute::point_normals>()
            && !_out_mesh->has<sight::data::mesh::attribute::point_normals>())
    );

    for(auto&& [in, out] : boost::combine(_in_mesh->crange<point::xyz>(), _out_mesh->range<point::xyz>()))
    {
        const glm::vec4 pt(in.x, in.y, in.z, 1.);
        const glm::vec4 transformed_pt = matrix * pt;

        out.x = transformed_pt.x;
        out.y = transformed_pt.y;
        out.z = transformed_pt.z;
    }

    if(_in_mesh->has<sight::data::mesh::attribute::point_normals>())
    {
        for(auto&& [in, out] : boost::combine(_in_mesh->crange<point::nxyz>(), _out_mesh->range<point::nxyz>()))
        {
            const glm::vec4 normal(in.nx, in.ny, in.nz, 0.);
            const glm::vec4 transformed_normal = glm::normalize(matrix * normal);

            out.nx = transformed_normal.x;
            out.ny = transformed_normal.y;
            out.nz = transformed_normal.z;
        }
    }

    if(_in_mesh->has<sight::data::mesh::attribute::cell_normals>())
    {
        SIGHT_ASSERT("out mesh must have normals", _out_mesh->has<sight::data::mesh::attribute::cell_normals>());
        for(auto&& [in, out] : boost::combine(_in_mesh->crange<cell::nxyz>(), _out_mesh->range<cell::nxyz>()))
        {
            const glm::vec4 normal(in.nx, in.ny, in.nz, 0.);
            const glm::vec4 transformed_normal = glm::normalize(matrix * normal);

            out.nx = transformed_normal.x;
            out.ny = transformed_normal.y;
            out.nz = transformed_normal.z;
        }
    }
}

//------------------------------------------------------------------------------

void mesh::transform(sight::data::mesh::sptr _mesh, const sight::data::matrix4& _t)
{
    mesh::transform(_mesh, _mesh, _t);
}

//------------------------------------------------------------------------------

void mesh::colorize_mesh_points(
    const sight::data::mesh::sptr& _mesh,
    const std::uint8_t _color_r,
    const std::uint8_t _color_g,
    const std::uint8_t _color_b,
    const std::uint8_t _color_a
)
{
    const auto dump_lock = _mesh->dump_lock();

    SIGHT_ASSERT("color array must be allocated", _mesh->has<sight::data::mesh::attribute::point_colors>());

    for(auto& c : _mesh->range<point::rgba>())
    {
        c.r = _color_r;
        c.g = _color_g;
        c.b = _color_b;
        c.a = _color_a;
    }

    auto sig = _mesh->signal<sight::data::mesh::signal_t>(sight::data::mesh::POINT_COLORS_MODIFIED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void mesh::colorize_mesh_points(
    const sight::data::mesh::sptr& _mesh,
    const std::vector<std::size_t>& _vector_num_triangle,
    const std::uint8_t _color_r,
    const std::uint8_t _color_g,
    const std::uint8_t _color_b,
    const std::uint8_t _color_a
)
{
    const auto dump_lock = _mesh->dump_lock();

    auto itr_cell = _mesh->begin<cell::triangle>();

    auto itr_point = _mesh->begin<point::rgba>();

    for(std::size_t index : _vector_num_triangle)
    {
        auto cell               = itr_cell + static_cast<std::ptrdiff_t>(index);
        const auto index_point0 = static_cast<std::ptrdiff_t>(cell->pt[0]);
        const auto index_point1 = static_cast<std::ptrdiff_t>(cell->pt[1]);
        const auto index_point2 = static_cast<std::ptrdiff_t>(cell->pt[2]);

        auto point1 = itr_point + index_point0;
        auto point2 = itr_point + index_point1;
        auto point3 = itr_point + index_point2;

        point1->r = _color_r;
        point1->g = _color_g;
        point1->b = _color_b;
        point1->a = _color_a;

        point2->r = _color_r;
        point2->g = _color_g;
        point2->b = _color_b;
        point2->a = _color_a;

        point3->r = _color_r;
        point3->g = _color_g;
        point3->b = _color_b;
        point3->a = _color_a;
    }

    auto sig = _mesh->signal<sight::data::mesh::signal_t>(sight::data::mesh::POINT_COLORS_MODIFIED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void mesh::colorize_mesh_cells(
    const sight::data::mesh::sptr& _mesh,
    const std::uint8_t _color_r,
    const std::uint8_t _color_g,
    const std::uint8_t _color_b,
    const std::uint8_t _color_a
)
{
    const auto dump_lock = _mesh->dump_lock();

    SIGHT_ASSERT("color array must be allocated", _mesh->has<sight::data::mesh::attribute::cell_colors>());

    for(auto& c : _mesh->range<cell::rgba>())
    {
        c.r = _color_r;
        c.g = _color_g;
        c.b = _color_b;
        c.a = _color_a;
    }

    auto sig = _mesh->signal<sight::data::mesh::signal_t>(sight::data::mesh::CELL_COLORS_MODIFIED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void mesh::colorize_mesh_cells(
    const sight::data::mesh::sptr& _mesh,
    const std::vector<std::size_t>& _triangle_index_vector,
    const std::uint8_t _color_r,
    const std::uint8_t _color_g,
    const std::uint8_t _color_b,
    const std::uint8_t _color_a
)
{
    const auto dump_lock = _mesh->dump_lock();

    auto itr_cell = _mesh->begin<cell::rgba>();

    for(std::size_t index : _triangle_index_vector)
    {
        auto cell = itr_cell + static_cast<std::ptrdiff_t>(index);

        cell->r = _color_r;
        cell->g = _color_g;
        cell->b = _color_b;
        cell->a = _color_a;
    }

    auto sig = _mesh->signal<sight::data::mesh::signal_t>(sight::data::mesh::CELL_COLORS_MODIFIED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

template<typename T, typename U>
std::pair<T, U> make_ordered_pair(const T _first, const U _second)
{
    if(_first < _second)
    {
        return std::pair<T, U>(_first, _second);
    }

    return std::pair<T, U>(_second, _first);
}

//------------------------------------------------------------------------------

bool mesh::is_closed(const sight::data::mesh::csptr& _mesh)
{
    bool is_closed = true;

    using edge_t           = std::pair<sight::data::mesh::cell_t, sight::data::mesh::cell_t>;
    using edge_histogram_t = std::map<edge_t, int>;
    edge_histogram_t edges_histogram;

    const auto dump_lock = _mesh->dump_lock();

    auto add_edge = [&edges_histogram](const sight::data::mesh::point_t& _p1, const sight::data::mesh::point_t& _p2)
                    {
                        const auto edge = make_ordered_pair(_p1, _p2);

                        if(edges_histogram.find(edge) == edges_histogram.end())
                        {
                            edges_histogram[edge] = 1;
                        }
                        else
                        {
                            ++edges_histogram[edge];
                        }
                    };

    switch(_mesh->cell_type())
    {
        case sight::data::mesh::cell_type_t::point:
            break;

        case sight::data::mesh::cell_type_t::line:
        {
            for(const auto& line : _mesh->crange<cell::line>())
            {
                add_edge(line.pt[0], line.pt[1]);
            }

            break;
        }

        case sight::data::mesh::cell_type_t::triangle:
        {
            for(const auto& cell : _mesh->crange<cell::triangle>())
            {
                add_edge(cell.pt[0], cell.pt[1]);
                add_edge(cell.pt[1], cell.pt[2]);
                add_edge(cell.pt[2], cell.pt[0]);
            }

            break;
        }

        case sight::data::mesh::cell_type_t::quad:
        case sight::data::mesh::cell_type_t::tetra:
        {
            for(const auto& cell : _mesh->crange<cell::quad>())
            {
                add_edge(cell.pt[0], cell.pt[1]);
                add_edge(cell.pt[1], cell.pt[2]);
                add_edge(cell.pt[2], cell.pt[3]);
                add_edge(cell.pt[3], cell.pt[0]);
            }

            break;
        }

        default:
            SIGHT_ASSERT("_SIZE is an invalid cell type", false);
    }

    for(const auto& h : edges_histogram)
    {
        if(h.second != 2)
        {
            is_closed = false;
            break;
        }
    }

    return is_closed;
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::data
