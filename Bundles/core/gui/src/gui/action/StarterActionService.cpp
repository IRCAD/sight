/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/UUID.hpp>

#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>


#include "gui/action/StarterActionService.hpp"


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
		::fwServices::IService::sptr service = ::fwServices::get( m_uuidServices.at(i).first ) ;
		SLM_ASSERT("service not found", service);
		if(m_uuidServices.at(i).second)
		{
			service->start();
			service->update();
		}
		else
		{
			service->stop();
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
		if( (*iter)->getName() == "start" )
		{
			SLM_ASSERT("attribute uid missing", (*iter)->hasAttribute("uid")) ;
			std::string uuid = (*iter)->getExistingAttributeValue("uid") ;
			m_uuidServices.push_back( std::make_pair(uuid, true) );
		}
		if( (*iter)->getName() == "stop" )
		{
			SLM_ASSERT("attribute uid missing", (*iter)->hasAttribute("uid")) ;
			std::string uuid = (*iter)->getExistingAttributeValue("uid") ;
			m_uuidServices.push_back( std::make_pair(uuid, false) );
		}
	}
}

}
}
