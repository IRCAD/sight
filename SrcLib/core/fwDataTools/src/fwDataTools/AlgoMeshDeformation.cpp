/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwDataTools/AlgoMeshDeformation.hpp"
#include "fwDataTools/MeshGenerator.hpp"

namespace fwDataTools
{

//-----------------------------------------------------------------------------

AlgoMeshDeformation::AlgoMeshDeformation() throw() :
        m_step(0), m_nbStep(0), m_amplitude(40), m_direction(1), m_nbPoints(0), m_nbCells(0), m_yCenter(0)
{}

//-----------------------------------------------------------------------------

AlgoMeshDeformation::~AlgoMeshDeformation() throw()
{}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::setParam(
            ::fwData::Mesh::sptr _mesh,
            const unsigned int _nbStep,
            const unsigned int _amplitude)
{
    SLM_TRACE_FUNC();
    m_mesh = _mesh;
    m_nbStep = _nbStep;
    m_amplitude = _amplitude;
    m_direction = 1;

    m_nbPoints = _mesh->getNumberOfPoints();
    m_nbCells = _mesh->getNumberOfCells();
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::computeDeformation( ::fwData::Mesh::sptr _mesh,
        const unsigned int _nbStep,
        const unsigned int _amplitude )
{
    if (    m_mesh.expired() ||
            m_nbPoints != _mesh->getNumberOfPoints() ||
            m_nbCells  != _mesh->getNumberOfCells() )
    {
        this->setParam( _mesh, 50, 50 );
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
    m_originPoints = ::fwData::Array::New();
    m_originPoints->deepCopy( m_mesh.lock()->getPointsArray() );
    m_step = 0;

    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();

    ::fwData::Mesh::PointsMultiArrayType points = m_mesh.lock()->getPoints();
    float coord;
    for(unsigned int i = 0; i < m_nbPoints; ++i)
    {
        coord = points[i][1];
        if ( coord < min ) { min = coord; }
        if ( coord > max ) { max = coord; }
    }

    m_yCenter = (max - min) / 2 + min;

    if ( ! m_mesh.lock()->getPointColorsArray() )
    {
        ::fwDataTools::MeshGenerator::colorizeMeshPoints( m_mesh.lock() );
    }
}

//-----------------------------------------------------------------------------

void AlgoMeshDeformation::computeSimu()
{
    SLM_TRACE_FUNC();
    m_step+=m_direction;
    if ( m_step == m_nbStep )
    {
        m_direction = -1;
    }
    else if ( m_step == 0 )
    {
        m_direction = 1;
    }

    const float scale = m_step / (float) m_nbStep;

    ::fwData::Mesh::PointsMultiArrayType points = m_mesh.lock()->getPoints();
    ::fwData::Mesh::PointColorsMultiArrayType colors = m_mesh.lock()->getPointColors();

    ::fwData::Mesh::PointsMultiArrayType opoints =
            ::fwData::Mesh::PointsMultiArrayType(
            static_cast< ::fwData::Mesh::PointsMultiArrayType::element* >(m_originPoints->getBuffer()),
            boost::extents[m_nbPoints][3] );

    for(unsigned int i = 0; i < m_nbPoints; ++i)
    {
        points[i][0] = opoints[i][0];
        OSLM_TRACE("opoints[i][1] - m_yCenter = " <<  opoints[i][1] - m_yCenter);
        if( opoints[i][1] - m_yCenter > 0 )
        {
            points[i][1] = opoints[i][1] + (opoints[i][1] - m_yCenter) * scale;
            colors[i][0] = 255 * scale;
        }
        else
        {
            colors[i][0] = 0;
        }
        points[i][2] = opoints[i][2];
    }

    ::fwDataTools::MeshGenerator::generatePointNormals( m_mesh.lock() );
}

//-----------------------------------------------------------------------------

}   // namespace fwBreathing

