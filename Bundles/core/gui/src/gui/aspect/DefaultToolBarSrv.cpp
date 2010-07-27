/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>
#include <fwServices/Base.hpp>

#include "gui/aspect/DefaultToolBarSrv.hpp"

REGISTER_SERVICE( ::fwGui::IToolBarSrv , ::gui::aspect::DefaultToolBarSrv , ::fwTools::Object ) ;

namespace gui
{

namespace aspect
{

DefaultToolBarSrv::DefaultToolBarSrv() throw()
{}

//-----------------------------------------------------------------------------

DefaultToolBarSrv::~DefaultToolBarSrv() throw()
{}

//-----------------------------------------------------------------------------

void DefaultToolBarSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create();
}

//-----------------------------------------------------------------------------

void DefaultToolBarSrv::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void DefaultToolBarSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//-----------------------------------------------------------------------------

void DefaultToolBarSrv::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultToolBarSrv::updating(::fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultToolBarSrv::info( std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

}

}
