/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>
#include <fwMath/PlaneFunctions.hpp>

#include "fwData/Plane.hpp"

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
	m_plane = ::fwMath::getPlane(_point1->getCRefCoord(), _point2->getCRefCoord(), _point3->getCRefCoord());
}

//------------------------------------------------------------------------------

Plane::~Plane ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Plane::sptr Plane::clone() const
{
	SLM_TRACE_FUNC();

	Plane::NewSptr pNewPlane;

	pNewPlane->m_vPoints[0] = this->m_vPoints[0];
	pNewPlane->m_vPoints[1] = this->m_vPoints[1];
	pNewPlane->m_vPoints[2] = this->m_vPoints[2];

	return pNewPlane;
}

//------------------------------------------------------------------------------

Plane & Plane::operator=( const Plane & _plane )
{
	// Copy encoding
	m_vPoints = _plane.m_vPoints;

	return *this;
}

//------------------------------------------------------------------------------

void Plane::setValue(::fwData::Point::sptr _point1, ::fwData::Point::sptr _point2, ::fwData::Point::sptr _point3)
{
	m_vPoints[0] = _point1;
	m_vPoints[1] = _point2;
	m_vPoints[2] = _point3;
	m_plane = ::fwMath::getPlane(_point1->getCRefCoord(), _point2->getCRefCoord(), _point3->getCRefCoord());

}

//------------------------------------------------------------------------------

} // namespace fwData


