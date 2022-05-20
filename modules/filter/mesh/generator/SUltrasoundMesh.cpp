/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "modules/filter/mesh/generator/SUltrasoundMesh.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <geometry/data/IntrasecTypes.hpp>
#include <geometry/data/Mesh.hpp>
#include <geometry/data/VectorFunctions.hpp>

#include <service/macros.hpp>

#include <boost/math/constants/constants.hpp>

namespace sight::module::filter::mesh
{

namespace generator
{

// -----------------------------------------------------------------------------

static const char* s_DEPTH       = "depth";
static const char* s_ANGLE       = "angle";
static const char* s_WIDTH       = "width";
static const char* s_DELTA_DEPTH = "deltaDepth";
static const char* s_SHAPE       = "shape";

static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT  = "setIntParameter";
static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT = "setBoolParameter";

static const std::string s_RESOLUTION_X_CONFIG    = "resolutionX";
static const std::string s_RESOLUTION_Y_CONFIG    = "resolutionY";
static const std::string s_DEPTH_CONFIG           = "depth";
static const std::string s_WIDTH_CONFIG           = "width";
static const std::string s_ANGLE_CONFIG           = "angle";
static const std::string s_DELTA_DEPTH_CONFIG     = "deltaDepth";
static const std::string s_IS_CONVEX_SHAPE_CONFIG = "isConvexShape";

// -----------------------------------------------------------------------------

SUltrasoundMesh::SUltrasoundMesh() noexcept
{
    newSlot(s_SET_INT_PARAMETER_SLOT, &SUltrasoundMesh::setIntParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SUltrasoundMesh::setBoolParameter, this);
}

// -----------------------------------------------------------------------------

SUltrasoundMesh::~SUltrasoundMesh()
{
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::configuring()
{
    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_resolutionX = config.get<std::size_t>(s_RESOLUTION_X_CONFIG, m_resolutionX);
    m_resolutionY = config.get<std::size_t>(s_RESOLUTION_Y_CONFIG, m_resolutionY);
    m_depth       = config.get<int>(s_DEPTH_CONFIG, m_depth);
    m_width       = config.get<int>(s_WIDTH_CONFIG, m_width);
    m_angle       = config.get<int>(s_ANGLE_CONFIG, m_angle);
    m_deltaDepth  = config.get<int>(s_DELTA_DEPTH_CONFIG, m_deltaDepth);
    m_shape       = config.get<bool>(s_IS_CONVEX_SHAPE_CONFIG, m_shape);
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::starting()
{
    // Allocate position array
    const long x = static_cast<long>(m_resolutionX);
    const long y = static_cast<long>(m_resolutionY);
    m_meshPositionArray.resize(boost::extents[x][y][3]);

    const auto mesh = m_mesh.lock();

    // Create mesh and notify
    this->updateMeshPosition();
    this->createQuadMesh(mesh.get_shared());
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::stopping()
{
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::updating()
{
    const auto mesh = m_mesh.lock();

    this->updateMeshPosition();
    this->updateQuadMesh(mesh.get_shared());
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::updateMeshPosition()
{
    // compute delta angle
    const double thetaInit   = (90. - m_angle / 2.) * boost::math::constants::pi<double>() / 180.;
    const double thetaEnd    = (90. + m_angle / 2.) * boost::math::constants::pi<double>() / 180.;
    const double delta_theta = (thetaEnd - thetaInit) / (double(m_resolutionX) - 1.);

    // compute delta lengths
    const double dDepth = m_depth / (double(m_resolutionY) - 1.);
    const double dWidth = m_width / (double(m_resolutionX) - 1.);

    const fwVec3d centerPosition = {{0., 0., 0.}};
    const fwVec3d direction      = {{0., 1., 0.}};
    const fwVec3d normal         = {{1., 0., 0.}};

    for(unsigned int widthGrid = 0 ;
        widthGrid < m_resolutionX ;
        ++widthGrid)
    {
        fwVec3d directionLive;
        fwVec3d centerLive;
        if(m_shape)
        {
            const double angleLive = thetaInit + delta_theta * double(m_resolutionX - widthGrid - 1);
            directionLive = std::cos(angleLive) * normal + std::sin(angleLive) * direction;
            centerLive    = centerPosition;
        }
        else
        {
            directionLive = direction;
            centerLive    = centerPosition + (widthGrid * dWidth - double(m_width) / 2.f) * normal;
        }

        for(unsigned int depthGrid = 0 ;
            depthGrid < m_resolutionY ;
            ++depthGrid)
        {
            const fwVec3d posReal = centerLive + (depthGrid * dDepth + m_deltaDepth) * directionLive;

            m_meshPositionArray[widthGrid][depthGrid][0] = static_cast<float>(posReal[0]);
            m_meshPositionArray[widthGrid][depthGrid][1] = static_cast<float>(posReal[1]);
            m_meshPositionArray[widthGrid][depthGrid][2] = static_cast<float>(posReal[2]);
        }
    }
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::createQuadMesh(const data::Mesh::sptr& _mesh) const
{
    const std::size_t width  = m_meshPositionArray.shape()[0];
    const std::size_t height = m_meshPositionArray.shape()[1];

    const std::size_t numPointsTotal = width * height;
    const std::size_t numQuads       = (width - 1) * (height - 1);

    _mesh->resize(
        data::Mesh::size_t(numPointsTotal),
        data::Mesh::size_t(numQuads),
        data::Mesh::CellType::QUAD,
        data::Mesh::Attributes::POINT_TEX_COORDS
        | data::Mesh::Attributes::POINT_NORMALS
    );

    // pointer on the positions buffer
    const float* pointsIn = static_cast<const float*>(m_meshPositionArray.data());

    // points position
    auto pointsItr = _mesh->zip_range<data::iterator::point::xyz, data::iterator::point::uv>().begin();

    // cells index (4 in a row)
    auto cellsItr = _mesh->begin<data::iterator::cell::quad>();

    for(std::size_t i = 0 ; i < width ; ++i)
    {
        for(std::size_t j = 0 ; j < height ; ++j)
        {
            auto&& [p, tex] = *pointsItr;
            p.x             = *pointsIn++;
            p.y             = *pointsIn++;
            p.z             = *pointsIn++;

            tex.u = data::Mesh::texcoord_t(i) / static_cast<data::Mesh::texcoord_t>(width - 1);
            tex.v = data::Mesh::texcoord_t(j) / static_cast<data::Mesh::texcoord_t>(height - 1);
            ++pointsItr;
        }
    }

    // index for each cell
    for(std::size_t i = 0 ; i < width - 1 ; ++i)
    {
        for(std::size_t j = 0 ; j < height - 1 ; ++j)
        {
            const data::Mesh::cell_t idx1 = data::Mesh::cell_t(j + i * height);
            const data::Mesh::cell_t idx2 = idx1 + 1;
            const data::Mesh::cell_t idx4 = data::Mesh::cell_t(idx1 + height);
            const data::Mesh::cell_t idx3 = idx4 + 1;

            cellsItr->pt[0] = idx1;
            cellsItr->pt[1] = idx2;
            cellsItr->pt[2] = idx3;
            cellsItr->pt[3] = idx4;

            ++cellsItr;
        }
    }

    geometry::data::Mesh::generatePointNormals(_mesh);

    const auto sig = _mesh->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
    sig->asyncEmit();
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::updateQuadMesh(const data::Mesh::sptr& _mesh)
{
    const int width  = static_cast<int>(m_meshPositionArray.shape()[0]);
    const int height = static_cast<int>(m_meshPositionArray.shape()[1]);

    // check that values describe a matrix
    SIGHT_ASSERT("At least 2*2 points are needed", width > 1 && height > 2);

    // Copy new positions inside the mesh
    auto pointsItr = _mesh->begin<data::iterator::point::xyz>();

    for(int i = 0 ; i < width ; ++i)
    {
        for(int j = 0 ; j < height ; ++j)
        {
            pointsItr->x = m_meshPositionArray[i][j][0];
            pointsItr->y = m_meshPositionArray[i][j][1];
            pointsItr->z = m_meshPositionArray[i][j][2];
            ++pointsItr;
        }
    }

    const auto sig = _mesh->signal<data::Mesh::signal_t>(
        data::Mesh::s_VERTEX_MODIFIED_SIG
    );
    sig->asyncEmit();
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::setIntParameter(int _val, std::string _key)
{
    if(_key == s_DEPTH)
    {
        m_depth = _val;
    }
    else if(_key == s_ANGLE)
    {
        m_angle = _val;
    }
    else if(_key == s_WIDTH)
    {
        m_width = _val;
    }
    else if(_key == s_DELTA_DEPTH)
    {
        m_deltaDepth = _val;
    }

    this->updating();
}

// -----------------------------------------------------------------------------

void SUltrasoundMesh::setBoolParameter(bool _val, std::string _key)
{
    if(_key == s_SHAPE)
    {
        m_shape = _val;
    }

    this->updating();
}

// -----------------------------------------------------------------------------

} // namespace generator

} // namespace sight::module::filter::mesh
