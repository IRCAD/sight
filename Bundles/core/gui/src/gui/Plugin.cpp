/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/intl.h>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwCore/base.hpp>
#include <fwData/Object.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "gui/Plugin.hpp"
#include "gui/Manager.hpp"
#include "gui/App.hpp"

#include "gui/action/NothingAction.hpp"
#include "gui/action/RunRootObjectServiceAction.hpp"
#include "gui/action/SelectConfigurationAction.hpp"
#include "gui/action/TestAction.hpp"
#include "gui/action/CloseAction.hpp"
#include "gui/action/QuitAction.hpp"
#include "gui/action/StarterActionService.hpp"
#include "gui/action/FullScreenActionService.hpp"

#include "gui/sample/ITestService.hpp"
#include "gui/sample/DummyTest.hpp"

#include "gui/aspect/IAspect.hpp"
#include "gui/aspect/DefaultAspect.hpp"
#include "gui/aspect/IMenu.hpp"
#include "gui/aspect/DefaultMenu.hpp"
#include "gui/aspect/IToolBar.hpp"
#include "gui/aspect/DefaultToolBar.hpp"
#include "gui/view/IView.hpp"
#include "gui/view/DefaultView.hpp"

#include "gui/editor/IEditor.hpp"
#include "gui/editor/DummyEditor.hpp"

namespace gui
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::gui::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
	// registration of basic action services.
	REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::NothingAction , ::fwTools::Object ) ;
	REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::RunRootObjectServiceAction , ::fwTools::Object ) ;
	REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::CloseAction , ::fwTools::Object ) ;
	REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::QuitAction , ::fwTools::Object ) ;
	REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::SelectConfigurationAction , ::fwTools::Object ) ;
	REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::TestAction , ::fwTools::Object ) ;
	REGISTER_SERVICE( ::gui::action::IAction, ::gui::action::StarterActionService, ::fwTools::Object );
	REGISTER_SERVICE( ::gui::action::IAction, ::gui::action::FullScreenActionService, ::fwTools::Object );

	// registration of basic editor services.
	REGISTER_SERVICE( ::gui::editor::IEditor , ::gui::editor::DummyEditor , ::fwTools::Object ) ;

	// registration of basic view service.
	REGISTER_SERVICE( ::gui::view::IView , ::gui::view::DefaultView , ::fwTools::Object ) ;
	// registration of basic aspect service.
	REGISTER_SERVICE( ::gui::aspect::IAspect , ::gui::aspect::DefaultAspect , ::fwTools::Object ) ;
	// registration of basic menu service.
	REGISTER_SERVICE( ::gui::aspect::IMenu , ::gui::aspect::DefaultMenu , ::fwTools::Object ) ;
	// registration of basic toolbar service.
	REGISTER_SERVICE( ::gui::aspect::IToolBar , ::gui::aspect::DefaultToolBar , ::fwTools::Object ) ;

	// registration of basic example service.
	REGISTER_SERVICE( ::gui::sample::ITestService , ::gui::sample::DummyTest , ::fwTools::Object ) ;



	SLM_TRACE("starting gui bundle");


	// verify if the command line and profile.xml are well formed
	startMethodPreconditionTest();


	if ( ( wxTheApp && wxTheApp->wxAppConsole::argc <= 3 ) || !wxTheApp )
	{
		std::string objectClassName(  this->getBundle()->getParameterValue("rootObject") ) ;
		std::string objectConfigurationName( this->getBundle()->getParameterValue("config") ) ;
		std::string objectConfigurationFile( this->getBundle()->getParameterValue("configFile") ) ;

		::fwServices::OSR::setRootObjectClassName( objectClassName ) ;
		::fwServices::OSR::setRootObjectConfigurationName(objectConfigurationName) ;

		if( this->getBundle()->hasParameter("configFile") )
		{
			::fwServices::OSR::setRootObjectConfigurationFile(objectConfigurationFile) ;
		}
	}

	if( this->getBundle()->hasParameter("Aspect") )
	{
		std::string id = this->getBundle()->getParameterValue("Aspect") ;
		OSLM_TRACE("Starting GUI: specified aspect is " << id ) ;
		::gui::Manager::setGlobalAspectInformations( std::pair< bool , std::string >( true , id ));
	}
	else
	{
		SLM_TRACE("Starting GUI: No aspect specified" ) ;
	}

	if( this->getBundle()->hasParameter("startingMode") )
	{
		::gui::Manager::initialize() ;
	}
	else
	{
		// Dummy parameters
		int argcc = 1 ;
		char **argvv = new char*[argcc];
		std::string text = "test";
		argvv[0] = const_cast< char * >( text.c_str() );

		SLM_TRACE("Starting GUI") ;
		::wxEntry( argcc , argvv ) ;
	}
}

void Plugin::startMethodPreconditionTest()
{
	// Precondition tests if the launcher is used

	// 1.  launcher.exe (default profile.xml)
	// in this case, the rootObject and configName must be declared in default profile.xml
	if ( wxTheApp && wxTheApp->wxAppConsole::argc == 1 )
	{
		SLM_ASSERT("rootObject and config parameter missing",
				this->getBundle()->hasParameter("rootObject") && this->getBundle()->hasParameter("config"));
	}

	// 2.  launcher.exe myProfile.xml (root object and config must be declared in gui activation )
	// in this case, the rootObject and configName must be declared in myProfile.xml
	if ( wxTheApp 	&& wxTheApp->wxAppConsole::argc == 2 )
	{
		SLM_ASSERT("rootObject and config parameter missing",
				this->getBundle()->hasParameter("rootObject") && this->getBundle()->hasParameter("config"));
	}

	// 3.  launcher.exe profile.xml rootObjectName configName
	// if the rootObject and configName are given in command line, they must not be declared in the profile.xml file.
	if ( wxTheApp && wxTheApp->wxAppConsole::argc == 4 )
	{
		SLM_ASSERT("rootObject and config parameter must not be declared in profile",
				!this->getBundle()->hasParameter("rootObject") && !this->getBundle()->hasParameter("config"));
	}

	//Launcher mode
	// test if the argument line is well formed.
	if ( wxTheApp )
	{
		assert( wxTheApp->wxAppConsole::argc != 3 && wxTheApp->wxAppConsole::argc <= 4);
	}

	//the startingMode must be defined in profile.xml
	if ( wxTheApp )
	{
		SLM_ASSERT("startingMode parameter missing", this->getBundle()->hasParameter("startingMode") );
	}

	// Precondition tests if the Clauncher is used (TODO)

}

void Plugin::stop() throw()
{}

} // namespace operators
