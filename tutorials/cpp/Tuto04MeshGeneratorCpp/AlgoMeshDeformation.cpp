/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "AlgoMeshDeformation.hpp"

#include <core/tools/NumericRoundCast.hxx>

#include <geometry/data/Mesh.hpp>

#include <cmath>

namespace data     = sight::data;
namespace geometry = sight::geometry;

namespace Tuto04MeshGeneratorCpp
{

//-----------------------------------------------------------------------------

AlgoMeshDeformation::AlgoMeshDeformation() noexcept =
    default;

//-----------------------------------------------------------------------------

AlgoMeshDeformation::~AlgoMeshDeformation() noexcept =
    default;

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::setParam(
    data::Mesh::sptr _mesh,
    const unsigned int _nbStep,
    const unsigned int _amplitude
)
{
    m_mesh      = _mesh;
    m_nbStep    = _nbStep;
    m_amplitude = _amplitude;
    m_direction = 1;

    m_nbPoints = _mesh->numPoints();
    m_nbCells  = _mesh->numCells();
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::computeDeformation(
    data::Mesh::sptr _mesh,
    const unsigned int _nbStep,
    const unsigned int _amplitude
)
{
    if(m_mesh.expired()
       || m_nbPoints != _mesh->numPoints()
       || m_nbCells != _mesh->numCells()
       || !_mesh->has<data::Mesh::Attributes::POINT_COLORS>())
    {
        this->setParam(_mesh, _nbStep, _amplitude);
        this->initSimu();
    }
    else
    {
        this->computeSimu();
    }
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::initSimu()
{
    const auto mesh = m_mesh.lock();
    m_originMesh = data::Object::copy(mesh);
    m_step       = 0;

    if(!m_mesh.lock()->has<data::Mesh::Attributes::POINT_COLORS>())
    {
        geometry::data::Mesh::colorizeMeshPoints(mesh);
    }

    const auto dumpLock = mesh->dump_lock();

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

    m_yCenter = (max - min) / 2 + min;
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::computeSimu()
{
    namespace core  = sight::core;
    namespace point = data::iterator::point;

    m_step += m_direction;
    if(m_step == static_cast<int>(m_nbStep))
    {
        m_direction = -1;
    }
    else if(m_step == 0)
    {
        m_direction = 1;
    }

    const float scale = static_cast<float>(m_step) / static_cast<float>(m_nbStep);

    const auto mesh         = m_mesh.lock();
    const auto dumpLock     = mesh->dump_lock();
    const auto origDumpLock = m_originMesh->dump_lock();

    const auto origRange = m_originMesh->czip_range<point::xyz, point::rgba>();
    const auto range     = mesh->zip_range<point::xyz, point::rgba>();

    for(const auto& [orig, cur] : boost::combine(origRange, range))
    {
        const auto& [pt1, c1] = orig;
        auto&& [pt2, c2]      = cur;

        pt2.x = pt1.x;
        if(pt1.y - m_yCenter > 0)
        {
            pt2.y = pt1.y + (pt1.y - m_yCenter) * scale;
            c2.r  = core::tools::numericRoundCast<data::Mesh::color_t>(255 * scale);
        }
        else
        {
            c2.r = 0;
        }

        pt2.z = pt1.z;
    }

    geometry::data::Mesh::generatePointNormals(mesh);
}

//-----------------------------------------------------------------------------

} // namespace Tuto04MeshGeneratorCpp.
