/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/action/SDoNothing.hpp"

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>



namespace gui
{
namespace action
{
fwServicesRegisterMacro( ::fwGui::IActionSrv, ::gui::action::SDoNothing, ::fwData::Object );


SDoNothing::SDoNothing() noexcept
{
}

//-----------------------------------------------------------------------------

SDoNothing::~SDoNothing() noexcept
{
}

//-----------------------------------------------------------------------------

void SDoNothing::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDoNothing::starting()
{
    SLM_TRACE_FUNC();
    this->actionServiceStarting();
}

//-----------------------------------------------------------------------------

void SDoNothing::stopping()
{
    SLM_TRACE_FUNC();
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SDoNothing::updating()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SDoNothing::info(std::ostream &_sstream )
{
    _sstream << "Nothing Action" << std::endl;
}

}
}

