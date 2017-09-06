/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/aspect/SDefaultMenuBar.hpp"

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

fwServicesRegisterMacro( ::fwGui::IMenuBarSrv, ::gui::aspect::SDefaultMenuBar, ::fwData::Object );

namespace gui
{

namespace aspect
{


//-----------------------------------------------------------------------------

SDefaultMenuBar::SDefaultMenuBar() noexcept
{
}

//-----------------------------------------------------------------------------

SDefaultMenuBar::~SDefaultMenuBar() noexcept
{
}

//-----------------------------------------------------------------------------

void SDefaultMenuBar::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void SDefaultMenuBar::starting()
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void SDefaultMenuBar::stopping()
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDefaultMenuBar::updating()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}

}
