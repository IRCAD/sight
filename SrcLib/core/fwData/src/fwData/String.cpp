/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/GenericField.hpp"
#include "fwData/String.hpp"

fwDataRegisterMacro( ::fwData::String );

namespace fwData
{

//------------------------------------------------------------------------------

String::String( const std::string value ) throw()
:   GenericField< std::string >( value )
{}

//------------------------------------------------------------------------------

String::~String() throw()
{}

//------------------------------------------------------------------------------
void String::shallowCopy( String::csptr _source )
{
    this->fieldShallowCopy( _source );
    m_value = _source->m_value;
}

//------------------------------------------------------------------------------

void String::deepCopy( String::csptr _source )
{
    this->fieldDeepCopy( _source );
    m_value = _source->m_value;
}

} // namespace fwData
