/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/aspect/DefaultMenuBarSrv.hpp"

#include <fwTools/fwID.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Runtime.hpp>

fwServicesRegisterMacro( ::fwGui::IMenuBarSrv, ::gui::aspect::DefaultMenuBarSrv, ::fwData::Object );

namespace gui
{

namespace aspect
{


//-----------------------------------------------------------------------------

DefaultMenuBarSrv::DefaultMenuBarSrv() throw()
{
}

//-----------------------------------------------------------------------------

DefaultMenuBarSrv::~DefaultMenuBarSrv() throw()
{
}

//-----------------------------------------------------------------------------

void DefaultMenuBarSrv::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void DefaultMenuBarSrv::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void DefaultMenuBarSrv::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void DefaultMenuBarSrv::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}

}
