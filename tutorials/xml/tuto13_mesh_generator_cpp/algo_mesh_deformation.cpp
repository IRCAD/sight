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

#include "algo_mesh_deformation.hpp"

#include <core/tools/numeric_round_cast.hxx>

#include <geometry/data/mesh.hpp>

#include <cmath>

namespace data     = sight::data;
namespace geometry = sight::geometry;

namespace tuto13_mesh_generator_cpp
{

//-----------------------------------------------------------------------------

algo_mesh_deformation::algo_mesh_deformation() noexcept =
    default;

//-----------------------------------------------------------------------------

algo_mesh_deformation::~algo_mesh_deformation() noexcept =
    default;

//-----------------------------------------------------------------------------

void algo_mesh_deformation::set_param(
    data::mesh::sptr _mesh,
    const unsigned int _nb_step,
    const unsigned int _amplitude
)
{
    m_mesh      = _mesh;
    m_nb_step   = _nb_step;
    m_amplitude = _amplitude;
    m_direction = 1;

    m_nb_points = _mesh->num_points();
    m_nb_cells  = _mesh->num_cells();
}

//-----------------------------------------------------------------------------

void algo_mesh_deformation::compute_deformation(
    data::mesh::sptr _mesh,
    const unsigned int _nb_step,
    const unsigned int _amplitude
)
{
    if(m_mesh.expired()
       || m_nb_points != _mesh->num_points()
       || m_nb_cells != _mesh->num_cells()
       || !_mesh->has<data::mesh::attribute::point_colors>())
    {
        this->set_param(_mesh, _nb_step, _amplitude);
        this->init_simu();
    }
    else
    {
        this->compute_simu();
    }
}

//-----------------------------------------------------------------------------

void algo_mesh_deformation::init_simu()
{
    const auto mesh = m_mesh.lock();
    m_origin_mesh = data::object::copy(mesh);
    m_step        = 0;

    if(!m_mesh.lock()->has<data::mesh::attribute::point_colors>())
    {
        geometry::data::mesh::colorize_mesh_points(mesh);
    }

    const auto dump_lock = mesh->dump_lock();

    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();
    for(const auto& p : mesh->crange<data::iterator::point::xyz>())
    {
        const float coord = p.y;

        if(coord < min)
        {
            min = coord;
        }

        if(coord > max)
        {
            max = coord;
        }
    }

    m_y_center = (max - min) / 2 + min;
}

//-----------------------------------------------------------------------------

void algo_mesh_deformation::compute_simu()
{
    namespace core  = sight::core;
    namespace point = data::iterator::point;

    m_step += m_direction;
    if(m_step == static_cast<int>(m_nb_step))
    {
        m_direction = -1;
    }
    else if(m_step == 0)
    {
        m_direction = 1;
    }

    const float scale = static_cast<float>(m_step) / static_cast<float>(m_nb_step);

    const auto mesh           = m_mesh.lock();
    const auto dump_lock      = mesh->dump_lock();
    const auto orig_dump_lock = m_origin_mesh->dump_lock();

    const auto orig_range = m_origin_mesh->czip_range<point::xyz, point::rgba>();
    const auto range      = mesh->zip_range<point::xyz, point::rgba>();

    for(const auto& [orig, cur] : boost::combine(orig_range, range))
    {
        const auto& [pt1, c1] = orig;
        auto&& [pt2, c2]      = cur;

        pt2.x = pt1.x;
        if(pt1.y - m_y_center > 0)
        {
            pt2.y = pt1.y + (pt1.y - m_y_center) * scale;
            c2.r  = core::tools::numeric_round_cast<data::mesh::color_t>(255 * scale);
        }
        else
        {
            c2.r = 0;
        }

        pt2.z = pt1.z;
    }

    geometry::data::mesh::generate_point_normals(mesh);
}

//-----------------------------------------------------------------------------

} // namespace tuto13_mesh_generator_cpp.
