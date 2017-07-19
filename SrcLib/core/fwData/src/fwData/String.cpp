/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/GenericField.hpp"
#include "fwData/String.hpp"

fwDataRegisterMacro( ::fwData::String );

namespace fwData
{

//------------------------------------------------------------------------------

String::String(::fwData::Object::Key key ) noexcept
{
}

//------------------------------------------------------------------------------

String::~String() noexcept
{
}

//------------------------------------------------------------------------------

void String::shallowCopy(const Object::csptr &_source )
{
    String::csptr other = String::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_value = other->m_value;
}

//------------------------------------------------------------------------------

void String::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    String::csptr other = String::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    m_value = other->m_value;
}

} // namespace fwData
