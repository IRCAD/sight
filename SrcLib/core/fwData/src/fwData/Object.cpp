/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/fwID.hpp>

#include <fwTools/Object.hpp>

#include "fwData/visitor/IVisitor.hpp"
#include "fwData/Object.hpp"

// ACH HACK, Force registration in factory
REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwTools::Object, ::fwTools::Object );
REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwTools::Field,  ::fwTools::Field );

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Object, ::fwData::Object);

namespace fwData
{

//------------------------------------------------------------------------------

Object::Object()
{}

//------------------------------------------------------------------------------

Object::~Object()
{}

//------------------------------------------------------------------------------


} // namespace fwData
