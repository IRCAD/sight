/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/PatientMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::PatientMsg, ::fwComEd::PatientMsg );

namespace fwComEd
{

//------------------------------------------------------------------------------

std::string PatientMsg::NEW_PATIENT = "NEW_PATIENT";
std::string PatientMsg::NEW_MATERIAL_FOR_RECONSTRUCTION = "NEW_MATERIAL_FOR_RECONSTRUCTION";

//------------------------------------------------------------------------------

PatientMsg::PatientMsg() throw()
{}

//------------------------------------------------------------------------------

PatientMsg::~PatientMsg() throw()
{}

//------------------------------------------------------------------------------

} // namespace fwComEd

