/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/Vector.hpp"

fwDataRegisterMacro( ::fwData::Vector );

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
    this->fieldShallowCopy( _source );
    (ObjectVectorType)(*this) = (ObjectVectorType)(*(_source.get()));
}

//------------------------------------------------------------------------------

void Vector::deepCopy( Vector::csptr _source )
{
    this->fieldDeepCopy( _source );

    this->clear();

    for(    Vector::Container::const_iterator iter = _source->begin();
            iter != _source->end();
            ++iter )
    {
        ::fwData::Object::sptr newObj = ::fwData::Factory::New( (*iter)->getClassname() );
        newObj->deepCopy( *iter );
        this->push_back( newObj );
    }
}

//------------------------------------------------------------------------------

}
