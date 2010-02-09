/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

IUpdaterSrv::IUpdaterSrv()
{}

//-----------------------------------------------------------------------------

IUpdaterSrv::~IUpdaterSrv()
{}

//-----------------------------------------------------------------------------

void IUpdaterSrv::configureManagedEvents(::fwRuntime::ConfigurationElement::sptr configuration)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElementContainer handleEvents = configuration->findAllConfigurationElement("update");

    SLM_ASSERT("Problem with configuration for ObjFromMsgUpdaterSrv type, missing element \"update\"", handleEvents.size() != 0 );
    OSLM_DEBUG( "handleEvents.size() = " << handleEvents.size() );
    m_managedEvents.clear();
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

void IUpdaterSrv::updateComposite(::fwData::Composite::sptr pComposite, ::fwData::Object::sptr _obj, std::string _compositeKey, ActionType _action )
{
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

}
