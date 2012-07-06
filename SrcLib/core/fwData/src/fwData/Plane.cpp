/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwMath/PlaneFunctions.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Plane.hpp"

#define EPSILON 0.00000001

fwDataRegisterMacro( ::fwData::Plane );

namespace fwData
{

//------------------------------------------------------------------------------

Plane::Plane () : m_isIntersection(true)
{
    m_vPoints[0] = ::fwData::Point::New();
    m_vPoints[1] = ::fwData::Point::New();
    m_vPoints[2] = ::fwData::Point::New();
}

//------------------------------------------------------------------------------

Plane::Plane(::fwData::Point::sptr _point1, ::fwData::Point::sptr _point2, ::fwData::Point::sptr _point3)
{
    m_vPoints[0] = _point1;
    m_vPoints[1] = _point2;
    m_vPoints[2] = _point3;
    this->computePlaneFromPoints();
}

//------------------------------------------------------------------------------

Plane::~Plane ()
{}

//------------------------------------------------------------------------------

void Plane::shallowCopy( Plane::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_vPoints = _source->m_vPoints;
    m_plane = _source->m_plane;
}

//------------------------------------------------------------------------------

void Plane::deepCopy( Plane::csptr _source )
{
    this->fieldDeepCopy( _source );
    m_vPoints[0]->deepCopy( _source->m_vPoints[0] );
    m_vPoints[1]->deepCopy( _source->m_vPoints[1] );
    m_vPoints[2]->deepCopy( _source->m_vPoints[2] );
    m_plane = _source->m_plane;
}

//------------------------------------------------------------------------------

bool Plane::operator==( const Plane & _plane )
{
    bool result = false;
    float dx = float(m_plane[0] - _plane.getPlane()[0] );
    float dy = float(m_plane[1] - _plane.getPlane()[1] );
    float dz = float(m_plane[2] - _plane.getPlane()[2] );
    float dd = float(m_plane[3] - _plane.getPlane()[3] );

    if (fabs(dx) < EPSILON && fabs(dy) < EPSILON && fabs(dz) < EPSILON && fabs(dd) < EPSILON)
    {
        result = true;
    }
    return result;
}
//------------------------------------------------------------------------------

void Plane::setValue(::fwData::Point::sptr _point1, ::fwData::Point::sptr _point2, ::fwData::Point::sptr _point3)
{
    m_vPoints[0] = _point1;
    m_vPoints[1] = _point2;
    m_vPoints[2] = _point3;
    this->computePlaneFromPoints();
}

//------------------------------------------------------------------------------

void Plane::computePlaneFromPoints()
{
    ::fwMath::setValues(m_plane, m_vPoints[0]->getCRefCoord(), m_vPoints[1]->getCRefCoord(), m_vPoints[2]->getCRefCoord());
}

//------------------------------------------------------------------------------

} // namespace fwData


