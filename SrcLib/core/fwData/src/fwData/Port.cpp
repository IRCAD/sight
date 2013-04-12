/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/Port.hpp"

fwDataRegisterMacro( ::fwData::Port );

namespace fwData
{
//------------------------------------------------------------------------------

Port::Port(::fwData::Object::Key key) : m_identifier("IDNOTdefined") , m_type("TypeNotDefined")
{}

//------------------------------------------------------------------------------

Port::~Port()
{}

//------------------------------------------------------------------------------

void Port::shallowCopy(const Object::csptr &_source )
{
    Port::csptr other = Port::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_identifier = other->m_identifier;
    m_type = other->m_type;
}

//------------------------------------------------------------------------------

void Port::deepCopy(const Object::csptr &_source )
{
    Port::csptr other = Port::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source );

    m_identifier = other->m_identifier;
    m_type = other->m_type;
}

//------------------------------------------------------------------------------

} // namespace fwData
