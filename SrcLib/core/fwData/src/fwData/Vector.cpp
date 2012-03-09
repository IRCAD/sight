/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Vector.hpp"

fwDataRegisterMacro( ::fwData::Vector );

namespace fwData
{

//------------------------------------------------------------------------------

Vector::Vector()
{}

//------------------------------------------------------------------------------

Vector::~Vector()
{}

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

void Vector::deepCopy( Vector::csptr source )
{
    this->fieldDeepCopy( source );
    this->clear();
    std::transform(source->begin(), source->end(),
                       this->begin(),
                       &::fwData::Object::copy< Vector::value_type::element_type >
                      );
}

//------------------------------------------------------------------------------

}
