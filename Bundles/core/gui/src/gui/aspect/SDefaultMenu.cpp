/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/aspect/SDefaultMenu.hpp"

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwGui::IMenuSrv, ::gui::aspect::SDefaultMenu, ::fwData::Object );

namespace gui
{

namespace aspect
{

//-----------------------------------------------------------------------------

SDefaultMenu::SDefaultMenu() throw()
{
}

//-----------------------------------------------------------------------------

SDefaultMenu::~SDefaultMenu() throw()
{
}

//-----------------------------------------------------------------------------

void SDefaultMenu::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultMenu::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}

}
