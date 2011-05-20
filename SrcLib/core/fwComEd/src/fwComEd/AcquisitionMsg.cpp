/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/AcquisitionMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::AcquisitionMsg, ::fwComEd::AcquisitionMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string AcquisitionMsg::ADD_RECONSTRUCTION          = "ADD_RECONSTRUCTION";
std::string AcquisitionMsg::REMOVED_RECONSTRUCTIONS     = "REMOVED_RECONSTRUCTIONS";
std::string AcquisitionMsg::SHOW_RECONSTRUCTIONS        = "ShowReconstructions";
std::string AcquisitionMsg::VISIBILITY                  = "VISIBILITY";
std::string AcquisitionMsg::NEW_RECONSTRUCTION_SELECTED = "NEW_RECONSTRUCTION_SELECTED";

//-----------------------------------------------------------------------------

AcquisitionMsg::AcquisitionMsg() throw()
{}

//-----------------------------------------------------------------------------

AcquisitionMsg::~AcquisitionMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

