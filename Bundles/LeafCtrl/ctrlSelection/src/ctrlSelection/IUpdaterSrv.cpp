/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/fwID.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/helper/Composite.hpp>

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
        if ( actionType == "ADD" )                    { action = ADD; }
        else if ( actionType == "SWAP" )              { action = SWAP; }
        else if ( actionType == "REMOVE" )            { action = REMOVE; }
        else if ( actionType == "ADD_OR_SWAP" )       { action = ADD_OR_SWAP; }
        else if ( actionType == "REMOVE_IF_PRESENT" ) { action = REMOVE_IF_PRESENT; }
        else if ( actionType == "DO_NOTHING" )        { action = DO_NOTHING; }
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

    // Manage special action
    if ( _action == ADD_OR_SWAP )
    {
        if ( pComposite->find(_compositeKey) != pComposite->end() )
        {
            _action = SWAP;
        }
        else
        {
            _action = ADD;
        }
    }
    else if(_action == REMOVE_IF_PRESENT )
    {
        if ( pComposite->find(_compositeKey) != pComposite->end() )
        {
            _action = REMOVE;
        }
        else
        {
            _action = DO_NOTHING;
        }
    }

    // Use helper on composite
    ::boost::shared_ptr< ::fwComEd::helper::Composite > pCompositeHelper ( new ::fwComEd::helper::Composite( pComposite ) );

    if(_action != DO_NOTHING)
    {
        switch ( _action )
        {
        case REMOVE :
        {
            pCompositeHelper->remove(_compositeKey);
            break;
        }
        case SWAP :
        {
            pCompositeHelper->swap(_compositeKey,_obj);
            break;
        }
        case ADD :
        {
            pCompositeHelper->add(_compositeKey,_obj);
            break;
        }
        default :
        {
            SLM_FATAL("Sorry, this action type is not managed");
            break;
        }
        }

        // Notification of message
        pCompositeHelper->notify( this->getSptr() );
    }
    else
    {
        OSLM_INFO("Do nothing for objectID " << _compositeKey);
    }
}

}
