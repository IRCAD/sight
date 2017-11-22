/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SDefaultButton.hpp"

namespace gui
{
namespace action
{
fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SDefaultButton );

SDefaultButton::SDefaultButton() noexcept
{
}

//-----------------------------------------------------------------------------

SDefaultButton::~SDefaultButton() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefaultButton::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultButton::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SDefaultButton::stopping()
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SDefaultButton::updating()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDefaultButton::info(std::ostream& _sstream )
{
    _sstream << "Default button" << std::endl;
}

}
}

