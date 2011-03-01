/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwServices/macros.hpp>
//#include <fwServices/RootManager.hpp>

#include <fwGui/Cursor.hpp>

#include "guiWx/action/QuitAction.hpp"

namespace gui
{
namespace action
{

REGISTER_SERVICE( ::fwGui::IActionSrv , ::gui::action::QuitAction , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

QuitAction::QuitAction() throw()
{}

//-----------------------------------------------------------------------------

QuitAction::~QuitAction() throw()
{}

//-----------------------------------------------------------------------------

void QuitAction::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC() ;
    this->initialize();
}

//-----------------------------------------------------------------------------

void QuitAction::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void QuitAction::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void QuitAction::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void QuitAction::info(std::ostream &_sstream )
{
    _sstream << "Quit Action" << std::endl;
}

//-----------------------------------------------------------------------------

void QuitAction::updating() throw( ::fwTools::Failed )
{
    ::fwGui::Cursor cursor;
    cursor.setCursor(::fwGui::ICursor::BUSY);

    wxExit();

    cursor.setDefaultCursor();
}

//-----------------------------------------------------------------------------

}
}
