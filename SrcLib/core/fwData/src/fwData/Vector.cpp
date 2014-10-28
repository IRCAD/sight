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

void Vector::cachedDeepCopy(const Object::csptr &source, DeepCopyCacheType &cache)
{
    Vector::csptr other = Vector::dynamicConstCast(source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (source?source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( source, cache );
    m_attrContainer.clear();
    m_attrContainer.reserve(other->m_attrContainer.size());
    BOOST_FOREACH(const ContainerType::value_type &obj, other->m_attrContainer)
    {
        m_attrContainer.push_back( ::fwData::Object::copy(obj, cache) );
    }
}

//------------------------------------------------------------------------------

}
