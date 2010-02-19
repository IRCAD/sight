/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>


#include "fwData/Vector.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Vector,  ::fwData::Vector);

namespace fwData
{


Vector::Vector()
{
    SLM_TRACE_FUNC();
    // TODO Auto-generated constructor stub
}


Vector::~Vector()
{
    SLM_TRACE_FUNC();
    // TODO Auto-generated destructor stub
}


Vector::Container &Vector::getRefContainer()
{
    return m_container;
}


Vector::Container const &Vector::getRefContainer() const
{
    return m_container;
}

//------------------------------------------------------------------------------

void Vector::shallowCopy( Vector::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_container = _source->m_container;
}

//------------------------------------------------------------------------------

void Vector::deepCopy( Vector::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->m_container.clear();

    for(    Vector::Container::const_iterator iter = _source->m_container.begin();
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
