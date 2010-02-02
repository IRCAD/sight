/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "gui/action/RunRootObjectServiceAction.hpp"

namespace gui
{
namespace action
{

REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::RunRootObjectServiceAction , ::fwTools::Object ) ;

RunRootObjectServiceAction::RunRootObjectServiceAction() throw()
{
}

//-----------------------------------------------------------------------------

RunRootObjectServiceAction::~RunRootObjectServiceAction() throw()
{
}

//-----------------------------------------------------------------------------

void RunRootObjectServiceAction::info(std::ostream &_sstream )
{
	_sstream << "RunRootObjectService Action" << std::endl;
}

//-----------------------------------------------------------------------------

void RunRootObjectServiceAction::updating() throw( ::fwTools::Failed )
{
	::fwTools::Object::sptr rootObject = ::fwServices::OSR::getRootObject();
	SLM_ASSERT("Root object not specified", rootObject );

	if( !::fwServices::support( rootObject , "::gui::editor::IEditor" ) )
	{
		SLM_WARN("Root object does not support ::gui::editor::IEditor") ;
	}
	else
	{
		::fwServices::IService::sptr service = ::fwServices::get( rootObject , "::gui::editor::IEditor" ) ;
		SLM_ASSERT("service not found", service ) ;
		service->start() ;
		service->update() ;
		service->stop() ;
	}
}
}
}

