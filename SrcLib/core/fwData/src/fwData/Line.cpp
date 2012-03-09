/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"



#include "fwData/Line.hpp"

fwDataRegisterMacro( ::fwData::Line );

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

void Line::shallowCopy( Line::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_position = _source->m_position;
    m_direction = _source->m_direction;
}

//------------------------------------------------------------------------------

void Line::deepCopy( Line::csptr _source )
{
    this->fieldDeepCopy( _source );
    m_position = ::fwData::Object::copy( _source->m_position );
    m_direction = ::fwData::Object::copy( _source->m_direction );
}

//------------------------------------------------------------------------------

void Line::setValue( ::fwData::Point::sptr _position, ::fwData::Point::sptr  _direction) {
    m_position = _position;
    m_direction = _direction;
}

} // namespace fwData


