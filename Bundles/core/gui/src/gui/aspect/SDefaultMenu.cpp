/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/aspect/SDefaultMenu.hpp"

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwGui::IMenuSrv, ::gui::aspect::SDefaultMenu );

namespace gui
{

namespace aspect
{

//-----------------------------------------------------------------------------

SDefaultMenu::SDefaultMenu() noexcept
{
}

//-----------------------------------------------------------------------------

SDefaultMenu::~SDefaultMenu() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefaultMenu::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::starting()
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::stopping()
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::updating()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}

}
