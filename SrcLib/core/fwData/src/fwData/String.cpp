/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/GenericField.hpp"
#include "fwData/String.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::String, ::fwData::String );

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

} // namespace fwData
