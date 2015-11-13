/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/wrapper/MsgForwarderSrv.hpp"

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>


namespace ctrlSelection
{
namespace wrapper
{
//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::MsgForwarderSrv, ::fwData::Composite );

//-----------------------------------------------------------------------------

MsgForwarderSrv::MsgForwarderSrv() throw()
{
}

//-----------------------------------------------------------------------------

MsgForwarderSrv::~MsgForwarderSrv() throw()
{
}

//-----------------------------------------------------------------------------

void MsgForwarderSrv::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    for( EventType item :  m_managedEvents)
    {
        std::string compositeKey = item.get<0>();
        std::string event        = item.get<1>();
        std::string fromUID      = item.get<2>();
        std::string msgType      = item.get<3>();
        // First element of tuple ( it->get<0>() ) is the event, test if message correspond to a defined event
        if(  message->isA(msgType))
        {
            if(composite->find(compositeKey) != composite->end())
            {
                ::fwData::Object::sptr object = (*composite)[compositeKey];
                ::fwData::Object::sptr objMsg = message->getSubject().lock();
                // Test if we manage this event from this object message uid ( it->get<1>() )
                if( objMsg->getID() == fromUID || fromUID == "*")
                {
                    if(event == "*" || message->hasEvent( event ) )
                    {
                        ::fwServices::ObjectMsg::constCast(message)->setSource( this->getSptr());
                        ::fwServices::ObjectMsg::constCast(message)->setSubject( object);
                        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
                        sig = object->signal< ::fwData::Object::ObjectModifiedSignalType >(
                            ::fwData::Object::s_OBJECT_MODIFIED_SIG);
                        {
                            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
                            sig->asyncEmit( ::fwServices::ObjectMsg::constCast(message) );
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void MsgForwarderSrv::starting()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void MsgForwarderSrv::stopping()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void MsgForwarderSrv::swapping()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void MsgForwarderSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer handleEvents = m_configuration->findAllConfigurationElement("forward");

    SLM_ASSERT("Problem with configuration for MsgForwarderSrv type, missing element \"forward\"",
               handleEvents.size() != 0 );
    m_managedEvents.clear();
    for( ::fwRuntime::ConfigurationElementContainer::Container::value_type item : handleEvents.getElements())
    {
        SLM_FATAL_IF( "Sorry, attribute \"compositeKey\" is missing", !item->hasAttribute("compositeKey") );
        std::string compositeKey = item->getExistingAttributeValue("compositeKey");

        SLM_FATAL_IF( "Sorry, attribute \"onEvent\" is missing", !item->hasAttribute("onEvent") );
        std::string onEvent = item->getExistingAttributeValue("onEvent");

        SLM_FATAL_IF( "Sorry, attribute \"fromUID\" is missing", !item->hasAttribute("fromUID") );
        std::string fromUID = item->getExistingAttributeValue("fromUID");

        SLM_FATAL_IF( "Sorry, attribute \"typeMsg\" is missing", !item->hasAttribute("msgType") );
        std::string msgType = item->getExistingAttributeValue("msgType");

        OSLM_INFO( "Manage event "<< onEvent <<" from this object "<< fromUID <<".");
        EventType managedEvent (compositeKey, onEvent, fromUID, msgType);
        m_managedEvents.push_back( managedEvent );
        //addNewHandledEvent( onEvent );
    }
}

//-----------------------------------------------------------------------------

void MsgForwarderSrv::updating() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void MsgForwarderSrv::info( std::ostream &_sstream )
{
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
