/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/msgdlg.h>

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/UUID.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwWX/convert.hpp>

#include "gui/action/StarterActionService.hpp"
#include <fwGui/MessageDialog.hpp>


namespace gui
{
namespace action
{

REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::StarterActionService , ::fwTools::Object ) ;


StarterActionService::StarterActionService() throw()
{
}

//-----------------------------------------------------------------------------

StarterActionService::~StarterActionService() throw()
{
}

//-----------------------------------------------------------------------------

void StarterActionService::info(std::ostream &_sstream )
{
    _sstream << "Starter Action" << std::endl;
}

//-----------------------------------------------------------------------------

void StarterActionService::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE("updating StarterActionService") ;
    this->::gui::action::IAction::updating();

    for(size_t i = 0; i < m_uuidServices.size(); i++)
    {
        ActionType action =  m_uuidServices.at(i).second;
        std::string uid =  m_uuidServices.at(i).first;
        bool srv_exists = ::fwTools::UUID::exist(uid, ::fwTools::UUID::SIMPLE );

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
                    OSLM_WARN("Service " << service->getUUID() << " is not stopped");
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
            ::fwGui::IMessageDialog::Icons icon = ::fwGui::IMessageDialog::WARNING;
            ::fwGui::MessageDialog messageBox;
            messageBox.setTitle("Service unavailable");
            messageBox.setMessage( msgInfo );
            messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::IMessageDialog::OK);
            messageBox.show();

            OSLM_INFO("Do nothing for Service " << m_uuidServices.at(i).first);
        }
    }
}

//-----------------------------------------------------------------------------

void StarterActionService::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE("configuring StarterActionService") ;
    this->::gui::action::IAction::configuring() ;

    ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin() ;
    for( ; iter != this->m_configuration->end() ; ++iter )
    {
        OSLM_INFO( "StarterActionService "  << (*iter)->getName());

        std::string actionType =  (*iter)->getName();
        ActionType action;
        if ( actionType == "start" )                { action = START; }
        else if ( actionType == "stop" )            { action = STOP; }
        else if ( actionType == "start_or_stop" )   { action = START_OR_STOP; }
        else if ( actionType == "start_if_exists" ) { action = START_IF_EXISTS; }
        else if ( actionType == "stop_if_exists" )  { action = STOP_IF_EXISTS; }
        else
        {
            OSLM_FATAL("Sorry this type of \"actionType\":" << actionType <<" is not managed by StarterActionService");
        }
        SLM_ASSERT("Attribute uid missing", (*iter)->hasAttribute("uid")) ;
        std::string uuid = (*iter)->getExistingAttributeValue("uid") ;

        m_uuidServices.push_back( std::make_pair(uuid, action) );
    }
}

}
}
