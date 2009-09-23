/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>


#include "gui/action/NothingAction.hpp"

namespace gui
{
namespace action
{
REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::NothingAction , ::fwTools::Object ) ;


NothingAction::NothingAction() throw()
{
}

//-----------------------------------------------------------------------------

NothingAction::~NothingAction() throw() 
{
}

//-----------------------------------------------------------------------------

void NothingAction::info(std::ostream &_sstream )
{
	_sstream << "Nothing Action" << std::endl;
}

//-----------------------------------------------------------------------------

void NothingAction::updating() throw( ::fwTools::Failed )
{
	SLM_TRACE("NothingAction::update") ;
}
}
}

