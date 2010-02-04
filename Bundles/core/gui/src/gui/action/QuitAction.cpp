/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/window.h>

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "gui/action/QuitAction.hpp"

namespace gui
{
namespace action
{
REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::QuitAction , ::fwTools::Object ) ;

QuitAction::QuitAction() throw()
{
}

//-----------------------------------------------------------------------------

QuitAction::~QuitAction() throw() 
{
}

//-----------------------------------------------------------------------------

void QuitAction::info(std::ostream &_sstream )
{
	_sstream << "Quit Action" << std::endl;
}

//-----------------------------------------------------------------------------

void QuitAction::updating() throw( ::fwTools::Failed )
{
	wxBeginBusyCursor();
	::fwServices::OSR::uninitializeRootObject();
	wxEndBusyCursor();
	wxTheApp->GetTopWindow()->Close();

}
}
}
