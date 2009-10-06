/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Object.hpp>

#include "gui/aspect/DefaultAspect.hpp"
#include "gui/aspect/IMenu.hpp"
#include "gui/view/IView.hpp"
#include "gui/Manager.hpp"


namespace gui
{
namespace aspect
{

REGISTER_SERVICE( ::gui::aspect::IAspect , ::gui::aspect::DefaultAspect , ::fwTools::Object ) ;


DefaultAspect::DefaultAspect() throw()
{
}

//-----------------------------------------------------------------------------

DefaultAspect::~DefaultAspect() throw()
{
}

//-----------------------------------------------------------------------------

void DefaultAspect::configuring() throw( ::fwTools::Failed )
{
	this->::gui::aspect::IAspect::configuring();
   	::fwRuntime::ConfigurationElementContainer::Iterator iter ;
   	for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
   	{
   		if( (*iter)->getName() == "views" )
   		{
   			::fwRuntime::ConfigurationElementContainer::Iterator view ;
		   	for( view = (*iter)->begin() ; view != (*iter)->end() ; ++view )
		   	{
		   		if( (*view)->getName() == "view" )
		   		{
		   			assert( (*view)->hasAttribute("guiContainerId")) ;
		   			::gui::view::IView::sptr service = ::fwServices::add< ::gui::view::IView >( this->getObject() ) ;
		   			service->setConfiguration( (*view) ) ;
		   			service->configure();
		   			m_views.push_back(service) ;
		   		}
		   	}
   		}
   	}
}

//-----------------------------------------------------------------------------

void DefaultAspect::starting() throw(::fwTools::Failed)
{
	this->::gui::aspect::IAspect::starting();
	// Starting views
	for(std::vector< ::gui::view::IView::sptr >::iterator iter = m_views.begin() ; iter != m_views.end() ; ++iter )
	{
		(*iter)->start();
	}
}

//-----------------------------------------------------------------------------

void DefaultAspect::stopping() throw(::fwTools::Failed)
{
	// To update name
	this->::gui::aspect::IAspect::stopping();
	for(std::vector< ::gui::view::IView::sptr >::iterator iter = m_views.begin() ; iter != m_views.end() ; ++iter )
	{
		(*iter)->stop();
	}
}

//-----------------------------------------------------------------------------

void DefaultAspect::info(std::ostream &_sstream )
{
	_sstream << "Manage aspect in main GUI application" ;
}

//-----------------------------------------------------------------------------

void DefaultAspect::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultAspect::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

}
}

