/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/filter/mesh/generator/ultrasound_mesh.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/mesh.hpp>
#include <geometry/data/types.hpp>
#include <geometry/data/vector_functions.hpp>

#include <service/macros.hpp>

#include <boost/math/constants/constants.hpp>

namespace sight::module::filter::mesh::generator
{

// -----------------------------------------------------------------------------

static const char* s_depth       = "depth";
static const char* s_angle       = "angle";
static const char* s_width       = "width";
static const char* s_delta_depth = "deltaDepth";
static const char* s_shape       = "shape";

static const core::com::slots::key_t SET_INT_PARAMETER_SLOT  = "setIntParameter";
static const core::com::slots::key_t SET_BOOL_PARAMETER_SLOT = "setBoolParameter";

static const std::string s_RESOLUTION_X_CONFIG    = "resolutionX";
static const std::string s_RESOLUTION_Y_CONFIG    = "resolutionY";
static const std::string s_DEPTH_CONFIG           = "depth";
static const std::string s_WIDTH_CONFIG           = "width";
static const std::string s_ANGLE_CONFIG           = "angle";
static const std::string s_DELTA_DEPTH_CONFIG     = "deltaDepth";
static const std::string s_IS_CONVEX_SHAPE_CONFIG = "isConvexShape";

// -----------------------------------------------------------------------------

ultrasound_mesh::ultrasound_mesh() noexcept
{
    new_slot(SET_INT_PARAMETER_SLOT, &ultrasound_mesh::setIntParameter, this);
    new_slot(SET_BOOL_PARAMETER_SLOT, &ultrasound_mesh::setBoolParameter, this);
}

// -----------------------------------------------------------------------------

ultrasound_mesh::~ultrasound_mesh()
= default;

// -----------------------------------------------------------------------------

void ultrasound_mesh::configuring(const config_t& _config)
{
    const auto config = _config.get_child("config.<xmlattr>");

    m_resolutionX = config.get<std::size_t>(s_RESOLUTION_X_CONFIG, m_resolutionX);
    m_resolutionY = config.get<std::size_t>(s_RESOLUTION_Y_CONFIG, m_resolutionY);
    m_depth       = config.get<int>(s_DEPTH_CONFIG, m_depth);
    m_width       = config.get<int>(s_WIDTH_CONFIG, m_width);
    m_angle       = config.get<int>(s_ANGLE_CONFIG, m_angle);
    m_deltaDepth  = config.get<int>(s_DELTA_DEPTH_CONFIG, m_deltaDepth);
    m_shape       = config.get<bool>(s_IS_CONVEX_SHAPE_CONFIG, m_shape);
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::starting()
{
    // Allocate position array
    const auto x = static_cast<std::int64_t>(m_resolutionX);
    const auto y = static_cast<std::int64_t>(m_resolutionY);
    m_meshPositionArray.resize(boost::extents[x][y][3]);

    const auto mesh = m_mesh.lock();

    // Create mesh and notify
    this->updateMeshPosition();
    this->createQuadMesh(mesh.get_shared());
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::stopping()
{
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::updating()
{
    const auto mesh = m_mesh.lock();

    this->updateMeshPosition();
    this->updateQuadMesh(mesh.get_shared());
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::updateMeshPosition()
{
    // compute delta angle
    const double theta_init  = (90. - m_angle / 2.) * boost::math::constants::pi<double>() / 180.;
    const double theta_end   = (90. + m_angle / 2.) * boost::math::constants::pi<double>() / 180.;
    const double delta_theta = (theta_end - theta_init) / (double(m_resolutionX) - 1.);

    // compute delta lengths
    const double d_depth = m_depth / (double(m_resolutionY) - 1.);
    const double d_width = m_width / (double(m_resolutionX) - 1.);

    const fwVec3d center_position = {{0., 0., 0.}};
    const fwVec3d direction       = {{0., 1., 0.}};
    const fwVec3d normal          = {{1., 0., 0.}};

    for(unsigned int width_grid = 0 ;
        width_grid < m_resolutionX ;
        ++width_grid)
    {
        fwVec3d direction_live;
        fwVec3d center_live;
        if(m_shape)
        {
            const double angle_live = theta_init + delta_theta * double(m_resolutionX - width_grid - 1);
            direction_live = std::cos(angle_live) * normal + std::sin(angle_live) * direction;
            center_live    = center_position;
        }
        else
        {
            direction_live = direction;
            center_live    = center_position + (width_grid * d_width - double(m_width) / 2.F) * normal;
        }

        for(unsigned int depth_grid = 0 ;
            depth_grid < m_resolutionY ;
            ++depth_grid)
        {
            const fwVec3d pos_real = center_live + (depth_grid * d_depth + m_deltaDepth) * direction_live;

            m_meshPositionArray[width_grid][depth_grid][0] = static_cast<float>(pos_real[0]);
            m_meshPositionArray[width_grid][depth_grid][1] = static_cast<float>(pos_real[1]);
            m_meshPositionArray[width_grid][depth_grid][2] = static_cast<float>(pos_real[2]);
        }
    }
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::createQuadMesh(const data::mesh::sptr& _mesh) const
{
    const std::size_t width  = m_meshPositionArray.shape()[0];
    const std::size_t height = m_meshPositionArray.shape()[1];

    const std::size_t num_points_total = width * height;
    const std::size_t num_quads        = (width - 1) * (height - 1);

    _mesh->resize(
        data::mesh::size_t(num_points_total),
        data::mesh::size_t(num_quads),
        data::mesh::cell_type_t::QUAD,
        data::mesh::Attributes::POINT_TEX_COORDS
        | data::mesh::Attributes::POINT_NORMALS
    );

    // pointer on the positions buffer
    const auto* points_in = static_cast<const float*>(m_meshPositionArray.data());

    // points position
    auto points_itr = _mesh->zip_range<data::iterator::point::xyz, data::iterator::point::uv>().begin();

    // cells index (4 in a row)
    auto cells_itr = _mesh->begin<data::iterator::cell::quad>();

    for(std::size_t i = 0 ; i < width ; ++i)
    {
        for(std::size_t j = 0 ; j < height ; ++j)
        {
            auto&& [p, tex] = *points_itr;
            p.x             = *points_in++;
            p.y             = *points_in++;
            p.z             = *points_in++;

            tex.u = data::mesh::texcoord_t(i) / static_cast<data::mesh::texcoord_t>(width - 1);
            tex.v = data::mesh::texcoord_t(j) / static_cast<data::mesh::texcoord_t>(height - 1);
            ++points_itr;
        }
    }

    // index for each cell
    for(std::size_t i = 0 ; i < width - 1 ; ++i)
    {
        for(std::size_t j = 0 ; j < height - 1 ; ++j)
        {
            const auto idx1               = data::mesh::cell_t(j + i * height);
            const data::mesh::cell_t idx2 = idx1 + 1;
            const auto idx4               = data::mesh::cell_t(idx1 + height);
            const data::mesh::cell_t idx3 = idx4 + 1;

            cells_itr->pt[0] = idx1;
            cells_itr->pt[1] = idx2;
            cells_itr->pt[2] = idx3;
            cells_itr->pt[3] = idx4;

            ++cells_itr;
        }
    }

    geometry::data::mesh::generatePointNormals(_mesh);

    const auto sig = _mesh->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
    sig->async_emit();
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::updateQuadMesh(const data::mesh::sptr& _mesh)
{
    const int width  = static_cast<int>(m_meshPositionArray.shape()[0]);
    const int height = static_cast<int>(m_meshPositionArray.shape()[1]);

    // check that values describe a matrix
    SIGHT_ASSERT("At least 2*2 points are needed", width > 1 && height > 2);

    // Copy new positions inside the mesh
    auto points_itr = _mesh->begin<data::iterator::point::xyz>();

    for(int i = 0 ; i < width ; ++i)
    {
        for(int j = 0 ; j < height ; ++j)
        {
            points_itr->x = m_meshPositionArray[i][j][0];
            points_itr->y = m_meshPositionArray[i][j][1];
            points_itr->z = m_meshPositionArray[i][j][2];
            ++points_itr;
        }
    }

    const auto sig = _mesh->signal<data::mesh::signal_t>(
        data::mesh::VERTEX_MODIFIED_SIG
    );
    sig->async_emit();
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::setIntParameter(int _val, std::string _key)
{
    if(_key == s_depth)
    {
        m_depth = _val;
    }
    else if(_key == s_angle)
    {
        m_angle = _val;
    }
    else if(_key == s_width)
    {
        m_width = _val;
    }
    else if(_key == s_delta_depth)
    {
        m_deltaDepth = _val;
    }

    this->updating();
}

// -----------------------------------------------------------------------------

void ultrasound_mesh::setBoolParameter(bool _val, std::string _key)
{
    if(_key == s_shape)
    {
        m_shape = _val;
    }

    this->updating();
}

// -----------------------------------------------------------------------------

} // namespace sight::module::filter::mesh::generator
