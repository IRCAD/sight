/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/Point.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Point, ::fwData::Point );

namespace fwData
{

//------------------------------------------------------------------------------

Point::Point ()
{
	SLM_TRACE_FUNC();
	m_vCoord[0] = 0.0;
	m_vCoord[1] = 0.0;
	m_vCoord[2] = 0.0;
}

//------------------------------------------------------------------------------

Point::~Point ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Point::sptr Point::clone() const
{
	SLM_TRACE_FUNC();

	Point::NewSptr pNewPoint;

	// Copy encoding
	pNewPoint->m_vCoord = this->m_vCoord;

	return pNewPoint;
}

//------------------------------------------------------------------------------

Point & Point::operator=( const Point & _point )
{
	// Copy encoding
	m_vCoord = _point.m_vCoord;

	return *this;
}

//------------------------------------------------------------------------------

} // namespace fwData


