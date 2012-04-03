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
{}

//------------------------------------------------------------------------------

Vector::~Vector()
{}

//------------------------------------------------------------------------------

void Vector::shallowCopy( Vector::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_attrContainer = _source->m_attrContainer;
}

//------------------------------------------------------------------------------

void Vector::deepCopy( Vector::csptr source )
{
    this->fieldDeepCopy( source );
    m_attrContainer.clear();
    m_attrContainer.resize(source->m_attrContainer.size());
    std::transform(
            source->begin(), source->end(),
            this->begin(),
            &::fwData::Object::copy< ValueType::element_type >
    );
}

//------------------------------------------------------------------------------

}
