/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Object.hpp>

#include "gui/aspect/IMenu.hpp"
#include "gui/aspect/IAspect.hpp"
#include "gui/aspect/IToolBar.hpp"
#include "gui/action/IAction.hpp"
#include "gui/action/NothingAction.hpp"
#include "gui/Manager.hpp"


namespace gui
{

namespace aspect
{

IAspect::IAspect() throw():
	m_name("NO NAME"),
	m_minSizeHeight(-1),
	m_minSizeWidth(-1)
{}

//---------------------------------------------------------------------------

IAspect::~IAspect() throw()
{}

//---------------------------------------------------------------------------

std::string IAspect::getName()
{
	return m_name ;
}


//---------------------------------------------------------------------------

void IAspect::configuring() throw( ::fwTools::Failed )
{
	SLM_TRACE("IAspect::configuring");
   	assert( m_configuration->getName() == "aspect" || m_configuration->getName() == "service" );
   	::fwRuntime::ConfigurationElementContainer::Iterator iter ;
   	for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
   	{
   		SLM_ASSERT( "actions tag is deprecated", (*iter)->getName() != "actions" );

   		if( (*iter)->getName() == "name" )
   		{
   			m_name = (*iter)->getValue();
   		}

   		if( (*iter)->getName() == "icon" )
   		{
   			OSLM_TRACE("icon: "<< (*iter)->getValue());
   			m_icon = ::boost::shared_ptr< wxIcon >(createIcon((*iter)->getValue()));
   		}

   		if( (*iter)->getName() == "minSize" )
   		{
   			//TODO: it should be useful to verify the config is well formed...
   			//assert((*iter)->hasAttribute("height") && (*iter)->hasAttribute("width") );
   			//m_minSizeHeight = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("height").c_str());
   			//m_minSizeWidth = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("width").c_str());

   			if( (*iter)->hasAttribute("height") )
   			{
   				m_minSizeHeight = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("height").c_str());
   			}
   			if( (*iter)->hasAttribute("width") )
   			{
   				m_minSizeWidth = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("width").c_str());
   			}
   		}

   		if( (*iter)->getName() == "menus" )
   		{
   			::fwRuntime::ConfigurationElementContainer::Iterator menu ;
		   	for( menu = (*iter)->begin() ; menu != (*iter)->end() ; ++menu )
		   	{
		   		SLM_ASSERT("menu tag missing", (*menu)->getName() == "menu" );
		   		SLM_ASSERT("uid attribute missing", (*menu)->hasAttribute("uid"));
		   		m_menusUUID.push_back( (*menu)->getExistingAttributeValue("uid") ) ;
		   	}
   		}

   		if( (*iter)->getName() == "toolBar" )
   		{
   			::gui::aspect::IToolBar::sptr service = ::fwServices::add< ::gui::aspect::IToolBar >(this->getObject(),"::gui::aspect::DefaultToolBar"); ;
   			service->setConfiguration( *iter ) ;
   			service->configure() ;
   			service->start() ;
   		}
   	}
}

//---------------------------------------------------------------------------

void IAspect::starting() throw(::fwTools::Failed)
{
	// To update name
	::gui::Manager::registerAspect( ::boost::dynamic_pointer_cast< ::gui::aspect::IAspect >( shared_from_this() ) ) ;

	// Create MenuBar
	SLM_TRACE("No menu bar : creating it") ;
	wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
	SLM_ASSERT( "No wxFrame", frame ) ;

	if(frame->GetMenuBar())
	{
		SLM_ASSERT( "MenuBar must be null or empty", frame->GetMenuBar()->GetMenuCount() == 0 );
	}
	else
	{
		frame->SetMenuBar(new wxMenuBar());
	}

	// Starting menus (do this before actions)
	std::vector< ::gui::aspect::IMenu::sptr > allMenus = ::fwServices::OSR::getServices< ::gui::aspect::IMenu >() ;
	for(std::vector< std::string >::iterator iterUUID = m_menusUUID.begin() ; iterUUID != m_menusUUID.end() ; ++iterUUID )
	{
		bool menuIsFound = false;
		for( 	std::vector< ::boost::shared_ptr< ::gui::aspect::IMenu > >::iterator iterMenu = allMenus.begin();
				iterMenu != allMenus.end() && ! menuIsFound ;
				++iterMenu )
		{
			if( (*iterMenu)->getUUID() == *iterUUID )
			{
				(*iterMenu)->start();
				menuIsFound = true;
			}
		}
		SLM_ASSERT("Menu Not Found", menuIsFound);
	}
}
//---------------------------------------------------------------------------

void IAspect::stopping() throw(::fwTools::Failed)
{
	// To update name
	::gui::Manager::unregisterAspect( ::boost::dynamic_pointer_cast< ::gui::aspect::IAspect >( shared_from_this() ) ) ;

	// Stopping menus (do this after actions)
	std::vector< ::boost::shared_ptr< ::gui::aspect::IMenu > > allMenus = ::fwServices::OSR::getServices< ::gui::aspect::IMenu >() ;
	for(std::vector< std::string >::iterator iterUUID = m_menusUUID.begin() ; iterUUID != m_menusUUID.end() ; ++iterUUID )
	{
		bool menuIsFound = false;
		for( 	std::vector< ::gui::aspect::IMenu::sptr >::iterator iterMenu = allMenus.begin();
				iterMenu != allMenus.end() && ! menuIsFound ;
				++iterMenu )
		{
			if( (*iterMenu)->getUUID() == *iterUUID )
			{
				(*iterMenu)->stop();
				menuIsFound = true;
			}
		}
		SLM_ASSERT("Menu Not Found", menuIsFound);
	}

}

//---------------------------------------------------------------------------

void IAspect::info(std::ostream &_sstream )
{
	_sstream << "Manage aspect in main GUI application" ;
}

//---------------------------------------------------------------------------

::boost::shared_ptr< wxIcon > IAspect::getIcon()
{
	return m_icon;
}

//---------------------------------------------------------------------------

wxSize IAspect::getMinSize()
{
	return wxSize(m_minSizeWidth, m_minSizeHeight);
}

//---------------------------------------------------------------------------

wxIcon* IAspect::createIcon(std::string _imagePath) const throw(::fwTools::Failed)
{
	wxIcon	* result = 0;
	::boost::filesystem::path imagePath(_imagePath);
	std::string	imageFileExtension( ::boost::filesystem::extension( imagePath ) );
	if( imageFileExtension == ".ico" )
	{
		// Creates the new icon directly from the image path.
		result = new wxIcon(
			wxConvertMB2WX( imagePath.native_file_string().c_str() ),
			wxBITMAP_TYPE_ICO );

		// Ensure that the icon is valid.
		if( result->Ok() == false )
		{
			std::string	message;
			message += imagePath.string();
			message += ": unbale to create an icon instance from the file.";
			throw ::fwTools::Failed(message);
		}
	}
	return result;
}
}
}

