/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/wrapper/MsgWrapperSrv.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/Base.hpp>

#include <fwComEd/GraphMsg.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>


fwServicesRegisterMacro( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::MsgWrapperSrv, ::fwData::Object );

namespace ctrlSelection
{

namespace wrapper
{

//-----------------------------------------------------------------------------

MsgWrapperSrv::MsgWrapperSrv() throw()
{
    //TODO addNewHandledEvent( ::fwServices:: ObjectMsg::NEW_OBJECT );
    //handlingEventOff ::fwServices::ObjectMsg::UPDATED_OBJECT );
    //TODO addNewHandledEvent( ::fwServices:: ObjectMsg::DELETE_OBJECT );
}

//-----------------------------------------------------------------------------

MsgWrapperSrv::~MsgWrapperSrv() throw()
{
}

//-----------------------------------------------------------------------------

void MsgWrapperSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer handleEvents = m_configuration->findAllConfigurationElement("wrapper");

    SLM_ASSERT("Problem with configuration for ObjToCompositeMsgForwarderSrv type, missing element \"wrapper\"",
               handleEvents.size() != 0 );
    m_managedEvents.clear();
    BOOST_FOREACH( ::fwRuntime::ConfigurationElementContainer::Container::value_type item,handleEvents.getElements())
    {
        SLM_FATAL_IF( "Sorry, attribute \"onEvent\" is missing", !item->hasAttribute("onEvent") );
        std::string onEvent = item->getExistingAttributeValue("onEvent");

        SLM_FATAL_IF( "Sorry, attribute \"toEvent\" is missing", !item->hasAttribute("toEvent") );
        std::string toEvent = item->getExistingAttributeValue("toEvent");

        SLM_FATAL_IF( "Sorry, attribute \"toMsgType\" is missing", !item->hasAttribute("toMsgType") );
        std::string msgType = item->getExistingAttributeValue("toMsgType");

        OSLM_INFO( "Manage event "<< onEvent <<" to " << toEvent << ".");
        EventType managedEvent ( onEvent, toEvent, msgType);
        m_managedEvents.push_back( managedEvent );
        //addNewHandledEvent( onEvent );
    }
}
//-----------------------------------------------------------------------------

void MsgWrapperSrv::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    BOOST_FOREACH( EventType item, m_managedEvents)
    {
        std::string onEvent = item.get<0>();
        std::string toEvent = item.get<1>();
        std::string msgType = item.get<2>();

        if(message->hasEvent( onEvent ))
        {
            ::fwServices::ObjectMsg::sptr wrappedMsg = ::fwServices::factory::message::New(msgType);
            OSLM_ASSERT(msgType << " creation failed", wrappedMsg);
            wrappedMsg->addEvent(toEvent, message->getDataInfo(onEvent));
            wrappedMsg->setSource( this->getSptr());
            wrappedMsg->setSubject( this->getObject());
            ::fwData::Object::ObjectModifiedSignalType::sptr sig;
            sig = this->getObject()->signal< ::fwData::Object::ObjectModifiedSignalType >(
                ::fwData::Object::s_OBJECT_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
                sig->asyncEmit( wrappedMsg);
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
