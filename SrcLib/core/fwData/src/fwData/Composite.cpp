/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <boost/foreach.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"


#include "fwData/Composite.hpp"


fwDataRegisterMacro( ::fwData::Composite );

namespace fwData
{


Composite::Composite( ::fwData::Object::Key key )
{
}


Composite::~Composite()
{
}


//------------------------------------------------------------------------------

void Composite::shallowCopy(const Object::csptr &_source )
{
    Composite::csptr other = Composite::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_attrContainer.clear();

    m_attrContainer = other->m_attrContainer;
}

//------------------------------------------------------------------------------

void Composite::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Composite::csptr other = Composite::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_attrContainer.clear();

    BOOST_FOREACH(const ValueType &elem, *other)
    {
        m_attrContainer.insert( ValueType(elem.first, ::fwData::Object::copy(elem.second, cache) ) );
    }
}

//------------------------------------------------------------------------------

}
