/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <wx/app.h>
#include <wx/wx.h>

#include <fwTools/UUID.hpp>

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
#include "gui/aspect/DefaultMenuBarSrv.hpp"


REGISTER_SERVICE( ::fwGui::IMenuBarSrv , ::gui::aspect::DefaultMenuBarSrv , ::fwTools::Object );

namespace gui
{

namespace aspect
{


//-----------------------------------------------------------------------------

DefaultMenuBarSrv::DefaultMenuBarSrv() throw()
{}

//-----------------------------------------------------------------------------

DefaultMenuBarSrv::~DefaultMenuBarSrv() throw()
{}

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
    SLM_FATAL("ACH : an DefaultMenuBarSrv service does not be updated." );
}

//-----------------------------------------------------------------------------

void DefaultMenuBarSrv::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_FATAL("ACH : an DefaultMenuBarSrv service does not received a message." );
}

//-----------------------------------------------------------------------------

}

}
