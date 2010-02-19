/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>


#include "fwData/List.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::List,  ::fwData::List);

namespace fwData
{


List::List()
{
    SLM_TRACE_FUNC();
    // TODO Auto-generated constructor stub
}


List::~List()
{
    SLM_TRACE_FUNC();
    // TODO Auto-generated destructor stub
}


List::Container &List::getRefContainer()
{
    return m_container;
}


List::Container const &List::getRefContainer() const
{
    return m_container;
}

//------------------------------------------------------------------------------

void List::shallowCopy( List::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_container = _source->m_container;
}

//------------------------------------------------------------------------------

void List::deepCopy( List::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->m_container.clear();

    for(    List::Container::const_iterator iter = _source->m_container.begin();
            iter != _source->m_container.end();
            ++iter )
    {
        ::fwTools::Object::sptr newObj = ::fwTools::Factory::buildData( (*iter)->getClassname() );
        newObj->deepCopy( *iter );
        this->m_container.push_back( ::fwData::Object::dynamicCast( newObj ) );
    }
}

//------------------------------------------------------------------------------

}
