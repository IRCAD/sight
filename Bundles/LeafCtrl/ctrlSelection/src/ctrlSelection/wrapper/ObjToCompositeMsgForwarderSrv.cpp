/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Composite.hpp>


#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/CompositeMsg.hpp>

#include "ctrlSelection/wrapper/ObjToCompositeMsgForwarderSrv.hpp"

namespace ctrlSelection
{
namespace wrapper
{
//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::ObjToCompositeMsgForwarderSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

ObjToCompositeMsgForwarderSrv::ObjToCompositeMsgForwarderSrv() throw()
{}

//-----------------------------------------------------------------------------

ObjToCompositeMsgForwarderSrv::~ObjToCompositeMsgForwarderSrv() throw()
{}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    if (compositeMsg)
    {
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::ADDED_FIELDS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getAddedFields())
            {
                std::string key = elt.first;
                ::fwData::Object::sptr obj = elt.second;

                ::fwServices::IService::sptr comChannel = ::fwServices::registerCommunicationChannel(obj ,this->getSptr());
                comChannel->start();
                m_objComChannel[key] = comChannel;
            }
        }
        else if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::SWAPPED_FIELDS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getSwappedNewFields())
            {
                std::string key = elt.first;
                ::fwData::Object::sptr obj = elt.second;

                ::fwServices::IService::sptr comChannel = m_objComChannel[key].lock();
                comChannel->stop();
                ::fwServices::OSR::unregisterService(comChannel);

                comChannel = ::fwServices::registerCommunicationChannel(obj ,this->getSptr());
                comChannel->start();
                m_objComChannel[key] = comChannel;
            }
        }
        else if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::REMOVED_FIELDS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getRemovedFields())
            {
                std::string key = elt.first;

                ::fwServices::IService::sptr comChannel = m_objComChannel[key].lock();
                comChannel->stop();
                ::fwServices::OSR::unregisterService(comChannel);
            }
        }
    }
    else
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        BOOST_FOREACH( EventType item, m_managedEvents)
        {
            std::string fromKey      = item.get<0>();
            std::string event        = item.get<1>();
            std::string msgType      = item.get<2>();

            if(  message->isA(msgType))
            {
                if(fromKey == "*" || (composite->find(fromKey) != composite->end() && (*composite)[fromKey] == message->getSubject().lock()))
                {
                    if(event == "*"  )
                    {
                        ::fwServices::IEditionService::notify( this->getSptr(), composite, ::fwServices::ObjectMsg::constCast(message) );
                    }
                    else if(message->hasEvent( event ))
                    {

                        ::fwTools::Object::sptr msg = ::fwTools::Factory::New(msgType);
                        OSLM_ASSERT(msgType << " creation failed", msg);
                        ::fwServices::ObjectMsg::sptr forwardMsg = ::fwServices::ObjectMsg::dynamicCast(msg);
                        forwardMsg->addEvent(event, message->getDataInfo(event));
                        ::fwServices::IEditionService::notify( this->getSptr(), composite, forwardMsg);
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

        ::fwServices::IService::sptr comChannel = ::fwServices::registerCommunicationChannel(obj ,this->getSptr());
        comChannel->start();
        m_objComChannel[key] = comChannel;
    }
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::stopping()  throw ( ::fwTools::Failed )
{
    BOOST_FOREACH(ObjComChannelMap::value_type elt, m_objComChannel)
    {
        ::fwServices::IService::sptr comChannel = elt.second.lock();
        comChannel->stop();
        ::fwServices::OSR::unregisterService(comChannel);
    }
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::swapping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer handleEvents = m_configuration->findAllConfigurationElement("forward");

    SLM_ASSERT("Problem with configuration for ObjToCompositeMsgForwarderSrv type, missing element \"forward\"", handleEvents.size() != 0 );
    m_managedEvents.clear();
    BOOST_FOREACH( ::fwRuntime::ConfigurationElementContainer::Container::value_type item ,handleEvents.getElements())
    {
        SLM_FATAL_IF( "Sorry, attribute \"fromKey\" is missing", !item->hasAttribute("fromKey") );
        std::string fromKey =  item->getExistingAttributeValue("fromKey");

        SLM_FATAL_IF( "Sorry, attribute \"onEvent\" is missing", !item->hasAttribute("onEvent") );
        std::string onEvent =  item->getExistingAttributeValue("onEvent");

        SLM_FATAL_IF( "Sorry, attribute \"typeMsg\" is missing", !item->hasAttribute("msgType") );
        std::string msgType =  item->getExistingAttributeValue("msgType");

        OSLM_INFO( "Manage event "<< onEvent <<" from object " << fromKey << ".");
        EventType managedEvent ( fromKey, onEvent, msgType);
        m_managedEvents.push_back( managedEvent );
        //addNewHandledEvent( onEvent );
    }
}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ObjToCompositeMsgForwarderSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
