/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Composite.hpp>


#include <fwServices/Base.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include "ctrlSelection/wrapper/ObjToCompositeMsgForwarderSrv.hpp"

namespace ctrlSelection
{
namespace wrapper
{
//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::ObjToCompositeMsgForwarderSrv,
                         ::fwData::Composite );

//-----------------------------------------------------------------------------

ObjToCompositeMsgForwarderSrv::ObjToCompositeMsgForwarderSrv() throw()
{
}

//-----------------------------------------------------------------------------

ObjToCompositeMsgForwarderSrv::~ObjToCompositeMsgForwarderSrv() throw()
{
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    if (compositeMsg)
    {
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::ADDED_KEYS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getAddedKeys())
            {
                std::string key = elt.first;
                ::fwData::Object::sptr obj = elt.second;

                ::fwCom::Connection connection;
                connection = obj->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->connect(
                    this->slot(::fwServices::IService::s_RECEIVE_SLOT));
                m_objConnections[key] = connection;
            }
        }
        else if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::CHANGED_KEYS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getNewChangedKeys())
            {
                std::string key = elt.first;
                ::fwData::Object::sptr obj = elt.second;

                m_objConnections[key].disconnect();
                ::fwCom::Connection connection;
                connection = obj->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->connect(
                    this->slot(::fwServices::IService::s_RECEIVE_SLOT));
                m_objConnections[key] = connection;
            }
        }
        else if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::REMOVED_KEYS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getRemovedKeys())
            {
                std::string key = elt.first;

                m_objConnections[key].disconnect();
                m_objConnections.erase(key);
            }
        }
    }
    else
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        BOOST_FOREACH( EventType item, m_managedEvents)
        {
            std::string fromKey = item.get<0>();
            std::string event   = item.get<1>();
            std::string msgType = item.get<2>();

            if(  message->isA(msgType))
            {
                if(fromKey == "*" ||
                   (composite->find(fromKey) != composite->end() &&
                    (*composite)[fromKey] == message->getSubject().lock()))
                {
                    if(event == "*"  )
                    {
                        ::fwServices::ObjectMsg::constCast(message)->setSource( this->getSptr());
                        ::fwServices::ObjectMsg::constCast(message)->setSubject( composite);
                        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
                        sig = composite->signal< ::fwData::Object::ObjectModifiedSignalType >(
                            ::fwData::Object::s_OBJECT_MODIFIED_SIG);
                        {
                            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
                            sig->asyncEmit( ::fwServices::ObjectMsg::constCast(message) );
                        }
                    }
                    else if(message->hasEvent( event ))
                    {

                        ::fwServices::ObjectMsg::sptr forwardMsg = ::fwServices::factory::message::New(msgType);
                        OSLM_ASSERT(msgType << " creation failed", forwardMsg);
                        forwardMsg->addEvent(event, message->getDataInfo(event));
                        forwardMsg->setSource( this->getSptr());
                        forwardMsg->setSubject( composite);
                        ::fwData::Object::ObjectModifiedSignalType::sptr sig;
                        sig = composite->signal< ::fwData::Object::ObjectModifiedSignalType >(
                            ::fwData::Object::s_OBJECT_MODIFIED_SIG);
                        {
                            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
                            sig->asyncEmit( forwardMsg);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::starting()  throw ( ::fwTools::Failed )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    BOOST_FOREACH(::fwData::Composite::value_type elt, *composite)
    {
        std::string key = elt.first;
        ::fwData::Object::sptr obj = elt.second;

        ::fwCom::Connection connection;
        connection = obj->signal(::fwData::Object::s_OBJECT_MODIFIED_SIG)->connect(
            this->slot(::fwServices::IService::s_RECEIVE_SLOT));
        m_objConnections[key] = connection;
    }
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::stopping()  throw ( ::fwTools::Failed )
{
    BOOST_FOREACH(ObjConnectionMap::value_type elt, m_objConnections)
    {
        m_objConnections[elt.first].disconnect();
    }
    m_objConnections.clear();
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::swapping()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer handleEvents = m_configuration->findAllConfigurationElement("forward");

    SLM_ASSERT("Problem with configuration for ObjToCompositeMsgForwarderSrv type, missing element \"forward\"",
               handleEvents.size() != 0 );
    m_managedEvents.clear();
    BOOST_FOREACH( ::fwRuntime::ConfigurationElementContainer::Container::value_type item,handleEvents.getElements())
    {
        SLM_FATAL_IF( "Sorry, attribute \"fromKey\" is missing", !item->hasAttribute("fromKey") );
        std::string fromKey = item->getExistingAttributeValue("fromKey");

        SLM_FATAL_IF( "Sorry, attribute \"onEvent\" is missing", !item->hasAttribute("onEvent") );
        std::string onEvent = item->getExistingAttributeValue("onEvent");

        SLM_FATAL_IF( "Sorry, attribute \"typeMsg\" is missing", !item->hasAttribute("msgType") );
        std::string msgType = item->getExistingAttributeValue("msgType");

        OSLM_INFO( "Manage event "<< onEvent <<" from object " << fromKey << ".");
        EventType managedEvent ( fromKey, onEvent, msgType);
        m_managedEvents.push_back( managedEvent );
        //addNewHandledEvent( onEvent );
    }
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::updating() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::info( std::ostream &_sstream )
{
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
