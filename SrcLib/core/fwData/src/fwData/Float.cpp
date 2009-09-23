/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Object.hpp"
#include "fwData/GenericField.hpp"

#include "fwData/Float.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Float, ::fwData::Float );

namespace fwData
{
//------------------------------------------------------------------------------

Float::Float( const float value ) throw()
:	GenericField< float >( std::string( "Float" ), value )
{}

//------------------------------------------------------------------------------

Float::~Float() throw()
{}

//------------------------------------------------------------------------------

} // namespace fwData
