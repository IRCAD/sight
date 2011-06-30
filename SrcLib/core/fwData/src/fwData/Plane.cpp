/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>
#include <fwMath/PlaneFunctions.hpp>

#include "fwData/Plane.hpp"

#define EPSILON 0.00000001

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Plane, ::fwData::Plane );

namespace fwData
{

//------------------------------------------------------------------------------

Plane::Plane ()
: m_isIntersection(true)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Plane::Plane(::fwData::Point::sptr _point1, ::fwData::Point::sptr _point2, ::fwData::Point::sptr _point3) {
    m_vPoints[0] = _point1;
    m_vPoints[1] = _point2;
    m_vPoints[2] = _point3;
    computePlaneFromPoints();
}

//------------------------------------------------------------------------------

Plane::~Plane ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Plane::shallowCopy( Plane::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    //this->m_vPoints[0] = _source->m_vPoints[0];
    //this->m_vPoints[1] = _source->m_vPoints[1];
    //this->m_vPoints[2] = _source->m_vPoints[2];
    this->m_vPoints = _source->m_vPoints;
    this->m_plane = _source->m_plane;
}

//------------------------------------------------------------------------------

void Plane::deepCopy( Plane::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_vPoints[0]->deepCopy( _source->m_vPoints[0] );
    this->m_vPoints[1]->deepCopy( _source->m_vPoints[1] );
    this->m_vPoints[2]->deepCopy( _source->m_vPoints[2] );
    this->m_plane = _source->m_plane;
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
    computePlaneFromPoints();

}

void Plane::computePlaneFromPoints()
{
    ::fwMath::setValues(m_plane, m_vPoints[0]->getCRefCoord(), m_vPoints[1]->getCRefCoord(), m_vPoints[2]->getCRefCoord());
}

//------------------------------------------------------------------------------

} // namespace fwData


