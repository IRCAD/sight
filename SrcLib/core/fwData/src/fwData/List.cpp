/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"


#include "fwData/List.hpp"


fwDataRegisterMacro( ::fwData::List );

namespace fwData
{

//------------------------------------------------------------------------------

List::List(::fwData::Object::Key key)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

List::~List()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void List::shallowCopy(const Object::csptr &_source )
{
    List::csptr other = List::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_attrContainer = other->m_attrContainer;
}

//------------------------------------------------------------------------------

void List::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    List::csptr other = List::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_attrContainer.clear();
    BOOST_FOREACH(const ContainerType::value_type &obj, other->m_attrContainer)
    {
        m_attrContainer.push_back( ::fwData::Object::copy(obj, cache) );
    }
}

//------------------------------------------------------------------------------

}
