/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/PatientDB.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/AcqFromPDBUpdaterSrv.hpp"

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::AcqFromPDBUpdaterSrv, ::fwData::PatientDB    ) ;

//-----------------------------------------------------------------------------

AcqFromPDBUpdaterSrv::AcqFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

AcqFromPDBUpdaterSrv::~AcqFromPDBUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void AcqFromPDBUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

}
