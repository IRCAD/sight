/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwWX/convert.hpp>

#include "gui/action/IAction.hpp"
#include "gui/aspect/IMenu.hpp"
#include "gui/aspect/DefaultMenuSrv.hpp"


REGISTER_SERVICE( ::fwGui::IMenuSrv , ::gui::aspect::DefaultMenuSrv , ::fwTools::Object );

namespace gui
{

namespace aspect
{
const std::string  DefaultMenuSrv::SEPARATOR_UID = "SEPARATOR_UID";

//-----------------------------------------------------------------------------

DefaultMenuSrv::DefaultMenuSrv() throw()
{}

//-----------------------------------------------------------------------------

DefaultMenuSrv::~DefaultMenuSrv() throw()
{}

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

void DefaultMenuSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}

}
