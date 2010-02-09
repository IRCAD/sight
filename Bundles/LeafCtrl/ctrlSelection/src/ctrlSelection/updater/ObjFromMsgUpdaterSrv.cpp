/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Composite.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include "ctrlSelection/updater/ObjFromMsgUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IUpdaterSrv, ::ctrlSelection::updater::ObjFromMsgUpdaterSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

ObjFromMsgUpdaterSrv::ObjFromMsgUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

ObjFromMsgUpdaterSrv::~ObjFromMsgUpdaterSrv() throw()
{}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    for (   ManagedEvents::iterator it = m_managedEvents.begin();
            it != m_managedEvents.end();
            ++it )
    {
        // First element of tuple ( it->get<0>() ) is the event, test if message correspond to a defined event
        if( _msg->hasEvent( it->get<0>() ) )
        {
            ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast( _msg->getSubject().lock() );
            SLM_ASSERT(obj,"Sorry, the subject of message is not a ::fwData::Object");

            // Test if we manage this event from this object message uid ( it->get<1>() )
            if( obj->getUUID() == it->get<1>() )
            {
                // Udapte the composite object referenced by the composite key ( it->get<2>() )
                updateComposite( obj, it->get<2>(), it->get<3>() );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::updateComposite( ::fwData::Object::sptr _obj, std::string _compositeKey, ActionType _action )
{

    ::fwData::Composite::sptr pComposite = this->getObject< ::fwData::Composite >();

    // Manage pre-condition
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must not exist in composite on action : " << _action, ( _action == ADD ) && ( pComposite->getRefMap().find(_compositeKey) != pComposite->getRefMap().end() ) );
    OSLM_FATAL_IF( "Sorry the composite key " << _compositeKey << " must exist in composite on action : " << _action, ( _action == REMOVE || _action == SWAP ) && ( pComposite->getRefMap().find(_compositeKey) == pComposite->getRefMap().end() ) );

    // Manage special action
    if ( _action == ADD_OR_SWAP )
    {
        if ( pComposite->getRefMap().find(_compositeKey) != pComposite->getRefMap().end() )
        {
            _action = SWAP;
        }
        else
        {
            _action = ADD;
        }
    }

    // Message & parameters messages
    ::fwComEd::CompositeMsg::NewSptr compositeMsg;
    std::vector< std::string > modifiedFields;
    std::vector< ::fwData::Object::sptr > oldObjects;

    switch ( _action )
    {
    case REMOVE :
    {
        // Backup
        ::fwData::Object::sptr objBackup = pComposite->getRefMap()[ _compositeKey ];
        oldObjects.push_back( objBackup );

        // Message
        modifiedFields.push_back(_compositeKey);

        // Composite modification
        pComposite->getRefMap().erase( _compositeKey );
        break;
    }
    case SWAP :
    {
        // Backup
        ::fwData::Object::sptr objBackup = pComposite->getRefMap()[ _compositeKey ];
        oldObjects.push_back( objBackup );

        // Message
        modifiedFields.push_back(_compositeKey);

        // Composite modification
        pComposite->getRefMap()[ _compositeKey ] = _obj;
        break;
    }
    case ADD :
    {
        // Push temp object (hack, necessary for composite message)
        ::fwData::Object::NewSptr pTmpObj;
        oldObjects.push_back( pTmpObj );

        // Message
        modifiedFields.push_back(_compositeKey);

        // Composite modification
        pComposite->getRefMap()[ _compositeKey ] = _obj;
        break;
    }
    default :
    {
        SLM_FATAL("Sorry, this action type is not managed");
        break;
    }
    }

    // Notification of message
    compositeMsg->addEventModifiedFields(modifiedFields,oldObjects);
    ::fwServices::IEditionService::notify(this->getSptr(), pComposite, compositeMsg);

}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::configuring()  throw ( ::fwTools::Failed )
{

    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer handleEvents = m_configuration->findAllConfigurationElement("update");

    SLM_ASSERT("Problem with configuration for ObjFromMsgUpdaterSrv type, missing element \"update\"", handleEvents.size() != 0 );
    OSLM_DEBUG( "handleEvents.size() = " << handleEvents.size() );
    for(    ::fwRuntime::ConfigurationElementContainer::Iterator item = handleEvents.begin();
            item != handleEvents.end();
            ++item )
    {
        SLM_FATAL_IF( "Sorry, attribute \"compositeKey\" is missing", !(*item)->hasAttribute("compositeKey") );
        std::string compositeKey =  (*item)->getExistingAttributeValue("compositeKey");

        SLM_FATAL_IF( "Sorry, attribute \"onEvent\" is missing", !(*item)->hasAttribute("onEvent") );
        std::string onEvent =  (*item)->getExistingAttributeValue("onEvent");

        SLM_FATAL_IF( "Sorry, attribute \"fromUID\" is missing", !(*item)->hasAttribute("fromUID") );
        std::string fromUID =  (*item)->getExistingAttributeValue("fromUID");

        SLM_FATAL_IF( "Sorry, attribute \"actionType\" is missing", !(*item)->hasAttribute("actionType") );
        std::string actionType =  (*item)->getExistingAttributeValue("actionType");

        ActionType action;
        if ( actionType == "ADD" )              { action = ADD; }
        else if ( actionType == "SWAP" )        { action = SWAP; }
        else if ( actionType == "REMOVE" )      { action = REMOVE; }
        else if ( actionType == "ADD_OR_SWAP" ) { action = ADD_OR_SWAP; }
        else
        {
            SLM_FATAL("Sorry this type of \"actionType\" is not managed by ObjFromMsgUpdaterSrv type");
        }

        OSLM_INFO( "Manage event "<< onEvent <<" from this object "<< fromUID <<" and "<< actionType << " "<< compositeKey <<" in my composite.");
        ::boost::tuple< std::string, std::string, std::string, ActionType > managedEvent (onEvent, fromUID, compositeKey, action);
        m_managedEvents.push_back( managedEvent );
        addNewHandledEvent( onEvent );
    }
}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void ObjFromMsgUpdaterSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // updater
} // ctrlSelection
