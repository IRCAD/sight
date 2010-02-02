/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/frame.h>
#include <string>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/helper.hpp>
#include <fwCore/base.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <boost/lexical_cast.hpp>

#include "gui/editor/IEditor.hpp"

namespace gui
{
namespace editor
{


IEditor::IEditor() throw() :
    m_container( 0 ),
    m_guiContainerId( std::pair< bool , int >(false,0)),
    m_isContainerLocallyCreated( false )
{}

IEditor::~IEditor() throw()
{}

void IEditor::configuring() throw( ::fwTools::Failed )
{
	if( m_configuration->findConfigurationElement("win") )
	{
		SLM_TRACE("IEditor:: find win") ;
		std::string id = m_configuration->findConfigurationElement("win")->getExistingAttributeValue("guiContainerId") ;
		m_guiContainerId = std::pair< bool , int >(true, ::boost::lexical_cast<int >(id)) ;
	}
}

void IEditor::info(std::ostream &_sstream )
{
}

void IEditor::starting() throw(::fwTools::Failed)
{
	// Precondition management
	::fwTools::Object::sptr associatedObject = this->getObject();

	if( m_guiContainerId.first )
	{
		OSLM_TRACE( "Finding gui container from IDENTIFIER " <<  m_guiContainerId.second ) ;
		m_container = wxWindow::FindWindowById( m_guiContainerId.second ) ;
		assert( m_container ) ;
	}

	if( m_container == 0 )
	{
		std::stringstream msg;
		msg << "Tracking service cannot be started for " << associatedObject->className() << " (adr: " << associatedObject.get() << ")" << std::endl;
		msg << "WxContainer not specified : using application top one" << std::endl;
		SLM_WARN( msg.str() );
		m_container = new wxFrame(wxTheApp->GetTopWindow(),-1, wxConvertMB2WX( "IEditoreringService" ));
		m_container->Show();
		m_container->Refresh();
		m_isContainerLocallyCreated = true ;
	}

//	fwServices::registerCommunicationChannel(this->getObject() ,this->getSptr() )->start();

	// Postcondition assertions
	assert( m_container ) ;
}
void IEditor::stopping() throw(::fwTools::Failed)
{
	// Do not destroy m_container children either refresh
	// This is delegated to delegate which manage uninstallation of installed subwindows, such as the vtk one.
	if( m_isContainerLocallyCreated && m_container != 0 )
	{
		SLM_DEBUG("Destroying container") ;
		m_container->Destroy();
		m_container = 0 ;
	}
}

void IEditor::setWxContainer( wxWindow * container )
{
	m_container = container ;
}

wxWindow * IEditor::getWxContainer()
{
	assert( m_container ) ;
	return m_container ;
}

}
}

