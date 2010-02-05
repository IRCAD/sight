/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>
#include <wx/wx.h>

#include <fwCore/base.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include "gui/action/FullScreenActionService.hpp"

namespace gui
{
namespace action
{

REGISTER_SERVICE( ::gui::action::IAction , ::gui::action::FullScreenActionService , ::fwTools::Object ) ;

FullScreenActionService::FullScreenActionService() throw(): m_isFullScreen(false)
{
}

//-----------------------------------------------------------------------------

FullScreenActionService::~FullScreenActionService() throw()
{
}

//-----------------------------------------------------------------------------

void FullScreenActionService::configuring() throw( ::fwTools::Failed )
{
        SLM_INFO("FullScreenActionService::configuring");
        this->::gui::action::IAction::configuring();
}

//-----------------------------------------------------------------------------

void FullScreenActionService::updating() throw(::fwTools::Failed)
{
        SLM_INFO("FullScreenActionService::updating");
        this->::gui::action::IAction::updating();
        wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
//      full-screen mode:
//    * wxFULLSCREEN_NOMENUBAR
//    * wxFULLSCREEN_NOTOOLBAR
//    * wxFULLSCREEN_NOSTATUSBAR
//    * wxFULLSCREEN_NOBORDER
//    * wxFULLSCREEN_NOCAPTION
//    * wxFULLSCREEN_ALL (all of the above)
        m_isFullScreen = !m_isFullScreen;
        frame->ShowFullScreen(m_isFullScreen, wxFULLSCREEN_NOMENUBAR|wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION);
}

//-----------------------------------------------------------------------------

void FullScreenActionService::info(std::ostream &_sstream )
{
        _sstream << "FullScreen Action" << std::endl;
}

}

}



