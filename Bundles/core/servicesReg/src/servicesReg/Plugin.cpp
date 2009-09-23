/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Object.hpp>

#include <fwData/Composite.hpp>
#include <fwData/ProcessObject.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/DefaultEditor.hpp>
#include <fwServices/ObjectXMLParser.hpp>

#include <fwComEd/objectConfigurationParser.hpp>

#include "servicesReg/Plugin.hpp"

namespace servicesReg
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::servicesReg::Plugin");

Plugin::~Plugin() throw()
{}

void Plugin::start() throw( ::fwRuntime::RuntimeException )
{
	// Services of fwServices library
	REGISTER_SERVICE( ::fwServices::IXMLParser , 		::fwServices::ObjectXMLParser , 	::fwTools::Object ) ;
	REGISTER_SERVICE( ::fwServices::IEditionService, 	::fwServices::DefaultEditor, 		::fwTools::Object    ) ;

	// Services of fwComEd library
	REGISTER_SERVICE( ::fwServices::IXMLParser , 		::fwComEd::ICompositeParser , 		::fwData::Composite );
	REGISTER_SERVICE( ::fwServices::IXMLParser , 		::fwComEd::IProcessObjectParser ,  	::fwData::ProcessObject );

}

void Plugin::stop() throw()
{}

} // namespace servicesReg
