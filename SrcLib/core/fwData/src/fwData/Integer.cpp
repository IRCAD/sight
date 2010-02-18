/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Object.hpp"
#include "fwData/GenericField.hpp"
#include "fwData/Integer.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Integer, ::fwData::Integer );

namespace fwData
{
//------------------------------------------------------------------------------

Integer::Integer( const int value ) throw()
:   GenericField< int >( value )
{}

//------------------------------------------------------------------------------

Integer::~Integer() throw()
{}

//------------------------------------------------------------------------------

void Integer::shallowCopy( Integer::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
}

//------------------------------------------------------------------------------

void Integer::deepCopy( Integer::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
}

} // namespace fwData
