/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Vector.hpp"

fwDataRegisterMacro( ::fwData::Vector );

namespace fwData
{

//------------------------------------------------------------------------------

Vector::Vector(::fwData::Object::Key key)
{}

//------------------------------------------------------------------------------

Vector::~Vector()
{}

//------------------------------------------------------------------------------

void Vector::shallowCopy(const Object::csptr &_source )
{
    Vector::csptr other = Vector::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_attrContainer = other->m_attrContainer;
}

//------------------------------------------------------------------------------

void Vector::deepCopy(const Object::csptr &source )
{
    Vector::csptr other = Vector::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (source?source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source );
    m_attrContainer.clear();
    m_attrContainer.resize(other->m_attrContainer.size());
    std::transform(
            other->begin(), other->end(),
            this->begin(),
            &::fwData::Object::copy< ValueType::element_type >
    );
}

//------------------------------------------------------------------------------

}
