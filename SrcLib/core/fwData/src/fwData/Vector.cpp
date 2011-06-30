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

//------------------------------------------------------------------------------

Vector::Vector()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Vector::~Vector()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Vector &Vector::getRefContainer()
{
    return *this;
}

//------------------------------------------------------------------------------

Vector const &Vector::getRefContainer() const
{
    return *this;
}

//------------------------------------------------------------------------------

void Vector::shallowCopy( Vector::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    (ObjectVectorType)(*this) = (ObjectVectorType)(*(_source.get()));
}

//------------------------------------------------------------------------------

void Vector::deepCopy( Vector::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->clear();

    for(    Vector::Container::const_iterator iter = _source->begin();
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
