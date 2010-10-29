/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/Composite.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/updater/DataInfoFromMsgUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::DataInfoFromMsgUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

DataInfoFromMsgUpdaterSrv::DataInfoFromMsgUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

DataInfoFromMsgUpdaterSrv::~DataInfoFromMsgUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void DataInfoFromMsgUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    for (   ManagedEvents::iterator it = m_managedEvents.begin();
            it != m_managedEvents.end();
            ++it )
    {
        std::string event         = it->get<0>();
        std::string uuid          = it->get<1>();
        std::string compositeKey  = it->get<2>();
        ctrlSelection::IUpdaterSrv::ActionType action        = it->get<3>();

        //  test if message correspond to a defined event
        if( _msg->hasEvent( event ) )
        {
            ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast( _msg->getSubject().lock() );
            SLM_ASSERT(obj,"Sorry, the subject of message is not a ::fwData::Object");

            // Test if we manage this event from this object message uid
            if( obj->getID() == uuid )
            {
                ::fwData::Object::sptr dataInfo = ::boost::const_pointer_cast< ::fwData::Object >(_msg->getDataInfo(  event ));
                SLM_ASSERT("no dataInfo set!!!" ,  dataInfo
                        ||  action== ctrlSelection::IUpdaterSrv::REMOVE
                        ||  action== ctrlSelection::IUpdaterSrv::REMOVE_IF_PRESENT
                );
                // Udpate the composite object referenced by the composite key
                this->updateComposite(composite, dataInfo , compositeKey , action );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void DataInfoFromMsgUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DataInfoFromMsgUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DataInfoFromMsgUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->configureManagedEvents(m_configuration);
}

//-----------------------------------------------------------------------------

void DataInfoFromMsgUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DataInfoFromMsgUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void DataInfoFromMsgUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
