/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/Line.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Line, ::fwData::Line );

namespace fwData
{

//------------------------------------------------------------------------------

Line::Line ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Line::Line( ::fwData::Point::sptr _position, ::fwData::Point::sptr  _direction)
: m_position(_position),
  m_direction(_direction)
{

}

//------------------------------------------------------------------------------

Line::~Line ()
{
	SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Line::sptr Line::clone() const
{
	SLM_TRACE_FUNC();

	Line::NewSptr pNewLine;

	pNewLine->m_position = this->m_position;
	pNewLine->m_direction = this->m_direction;

	return pNewLine;
}

//------------------------------------------------------------------------------

Line & Line::operator=( const ::fwData::Line& _line )
{
	// Copy encoding
	this->m_position = _line.m_position;
	this->m_direction = _line.m_direction;

	return *this;
}

//------------------------------------------------------------------------------

void Line::setValue( ::fwData::Point::sptr _position, ::fwData::Point::sptr  _direction) {
	m_position = _position;
	m_direction = _direction;
}

} // namespace fwData


