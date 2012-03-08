/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/Object.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Boolean.hpp"

fwDataRegisterMacro( ::fwData::Boolean );

namespace fwData
{

//------------------------------------------------------------------------------

Boolean::Boolean( const bool value ) throw()
:   GenericField< bool >( value )
{}

//------------------------------------------------------------------------------

Boolean::~Boolean() throw()
{}

//------------------------------------------------------------------------------
void Boolean::shallowCopy( Boolean::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_value = _source->m_value;
}

//------------------------------------------------------------------------------

void Boolean::deepCopy( Boolean::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_value = _source->m_value;
}

} // namespace fwData
