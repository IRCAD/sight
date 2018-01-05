/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SDefault.hpp"

namespace gui
{
namespace action
{
fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SDefault );

SDefault::SDefault() noexcept
{
}

//-----------------------------------------------------------------------------

SDefault::~SDefault() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefault::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefault::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SDefault::stopping()
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SDefault::updating()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDefault::info(std::ostream& _sstream )
{
    _sstream << "Default button" << std::endl;
}

}
}

