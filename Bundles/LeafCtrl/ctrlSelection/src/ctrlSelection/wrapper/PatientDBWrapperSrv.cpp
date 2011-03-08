/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/Base.hpp>

#include <fwData/PatientDB.hpp>

#include <fwComEd/PatientDBMsg.hpp>


#include "ctrlSelection/wrapper/PatientDBWrapperSrv.hpp"
#include <fwServices/IEditionService.hpp>


REGISTER_SERVICE( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::PatientDBWrapperSrv, ::fwData::PatientDB ) ;

namespace ctrlSelection
{

namespace wrapper
{

//-----------------------------------------------------------------------------

PatientDBWrapperSrv::PatientDBWrapperSrv() throw()
{
    //TODO addNewHandledEvent( ::fwServices:: ObjectMsg::NEW_OBJECT );
    addNewHandledEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT );
    //TODO addNewHandledEvent( ::fwServices:: ObjectMsg::DELETE_OBJECT );
}

//-----------------------------------------------------------------------------

PatientDBWrapperSrv::~PatientDBWrapperSrv() throw()
{}

//-----------------------------------------------------------------------------

void PatientDBWrapperSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    if ( message->hasEvent( ::fwServices:: ObjectMsg::UPDATED_OBJECT ) )
    {
        ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >();
        SLM_ASSERT("pDPDB not instanced", pDPDB);

        assert( message->getDataInfo( ::fwServices:: ObjectMsg::UPDATED_OBJECT   ) == pDPDB );

        ::fwComEd::PatientDBMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT, pDPDB );
        msg->addEvent( ::fwComEd::PatientDBMsg::NEW_LOADED_PATIENT );

        ::fwServices::IEditionService::notify( this->getSptr(),  pDPDB, msg, ::fwServices::ComChannelService::NOTIFY_SOURCE );
    }
    //TODO other event
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection


