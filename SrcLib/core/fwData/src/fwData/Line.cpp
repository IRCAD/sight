/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"



#include "fwData/Line.hpp"

fwDataRegisterMacro( ::fwData::Line );

namespace fwData
{

//------------------------------------------------------------------------------

Line::Line (::fwData::Object::Key key)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Line::~Line ()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Line::shallowCopy(const Object::csptr &_source )
{
    Line::csptr other = Line::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_position = other->m_position;
    m_direction = other->m_direction;
}

//------------------------------------------------------------------------------

void Line::deepCopy(const Object::csptr &_source )
{
    Line::csptr other = Line::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source );
    m_position = ::fwData::Object::copy( other->m_position );
    m_direction = ::fwData::Object::copy( other->m_direction );
}

//------------------------------------------------------------------------------

void Line::setValue( ::fwData::Point::sptr _position, ::fwData::Point::sptr  _direction) {
    m_position = _position;
    m_direction = _direction;
}

} // namespace fwData


