/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "Tuto04MeshGeneratorCpp/AlgoMeshDeformation.hpp"

#include <core/tools/NumericRoundCast.hxx>

#include <fwDataTools/Mesh.hpp>

namespace Tuto04MeshGeneratorCpp
{

//-----------------------------------------------------------------------------

AlgoMeshDeformation::AlgoMeshDeformation() noexcept
{
}

//-----------------------------------------------------------------------------

AlgoMeshDeformation::~AlgoMeshDeformation() noexcept
{
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::setParam(
    data::Mesh::sptr _mesh,
    const unsigned int _nbStep,
    const unsigned int _amplitude)
{
    m_mesh      = _mesh;
    m_nbStep    = _nbStep;
    m_amplitude = _amplitude;
    m_direction = 1;

    m_nbPoints = _mesh->getNumberOfPoints();
    m_nbCells  = _mesh->getNumberOfCells();
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::computeDeformation( data::Mesh::sptr _mesh,
                                              const unsigned int _nbStep,
                                              const unsigned int _amplitude )
{
    if (    m_mesh.expired() ||
            m_nbPoints != _mesh->getNumberOfPoints() ||
            m_nbCells != _mesh->getNumberOfCells()  ||
            !_mesh->hasPointColors())
    {
        this->setParam( _mesh, _nbStep, _amplitude );
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

    if ( !m_mesh.lock()->hasPointColors() )
    {
        ::fwDataTools::Mesh::colorizeMeshPoints( mesh );
    }

    const auto dumpLock = mesh->lock();

    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();

    auto pointsItr       = mesh->begin< data::iterator::ConstPointIterator >();
    const auto pointsEnd = mesh->end< data::iterator::ConstPointIterator >();
    float coord;
    for(; pointsItr != pointsEnd; ++pointsItr)
    {
        coord = pointsItr->point->y;
        if ( coord < min )
        {
            min = coord;
        }
        if ( coord > max )
        {
            max = coord;
        }
    }

    m_yCenter = (max - min) / 2 + min;
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::computeSimu()
{
    m_step += m_direction;
    if ( m_step == m_nbStep )
    {
        m_direction = -1;
    }
    else if ( m_step == 0 )
    {
        m_direction = 1;
    }

    const float scale = m_step / (float) m_nbStep;

    const auto mesh         = m_mesh.lock();
    const auto dumpLock     = mesh->lock();
    const auto origDumpLock = m_originMesh->lock();

    auto pointsItr       = mesh->begin< data::iterator::PointIterator >();
    const auto pointsEnd = mesh->end< data::iterator::PointIterator >();
    auto origPointsItr   = m_originMesh->begin< data::iterator::ConstPointIterator >();

    for(; pointsItr != pointsEnd; ++pointsItr, ++origPointsItr)
    {
        pointsItr->point->x = origPointsItr->point->x;
        if(origPointsItr->point->y - m_yCenter > 0)
        {
            pointsItr->point->y = origPointsItr->point->y + (origPointsItr->point->y - m_yCenter) * scale;
            pointsItr->rgba->r  = core::tools::numericRoundCast< data::Mesh::ColorValueType >(255 * scale);
        }
        else
        {
            pointsItr->rgba->r = 0;
        }
        pointsItr->point->z = origPointsItr->point->z;
    }

    ::fwDataTools::Mesh::generatePointNormals( mesh );
}

//-----------------------------------------------------------------------------

} // namespace Tuto04MeshGeneratorCpp.
