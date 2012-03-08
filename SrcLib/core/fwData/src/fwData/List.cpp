/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"


#include "fwData/List.hpp"


fwDataRegisterMacro( ::fwData::List );

namespace fwData
{

//------------------------------------------------------------------------------

List::List()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

List::~List()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

List &List::getRefContainer()
{
    return *this;
}

//------------------------------------------------------------------------------

List const &List::getRefContainer() const
{
    return *this;
}

//------------------------------------------------------------------------------

void List::shallowCopy( List::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    (ObjectListType)(*this) = (ObjectListType)(*(_source.get()));
}

//------------------------------------------------------------------------------

void List::deepCopy( List::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->clear();

    for(    List::Container::const_iterator iter = _source->begin();
            iter != _source->end();
            ++iter )
    {
        ::fwTools::Object::sptr newObj = ::fwTools::Factory::buildData( (*iter)->getClassname() );
        newObj->deepCopy( *iter );
        this->push_back( ::fwData::Object::dynamicCast( newObj ) );
    }
}

//------------------------------------------------------------------------------

}
