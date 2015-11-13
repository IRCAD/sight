/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "gui/aspect/DefaultMenuSrv.hpp"


fwServicesRegisterMacro( ::fwGui::IMenuSrv, ::gui::aspect::DefaultMenuSrv, ::fwData::Object );

namespace gui
{

namespace aspect
{

//-----------------------------------------------------------------------------

DefaultMenuSrv::DefaultMenuSrv() throw()
{
}

//-----------------------------------------------------------------------------

DefaultMenuSrv::~DefaultMenuSrv() throw()
{
}

//-----------------------------------------------------------------------------

void DefaultMenuSrv::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void DefaultMenuSrv::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void DefaultMenuSrv::stopping() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void DefaultMenuSrv::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}

}
