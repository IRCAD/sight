/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto14MeshGenerator/AlgoMeshDeformation.hpp"

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/Mesh.hpp>

#include <fwTools/NumericRoundCast.hxx>

namespace Tuto14MeshGenerator
{

//-----------------------------------------------------------------------------

AlgoMeshDeformation::AlgoMeshDeformation() noexcept :
    m_nbStep(0),
    m_amplitude(40),
    m_step(0),
    m_direction(1),
    m_nbPoints(0),
    m_nbCells(0),
    m_yCenter(0)
{
}

//-----------------------------------------------------------------------------

AlgoMeshDeformation::~AlgoMeshDeformation() noexcept
{
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::setParam(
    ::fwData::Mesh::sptr _mesh,
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

void AlgoMeshDeformation::computeDeformation( ::fwData::Mesh::sptr _mesh,
                                              const unsigned int _nbStep,
                                              const unsigned int _amplitude )
{
    if (    m_mesh.expired() ||
            m_nbPoints != _mesh->getNumberOfPoints() ||
            m_nbCells != _mesh->getNumberOfCells()  ||
            !_mesh->getPointColorsArray())
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
    SLM_TRACE_FUNC();
    m_originPoints = ::fwData::Object::copy( m_mesh.lock()->getPointsArray() );
    m_step         = 0;

    if ( !m_mesh.lock()->getPointColorsArray() )
    {
        ::fwDataTools::Mesh::colorizeMeshPoints( m_mesh.lock() );
    }

    m_meshHelper = ::fwDataTools::helper::Mesh::New(m_mesh.lock());

    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();

    ::fwData::Mesh::PointsMultiArrayType points = m_meshHelper->getPoints();
    float coord;
    for(unsigned int i = 0; i < m_nbPoints; ++i)
    {
        coord = points[i][1];
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
    SLM_TRACE_FUNC();
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

    ::fwDataTools::helper::Array originPointsHelper(m_originPoints);

    ::fwData::Mesh::PointsMultiArrayType points      = m_meshHelper->getPoints();
    ::fwData::Mesh::PointColorsMultiArrayType colors = m_meshHelper->getPointColors();

    ::fwData::Mesh::PointsMultiArrayType opoints =
        ::fwData::Mesh::PointsMultiArrayType(
            static_cast< ::fwData::Mesh::PointsMultiArrayType::element* >(originPointsHelper.getBuffer()),
            ::boost::extents[m_nbPoints][3] );

    for(unsigned int i = 0; i < m_nbPoints; ++i)
    {
        points[i][0] = opoints[i][0];
        OSLM_TRACE("opoints[i][1] - m_yCenter = " <<  opoints[i][1] - m_yCenter);
        if( opoints[i][1] - m_yCenter > 0 )
        {
            points[i][1] = opoints[i][1] + (opoints[i][1] - m_yCenter) * scale;
            colors[i][0] = ::fwTools::numericRoundCast< ::fwData::Mesh::ColorValueType >(255 * scale);
        }
        else
        {
            colors[i][0] = 0;
        }
        points[i][2] = opoints[i][2];
    }

    ::fwDataTools::Mesh::generatePointNormals( m_mesh.lock() );
}

//-----------------------------------------------------------------------------

} // namespace Tuto14MeshGenerator

