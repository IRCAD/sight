/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/fwID.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "gui/action/StarterActionService.hpp"

namespace gui
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::gui::action::StarterActionService , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

StarterActionService::StarterActionService() throw()
{}

//-----------------------------------------------------------------------------

StarterActionService::~StarterActionService() throw()
{}

//-----------------------------------------------------------------------------

void StarterActionService::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void StarterActionService::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    typedef std::pair< std::string, ActionType > ServiceUidPair;
    BOOST_FOREACH( ServiceUidPair serviceUid, m_uuidServices)
    {
        bool srv_exists = ::fwTools::fwID::exist(serviceUid.first );
        if (srv_exists)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( serviceUid.first ) ;
            if (service->isStarted())
            {
                service->stop();
            }
        }
    }

    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void StarterActionService::info(std::ostream &_sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void StarterActionService::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;

    for(size_t i = 0; i < m_uuidServices.size(); i++)
    {
        ActionType action =  m_uuidServices.at(i).second;
        std::string uid =  m_uuidServices.at(i).first;
        bool srv_exists = ::fwTools::fwID::exist(uid  );

        // Manage special action
        if ( action == START_IF_EXISTS )
        {
            if ( srv_exists )
            {
                action = START;
            }
            else
            {
                action = DO_NOTHING;
            }
        }
        else if( action == STOP_IF_EXISTS )
        {
            if ( srv_exists )
            {
                action = STOP;
            }
            else
            {
                action = DO_NOTHING;
            }
        }

        if( action != DO_NOTHING)
        {
            ::fwServices::IService::sptr service = ::fwServices::get( uid ) ;
            SLM_ASSERT("service not found", service);
            switch ( action )
            {
            case START_OR_STOP:
            {
                if(service->isStopped())
                {
                    service->start();
                    service->update();
                }
                else
                {
                    service->stop();
                }
                break;
            }
            case START :
            {
                if(service->isStopped())
                {
                    service->start();
                }
                else
                {
                    OSLM_WARN("Service " << service->getID() << " is not stopped");
                }
                service->update();
                break;
            }
            case STOP :
            {
                service->stop();
                break;
            }
            default :
            {
                SLM_FATAL("Sorry, this action type is not managed");
                break;
            }
            }
        }
        else
        {
            std::string msgInfo = "Sorry, the service is unavailable.";
            ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Service unavailable");
            messageBox.setMessage( msgInfo );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();

            OSLM_INFO("Do nothing for Service " << m_uuidServices.at(i).first);
        }
    }
}

//-----------------------------------------------------------------------------

void StarterActionService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void StarterActionService::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();

    BOOST_FOREACH(ConfigurationType actionCfg, m_configuration->getElements() )
    {
        OSLM_INFO( "StarterActionService "  << actionCfg->getName());

        std::string actionType =  actionCfg->getName();
        ActionType action;
        if ( actionType == "start" )                { action = START; }
        else if ( actionType == "stop" )            { action = STOP; }
        else if ( actionType == "start_or_stop" )   { action = START_OR_STOP; }
        else if ( actionType == "start_if_exists" ) { action = START_IF_EXISTS; }
        else if ( actionType == "stop_if_exists" )  { action = STOP_IF_EXISTS; }
        else
        {
            OSLM_WARN("Sorry this type of \"actionType\":" << actionType <<" is not managed by StarterActionService");
            continue;
        }
        SLM_ASSERT("Attribute uid missing", actionCfg->hasAttribute("uid")) ;
        std::string uuid = actionCfg->getExistingAttributeValue("uid") ;

        m_uuidServices.push_back( std::make_pair(uuid, action) );
    }
}

//-----------------------------------------------------------------------------

} // namespace action
} // namespace gui

