/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SQuit.hpp"

#include <fwCore/base.hpp>

#include <fwGui/Application.hpp>
#include <fwGui/Cursor.hpp>

#include <fwServices/macros.hpp>

namespace gui
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SQuit, ::fwData::Object );

//-----------------------------------------------------------------------------

SQuit::SQuit() noexcept
{
}

//-----------------------------------------------------------------------------

SQuit::~SQuit() noexcept
{
}

//-----------------------------------------------------------------------------

void SQuit::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void SQuit::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SQuit::stopping()
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SQuit::info(std::ostream &_sstream )
{
    _sstream << "Quit Action" << std::endl;
}

//-----------------------------------------------------------------------------

void SQuit::updating()
{
    ::fwGui::Cursor cursor;
    cursor.setCursor(::fwGui::ICursor::BUSY);
    ::fwGui::Application::New()->exit(0);
    cursor.setDefaultCursor();
}

//-----------------------------------------------------------------------------

}
}
