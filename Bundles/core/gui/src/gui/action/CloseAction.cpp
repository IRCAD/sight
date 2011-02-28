/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwServices/macros.hpp>
//#include <fwServices/RootManager.hpp>

#include <fwGui/Cursor.hpp>

#include "gui/action/CloseAction.hpp"

namespace gui
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::gui::action::CloseAction , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

CloseAction::CloseAction() throw()
{}

//-----------------------------------------------------------------------------

CloseAction::~CloseAction() throw()
{}

//-----------------------------------------------------------------------------

void CloseAction::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();
}

//-----------------------------------------------------------------------------

void CloseAction::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void CloseAction::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void CloseAction::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void CloseAction::info(std::ostream &_sstream )
{
    _sstream << "Close Action" << std::endl;
}

//-----------------------------------------------------------------------------

void CloseAction::updating() throw( ::fwTools::Failed )
{
    ::fwGui::Cursor cursor;
    cursor.setCursor(::fwGui::ICursor::BUSY);
    //::fwServices::RootManager::uninitializeRootObject();
    cursor.setDefaultCursor();
}

//-----------------------------------------------------------------------------

}
}

