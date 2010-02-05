/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwServices/macros.hpp>
#include <fwRuntime/helper.hpp>
#include <fwData/Object.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Extension.hpp>

#include "gui/action/CloseAction.hpp"

namespace gui
{
namespace action
{
REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::CloseAction , ::fwTools::Object ) ;


CloseAction::CloseAction() throw()
{}

//-----------------------------------------------------------------------------

CloseAction::~CloseAction() throw() 
{}

//-----------------------------------------------------------------------------

void CloseAction::info(std::ostream &_sstream )
{
    _sstream << "Close Action" << std::endl;
}

//-----------------------------------------------------------------------------

void CloseAction::updating() throw( ::fwTools::Failed )
{
    ::fwServices::OSR::uninitializeRootObject();
}

}
}
