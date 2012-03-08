/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/Object.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Float.hpp"


fwDataRegisterMacro( ::fwData::Float );

namespace fwData
{
//------------------------------------------------------------------------------

Float::Float( const float value ) throw()
:   GenericField< float >( value )
{}

//------------------------------------------------------------------------------

Float::~Float() throw()
{}

//------------------------------------------------------------------------------
void Float::shallowCopy( Float::csptr _source )
{
    this->::fwData::Object::fieldShallowCopy( _source );
    this->m_value = _source->m_value;
}

//------------------------------------------------------------------------------

void Float::deepCopy( Float::csptr _source )
{
    this->::fwData::Object::fieldDeepCopy( _source );
    this->m_value = _source->m_value;
}


} // namespace fwData
