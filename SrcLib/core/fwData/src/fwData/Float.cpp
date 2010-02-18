/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Object.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Float.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Float, ::fwData::Float );

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
    ::fwTools::Object::shallowCopyOfChildren( _source );
}

//------------------------------------------------------------------------------

void Float::deepCopy( Float::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
}


} // namespace fwData
