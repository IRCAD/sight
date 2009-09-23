/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Object.hpp"
#include "fwData/GenericField.hpp"

#include "fwData/Boolean.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Boolean,::fwData::Boolean );
namespace fwData
{


//------------------------------------------------------------------------------

Boolean::Boolean( const bool value ) throw()
:	GenericField< bool >( std::string( "Boolean" ), value )
{}

//------------------------------------------------------------------------------

Boolean::~Boolean() throw()
{}

//------------------------------------------------------------------------------

} // namespace fwData
